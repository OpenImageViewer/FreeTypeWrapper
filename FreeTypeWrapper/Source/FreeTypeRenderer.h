#pragma once
#include <cstdint>
#include <LLUtils/Color.h>
#include <LLUtils/Buffer.h>
class FreeTypeRenderer
{
public:
    struct BitmapProperties
    {
        uint32_t width;
        uint32_t height;
        uint32_t numChannels;
        uint32_t bitsPerChannel;
        uint32_t rowpitchInBytes;
    };

    struct GlyphRGBAParams
    {
        FT_BitmapGlyph bitmapGlyph;
        LLUtils::Color backgroudColor;
        LLUtils::Color textColor;
        BitmapProperties bitmapProperties;
    };


    static BitmapProperties GetBitmapGlyphProperties(const FT_Bitmap bitmap);
    static LLUtils::Buffer RenderGlyphToBuffer(const GlyphRGBAParams& params);
    
};
