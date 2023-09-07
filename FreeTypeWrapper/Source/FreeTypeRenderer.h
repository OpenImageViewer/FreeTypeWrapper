#pragma once
#include <cstdint>
#include <LLUtils/Color.h>
#include <LLUtils/Buffer.h>
#include <FreeTypeHeaders.h>
#include <FreeTypeWrapper/FreeTypeCommon.h>

namespace FreeType
{
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

        static FT_BitmapGlyph GetStrokerGlyph(FT_Stroker stroker, FT_GlyphSlot glyphSlot, uint32_t outlineWidth, FT_Render_Mode renderMode);
        static FT_Render_Mode GetRenderMode(RenderMode renderMode);
        static BitmapProperties GetBitmapGlyphProperties(const FT_Bitmap_ bitmap);
        static LLUtils::Buffer RenderGlyphToBuffer(const GlyphRGBAParams& params);

    };
}