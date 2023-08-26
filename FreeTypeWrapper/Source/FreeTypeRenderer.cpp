
#include <FreeTypeHeaders.h>
#include <freetype/ftglyph.h>
#include <freetype/freetype.h>
#include "freetype/ftimage.h"
#include <LLUtils/Exception.h>
#include <LLUtils/Buffer.h>
#include <FreeTypeRenderer.h>

namespace FreeType
{
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


        const int destPixelSize = sizeof(ColorF32);
        const uint32_t HeightInPixels = params.bitmapProperties.height;
        const uint32_t widthInPixels = params.bitmapProperties.width;
        
        Color textColor = params.textColor;
        //ColorF32 textColor = static_cast<ColorF32>(params.textColor).MultiplyAlpha();

        const size_t bufferSize = widthInPixels * HeightInPixels * destPixelSize;
        LLUtils::Buffer RGBABitmap(bufferSize);

        // Fill glyph background with background color.
        ColorF32* RGBABitmapPtr = reinterpret_cast<ColorF32*>(RGBABitmap.data());
        memset(RGBABitmapPtr, 0, RGBABitmap.size());

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
                        const auto bitValue = ((bitmap.buffer[bytePos] & (1 << bitPos)) >> bitPos);// *255;

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
                            , textColorFloat.A() * static_cast<decltype(textColorFloat)::color_channel_type>(bitmap.buffer[sourceRowStart + x + 0] / 255.0)
                        };
                        
                        break;
                    }

                    break;

                case 3: // RGB
                {
                    const uint32_t currentPixelPos = sourceRowStart + x * params.bitmapProperties.numChannels;

                    const uint8_t BC = bitmap.buffer[currentPixelPos + 0];
                    const uint8_t GC = bitmap.buffer[currentPixelPos + 1];
                    const uint8_t RC = bitmap.buffer[currentPixelPos + 2];
                    const uint8_t AC = static_cast<uint8_t>(((int)RC + (int)GC + (int)BC) / 3);



                    finalColorPremul =
                    {
                              static_cast<uint8_t>((textColor.R()* RC) / 0xFF)
                            , static_cast<uint8_t>((textColor.G()* GC) / 0xFF)
                            , static_cast<uint8_t>((textColor.B()* BC) / 0xFF)
                            , static_cast<uint8_t>(AC * textColor.A() / 0xFF)
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
