
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

        //bitmapProperties.bitsPerChannel = static_cast<uint32_t>(std::log2(bitmap.num_grays + 1));
        bitmapProperties.height = bitmap.rows;
        bitmapProperties.width = bitmap.width / bitmapProperties.numChannels;
        bitmapProperties.rowpitchInBytes = static_cast<uint32_t>(bitmap.pitch);

        return bitmapProperties;
    }

    LLUtils::Buffer FreeTypeRenderer::RenderGlyphToBuffer(const FreeTypeRenderer::GlyphRGBAParams& params)
    {
        using namespace LLUtils;

        BitmapProperties bitmapProperties = FreeTypeRenderer::GetBitmapGlyphProperties(*params.bitmapGlyph);


        FT_Bitmap bitmap = *params.bitmapGlyph;


        const int destPixelSize = 4;
        const uint32_t HeightInPixels = bitmapProperties.height;
        const uint32_t widthInPixels = bitmapProperties.width;
        
        LLUtils::Color textColor = params.textColor;

        const size_t bufferSize = widthInPixels * HeightInPixels * destPixelSize;
        LLUtils::Buffer RGBABitmap(bufferSize);

        // Fill glyph background with background color.
        LLUtils::Color* RGBABitmapPtr = reinterpret_cast<LLUtils::Color*>(RGBABitmap.data());
        memset(RGBABitmapPtr, 0, RGBABitmap.size());

        uint32_t sourceRowStart = 0;

        for (uint32_t y = 0; y < bitmap.rows; y++)
        {
            for (uint32_t x = 0; x < widthInPixels; x++)
            {
                const uint32_t destPos = y * widthInPixels + x;
                std::uint8_t R;
                std::uint8_t G;
                std::uint8_t B;
                std::uint8_t A;
                switch (bitmapProperties.numChannels)
                {
                case 1:
                    switch (bitmapProperties.bitsPerChannel)
                    {
                    case 1: // MONOCHROME
                    {
                        R = params.textColor.R();
                        G = params.textColor.G();
                        B = params.textColor.B();
                        const auto bytePos = sourceRowStart +  (x / 8);
                        const auto bitPos = 8 - (x % 8) - 1;
                        const auto bitValue = ((bitmap.buffer[bytePos] & (1 << bitPos)) >> bitPos);// *255;
                        //OutputDebugStringA((std::to_string(bitValue) + "\n").c_str());
                        A = bitValue * 255;
                    }
                        break;
                    case 8: //GrayScale
                        R = params.textColor.R();
                        G = params.textColor.G();
                        B = params.textColor.B();
                        A = bitmap.buffer[sourceRowStart + x + 0];
                        break;
                    }

                    break;

                case 3: // RGB
                {
                    uint32_t currentPixelPos = sourceRowStart + x * bitmapProperties.numChannels;

                    uint8_t BC = bitmap.buffer[currentPixelPos + 0];
                    uint8_t GC = bitmap.buffer[currentPixelPos + 1];
                    uint8_t RC = bitmap.buffer[currentPixelPos + 2];

                    R = static_cast<uint8_t>((textColor.R() * RC) / 0xFF);
                    G = static_cast<uint8_t>((textColor.G() * GC) / 0xFF);
                    B = static_cast<uint8_t>((textColor.B() * BC) / 0xFF);

                    A = static_cast<uint8_t>((RC + GC + BC) / 3);
                }
                break;
                default:
                    LL_EXCEPTION_UNEXPECTED_VALUE;
                }


                LLUtils::Color source(R, G, B, A);
                RGBABitmapPtr[destPos] = LLUtils::Color(RGBABitmapPtr[destPos]).Blend(source);
            }

            sourceRowStart += bitmapProperties.rowpitchInBytes;
        }
        return RGBABitmap;
    }
}
