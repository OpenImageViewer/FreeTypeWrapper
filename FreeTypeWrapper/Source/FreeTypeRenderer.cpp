
#include <FreeTypeHeaders.h>
#include <freetype/ftglyph.h>
#include <freetype/freetype.h>
#include "freetype/ftimage.h"
#include <LLUtils/Exception.h>
#include <LLUtils/Buffer.h>
#include <FreeTypeRenderer.h>


    FreeTypeRenderer::BitmapProperties FreeTypeRenderer::GetBitmapGlyphProperties(const FT_Bitmap bitmap)
    {
        FreeTypeRenderer::BitmapProperties bitmapProperties;
        
        switch (bitmap.pixel_mode)
        {
        case FT_PIXEL_MODE_GRAY:
            bitmapProperties.numChannels = 1;
            break;
        case FT_PIXEL_MODE_LCD:
            bitmapProperties.numChannels = 3;
            break;
        default:
            LL_EXCEPTION_UNEXPECTED_VALUE;
        }

        bitmapProperties.bitsPerChannel = static_cast<uint32_t>(std::log2(bitmap.num_grays + 1));
        bitmapProperties.height = bitmap.rows;
        bitmapProperties.width = bitmap.width / bitmapProperties.numChannels;
        bitmapProperties.rowpitchInBytes = static_cast<uint32_t>(bitmap.pitch);

        return bitmapProperties;
    }

    LLUtils::Buffer FreeTypeRenderer::RenderGlyphToBuffer(const FreeTypeRenderer::GlyphRGBAParams& params)
    {
    using namespace LLUtils;

    FT_Bitmap bitmap = params.bitmapGlyph->bitmap;


    const int destPixelSize = 4;
    const uint32_t HeightInPixels = params.bitmapProperties.height;
    const uint32_t widthInPixels = params.bitmapProperties.width;

    LLUtils::Color backgroudColor = params.backgroudColor;
    LLUtils::Color textColor = params.textColor;



    const size_t bufferSize = widthInPixels * HeightInPixels * destPixelSize;
    LLUtils::Buffer RGBABitmap(bufferSize);

    // Fill glyph background with background color.
    uint32_t* RGBABitmapPtr = reinterpret_cast<uint32_t*>(RGBABitmap.data());
    for (uint32_t i = 0; i < widthInPixels * HeightInPixels; i++)
    {
        RGBABitmapPtr[i] = backgroudColor.colorValue;
    }

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
            switch (params.bitmapProperties.numChannels)
            {
            case 1: // MONOCHROME
                R = params.textColor.R;
                G = params.textColor.G;
                B = params.textColor.B;
                A = bitmap.buffer[sourceRowStart + x + 0];
                break;

            case 3: // RGB
            {
                uint32_t currentPixelPos = sourceRowStart + x * params.bitmapProperties.numChannels;

                uint8_t BC = bitmap.buffer[currentPixelPos + 0];
                uint8_t GC = bitmap.buffer[currentPixelPos + 1];
                uint8_t RC = bitmap.buffer[currentPixelPos + 2];

                R = (textColor.R * RC) / 0xFF;
                G = (textColor.G * GC) / 0xFF;
                B = (textColor.B * BC) / 0xFF;

                A = (RC + GC + BC) / 3;
            }
            break;
            default:
                LL_EXCEPTION_UNEXPECTED_VALUE;
            }


            LLUtils::Color source(R, G, B, A);
            RGBABitmapPtr[destPos] = LLUtils::Color(RGBABitmapPtr[destPos]).Blend(source).colorValue;
        }

        sourceRowStart += params.bitmapProperties.rowpitchInBytes;
    }
    return RGBABitmap;
}
