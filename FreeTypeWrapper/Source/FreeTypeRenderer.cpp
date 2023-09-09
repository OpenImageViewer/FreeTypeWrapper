
#include <FreeTypeHeaders.h>
#include <freetype/ftglyph.h>
#include <freetype/freetype.h>
#include "freetype/ftimage.h"
#include <LLUtils/Exception.h>
#include <LLUtils/Buffer.h>
#include <FreeTypeRenderer.h>
#include <span>

namespace FreeType
{

    FT_BitmapGlyph FreeTypeRenderer::GetStrokerGlyph(FT_Stroker stroker,FT_GlyphSlot glyphSlot, uint32_t outlineWidth, FT_Render_Mode renderMode)
    {
        //  2 * 64 result in 2px outline
        FT_Stroker_Set(stroker, static_cast<FT_Fixed>(outlineWidth * 64), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_BEVEL, 0);
        FT_Glyph glyph;
        FT_Get_Glyph(glyphSlot, &glyph);
        FT_Glyph_StrokeBorder(&glyph, stroker, false, true);
        FT_Glyph_To_Bitmap(&glyph, renderMode, nullptr, true);
        FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
        return bitmapGlyph;
    }


    FT_Render_Mode FreeTypeRenderer::GetRenderMode(RenderMode renderMode)
    {
        switch (renderMode)
        {
        case RenderMode::Aliased:
            return FT_Render_Mode::FT_RENDER_MODE_MONO;
        case RenderMode::Default:
        case RenderMode::Antialiased:
            return FT_Render_Mode::FT_RENDER_MODE_NORMAL;
        case RenderMode::SubpixelAntiAliased:
            return FT_Render_Mode::FT_RENDER_MODE_LCD;
        default:
            return FT_Render_Mode::FT_RENDER_MODE_NORMAL;

        }
    }

    FreeTypeRenderer::BitmapProperties FreeTypeRenderer::GetBitmapGlyphProperties(const FT_Bitmap bitmap)
    {
        FreeTypeRenderer::BitmapProperties bitmapProperties;

        switch (bitmap.pixel_mode)
        {
        case FT_PIXEL_MODE_MONO:
            bitmapProperties.numChannels = 1;
            bitmapProperties.bitsPerChannel = 1;
            break;
        case FT_PIXEL_MODE_GRAY:
            bitmapProperties.numChannels = 1;
            bitmapProperties.bitsPerChannel = 8;
            break;
        case FT_PIXEL_MODE_LCD:
            bitmapProperties.numChannels = 3;
            bitmapProperties.bitsPerChannel = 8;
            break;
        default:
            LL_EXCEPTION_UNEXPECTED_VALUE;
        }

        bitmapProperties.height = bitmap.rows;
        bitmapProperties.width = bitmap.width / bitmapProperties.numChannels;
        bitmapProperties.rowpitchInBytes = static_cast<uint32_t>(bitmap.pitch);

        return bitmapProperties;
    }

    LLUtils::Buffer FreeTypeRenderer::RenderGlyphToBuffer(const FreeTypeRenderer::GlyphRGBAParams& params)
    {
        using namespace LLUtils;

        FT_Bitmap bitmap = params.bitmapGlyph->bitmap;
        std::span bitmapBuffer = std::span(params.bitmapGlyph->bitmap.buffer,  static_cast<size_t>(bitmap.rows * static_cast<unsigned int>(bitmap.pitch)));

        const int destPixelSize = sizeof(ColorF32);
        const uint32_t HeightInPixels = params.bitmapProperties.height;
        const uint32_t widthInPixels = params.bitmapProperties.width;
        
        Color textColor = params.textColor;

        const size_t bufferSize = widthInPixels * HeightInPixels * destPixelSize;
        LLUtils::Buffer RGBABitmap(bufferSize);
        memset(RGBABitmap.data(), 0, RGBABitmap.size());
        std::span<ColorF32,std::dynamic_extent> RGBABitmapPtr(reinterpret_cast<ColorF32*>(RGBABitmap.data()), widthInPixels * HeightInPixels);

        uint32_t sourceRowStart = 0;

        ColorF32 textColorFloat = static_cast<ColorF32>(params.textColor);
        ColorF32 finalColorPremul;

        for (uint32_t y = 0; y < bitmap.rows; y++)
        {
            for (uint32_t x = 0; x < widthInPixels; x++)
            {
                const uint32_t destPos = y * widthInPixels + x;
                switch (params.bitmapProperties.numChannels)
                {
                case 1:
                    switch (params.bitmapProperties.bitsPerChannel)
                    {
                    case 1: // MONOCHROME
                    {
                        const auto bytePos = sourceRowStart +  (x / 8);
                        const auto bitPos = 8 - (x % 8) - 1;
                        const auto bitValue = ((bitmapBuffer[bytePos] & (1 << bitPos)) >> bitPos);// *255;

                        finalColorPremul =
                        {
                              textColorFloat.R()
                            , textColorFloat.G()
                            , textColorFloat.B()
                            , static_cast<decltype(textColorFloat)::color_channel_type>(bitValue)
                        };
                            
                        

                    }
                        break;
                    case 8: //GrayScale
                        finalColorPremul =
                        {
                              textColorFloat.R()
                            , textColorFloat.G()
                            , textColorFloat.B()
                            , textColorFloat.A() * static_cast<decltype(textColorFloat)::color_channel_type>(bitmapBuffer[sourceRowStart + x + 0] / 255.0)
                        };
                        
                        break;
                    default:
                        LL_EXCEPTION_UNEXPECTED_VALUE;
                    }

                    break;

                case 3: // RGB
                {
                    const uint32_t currentPixelPos = sourceRowStart + x * params.bitmapProperties.numChannels;

                    const uint8_t BC = bitmapBuffer[currentPixelPos + 0];
                    const uint8_t GC = bitmapBuffer[currentPixelPos + 1];
                    const uint8_t RC = bitmapBuffer[currentPixelPos + 2];
                    const uint8_t AC = static_cast<uint8_t>(( static_cast<int>(RC) + static_cast<int>(GC) + static_cast<int>(BC)) / 3);
                    const uint8_t INVAC = static_cast < uint8_t>(255 - AC);

                    finalColorPremul =
                    {
                              static_cast<uint8_t>((textColor.R() * AC + BC * INVAC) / 0xFF)
                            , static_cast<uint8_t>((textColor.G() * AC + GC * INVAC) / 0xFF)
                            , static_cast<uint8_t>((textColor.B() * AC + RC * INVAC) / 0xFF)
                            , static_cast<uint8_t>(AC)
                    };
                }
                    
                break;
                default:
                    LL_EXCEPTION_UNEXPECTED_VALUE;
                }

                
                RGBABitmapPtr[destPos] = RGBABitmapPtr[destPos].BlendPreMultiplied(finalColorPremul.MultiplyAlpha());
                
            }

            sourceRowStart += params.bitmapProperties.rowpitchInBytes;
        }
        return RGBABitmap;
    }
}
