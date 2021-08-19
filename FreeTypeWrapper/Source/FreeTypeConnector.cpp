#include <locale>
#include <vector>
#include <string>
#include <iostream>

#include <FreeTypeWrapper/FreeTypeConnector.h>
#include <FreeTypeRenderer.h>
#include <FreeTypeFont.h>

#include <LLUtils/Exception.h>
#include <LLUtils/StringUtility.h>
#include <LLUtils/Utility.h>
#include <LLUtils/Color.h>
#include <LLUtils/Buffer.h>

#include "BlitBox.h"
#include "MetaTextParser.h"
#include <fribidi.h>
#include <ww898/utf_converters.hpp>


namespace FreeType
{

    FreeTypeConnector::FreeTypeConnector()
    {

        if (FT_Error error = FT_Init_FreeType(&fLibrary); error != FT_Err_Ok)
            LL_EXCEPTION(LLUtils::Exception::ErrorCode::RuntimeError, GenerateFreeTypeErrorString("can not load glyph", error));

    }

    FreeTypeConnector::~FreeTypeConnector()
    {
        fFontNameToFont.clear();
        FT_Stroker_Done(fStroker);
        FT_Error error = FT_Done_FreeType(fLibrary);
        if (error)
        {
            //TODO: destory freetype library eariler before object destruction.
            //throw std::logic_error("Can no destroy freetype library");
        }
    }


    std::string FreeTypeConnector::GenerateFreeTypeErrorString(std::string userMessage, FT_Error  error)
    {
        using namespace std::string_literals;
        return "FreeType error: "s + FT_Error_String(error) + ", " + userMessage;
    }

    template <typename string_type>
    std::u32string bidi_string(const string_type& logical)
    {
        FriBidiParType base = FRIBIDI_PAR_ON;
        FriBidiStrIndex* ltov, * vtol;
        FriBidiLevel* levels;
        FriBidiStrIndex new_len;
        fribidi_boolean log2vis;
        std::u32string logicalUTF32 = ww898::utf::convz<char32_t>(logical);
        std::u32string visualUTF32(logicalUTF32.length(), 0);

        ltov = nullptr;
        vtol = nullptr;
        levels = nullptr;

        log2vis = fribidi_log2vis(reinterpret_cast<FriBidiChar*>(logicalUTF32.data()), logicalUTF32.length(), &base,
            /* output */
            reinterpret_cast<FriBidiChar*>(visualUTF32.data()), ltov, vtol, levels);
        
        if (!log2vis) 
            LL_EXCEPTION(LLUtils::Exception::ErrorCode::InvalidState, "Cannot process string");
        
        return visualUTF32;
    }


    void FreeTypeConnector::MesaureText(const TextMesureParams& measureParams, TextMesureResult& mesureResult)
    {
        using namespace std;
        FreeTypeFont* font = GetOrCreateFont(measureParams.createParams.fontPath);
        FT_Face face = font->GetFace();
        const std::wstring& text = measureParams.createParams.text;

        font->SetSize(measureParams.createParams.fontSize, measureParams.createParams.DPIx, measureParams.createParams.DPIy);
        const uint32_t rowHeight = static_cast<int>((static_cast<uint32_t>(face->size->metrics.height) >> 6) + measureParams.createParams.outlineWidth * 2);

        mesureResult.rect = {};

        vector<FormattedTextEntry> formattedText = MetaText::GetFormattedText(text);
        
        int penX = 0;
        int penY = 0;
        int numberOfLines = 1;
        int maxRowWidth = 0;
        
        for (const FormattedTextEntry& el : formattedText)
        {
            std::u32string visualText = bidi_string(el.text.c_str());
            
            for (const decltype(el.text)::value_type& codepoint : visualText)
            {
                if (codepoint == '\n')
                {
                    numberOfLines++;
                    maxRowWidth = (std::max)(penX, maxRowWidth);
                    penX = 0;
                    penY += rowHeight;
                    continue;
                }


                const FT_UInt glyph_index = FT_Get_Char_Index(face, codepoint);

                if (FT_Error error = FT_Load_Glyph(
                    face,          /* handle to face object */
                    glyph_index,   /* glyph index           */
                    FT_LOAD_DEFAULT); error != FT_Err_Ok)  /* load flags, see below */
                {
                    LL_EXCEPTION(LLUtils::Exception::ErrorCode::RuntimeError, GenerateFreeTypeErrorString("can not load glyph", error));
                }
                
                mesureResult.rect.LeftTop().x = std::min<int>(mesureResult.rect.LeftTop().x, face->glyph->bitmap_left + penX);
                mesureResult.rect.LeftTop().y = std::min<int>(mesureResult.rect.LeftTop().y, face->glyph->bitmap_top + penY);

                mesureResult.rect.RightBottom().x = std::max<int>(mesureResult.rect.RightBottom().x, face->glyph->bitmap_left + face->glyph->bitmap.width + penX);

                penX += face->glyph->advance.x >> 6;
            }

            mesureResult.rect.RightBottom().y = rowHeight * numberOfLines;
        }



        maxRowWidth = (std::max)(penX, maxRowWidth);

        // Add outline width to the final width of the rasterized text.
        maxRowWidth += measureParams.createParams.outlineWidth * 2;
        mesureResult.rowHeight = static_cast<uint32_t>(rowHeight);
    }

    FreeTypeFont* FreeTypeConnector::GetOrCreateFont(const std::wstring& fontPath)
    {
        FreeTypeFont* font = nullptr;

        auto it = fFontNameToFont.find(fontPath);
        if (it != fFontNameToFont.end())
        {
            font = it->second.get();
        }
        else
        {
            auto it = fFontNameToFont.emplace(fontPath, std::make_unique<FreeTypeFont>(fLibrary, fontPath));
            font = it.first->second.get();
        }

        return font;
    }


    FT_Stroker FreeTypeConnector::GetStroker()
    {
        if (fStroker == nullptr)
        {
            FT_Stroker_New(fLibrary, &fStroker);
        }
        return fStroker;
    }



    void FreeTypeConnector::CreateBitmap(const TextCreateParams& textCreateParams, Bitmap& out_bitmap)
    {
        using namespace std;

        std::wstring text = textCreateParams.text;
        const std::wstring& fontPath = textCreateParams.fontPath;
        uint16_t fontSize = textCreateParams.fontSize;
        uint32_t OutlineWidth = textCreateParams.outlineWidth;
        const LLUtils::Color outlineColor = textCreateParams.outlineColor;
        const LLUtils::Color backgroundColor = textCreateParams.backgroundColor;
        RenderMode renderMode = textCreateParams.renderMode;

        FreeTypeFont* font = GetOrCreateFont(fontPath);

        /*if (error = FT_Library_SetLcdFilter(fLibrary, FT_LCD_FILTER_LIGHT))
            LL_EXCEPTION(LLUtils::Exception::ErrorCode::LogicError, GenerateFreeTypeErrorString("can not set LCD Filter", error));*/

        font->SetSize(fontSize, textCreateParams.DPIx, textCreateParams.DPIy);

        TextMesureParams params;
        params.createParams = textCreateParams;

        const FT_Render_Mode textRenderMOde = (renderMode == RenderMode::SubpixelAntiAliased ? FT_RENDER_MODE_LCD : FT_RENDER_MODE_NORMAL);

        TextMesureResult mesaureResult;

        MesaureText(params, mesaureResult);

		//TODO: is it the right way to calculate the extra width created by the anti-aliasing ?
        if (textRenderMOde == FT_RENDER_MODE_LCD)
        {
            mesaureResult.rect.RightBottom().x += 1;
            mesaureResult.rect.LeftTop().x -= 1;
        }
        
        mesaureResult.rect = mesaureResult.rect.Infalte(OutlineWidth * 2  + params.createParams.padding * 2, OutlineWidth * 2 + params.createParams.padding * 2);

        const uint32_t destPixelSize = 4;
        const uint32_t destRowPitch = mesaureResult.rect.GetWidth() * destPixelSize;
        const uint32_t sizeOfDestBuffer = mesaureResult.rect.GetHeight() * destRowPitch;
        const bool renderOutline = OutlineWidth > 0;
        const bool renderText = true;

        LLUtils::Buffer textBuffer(sizeOfDestBuffer);

        //// when rendering with outline, the outline buffer is the final buffer, otherwise the text buffer is the final buffer.
        //Reset final text buffer to background color.

        LLUtils::Color textBackgroundBuffer = renderOutline ? 0 : backgroundColor.colorValue;
        

        for (uint32_t i = 0; i < mesaureResult.rect.GetWidth() * mesaureResult.rect.GetHeight(); i++)
            reinterpret_cast<uint32_t*>(textBuffer.data())[i] = textBackgroundBuffer.colorValue;



        LLUtils::Buffer outlineBuffer;
        BlitBox  destOutline = {};
        if (renderOutline)
        {
            outlineBuffer.Allocate(sizeOfDestBuffer);
            //Reset outline buffer to background color.
            for (uint32_t i = 0; i < mesaureResult.rect.GetWidth() * mesaureResult.rect.GetHeight(); i++)
                reinterpret_cast<uint32_t*>(outlineBuffer.data())[i] =  backgroundColor.colorValue;

            destOutline.buffer = outlineBuffer.data();
            destOutline.width = mesaureResult.rect.GetWidth();
            destOutline.height = mesaureResult.rect.GetHeight();
            destOutline.pixelSizeInbytes = destPixelSize;
            destOutline.rowPitch = destRowPitch;
        }


        BlitBox  dest {};
        dest.buffer = textBuffer.data();
        dest.width = mesaureResult.rect.GetWidth();
        dest.height = mesaureResult.rect.GetHeight();
        dest.pixelSizeInbytes = destPixelSize;
        dest.rowPitch = destRowPitch;

        
        int penX = -mesaureResult.rect.LeftTop().x;
        int penY = -mesaureResult.rect.LeftTop().y;

        
        FT_Face face = font->GetFace();
        auto descender = face->size->metrics.descender >> 6;
        int rowHeight = mesaureResult.rowHeight;

        vector<FormattedTextEntry> formattedText = MetaText::GetFormattedText(text);

        for (const FormattedTextEntry& el : formattedText)
        {
            std::u32string visualText = bidi_string(el.text.c_str());
            for (const decltype(el.text)::value_type& codepoint : visualText)
            {
                if (codepoint == L'\n')
                {
                    penX = static_cast<int>(-mesaureResult.rect.LeftTop().x);
                    penY += rowHeight;
                    continue;
                }

                const FT_UInt glyph_index = FT_Get_Char_Index(face, codepoint);
                if (FT_Error error = FT_Load_Glyph(
                    face,          /* handle to face object */
                    glyph_index,   /* glyph index           */
                    FT_LOAD_DEFAULT); error != FT_Err_Ok)/* load flags, see below */
                {
                    LL_EXCEPTION(LLUtils::Exception::ErrorCode::RuntimeError, GenerateFreeTypeErrorString("can not Load glyph", error));
                }

                auto baseVerticalPos = rowHeight + penY + descender;
				
                //Render outline

                if (renderOutline) // render outline
                {
                    //initialize stroker, so you can create outline font
                    FT_Stroker stroker = GetStroker();

                    //  2 * 64 result in 2px outline
                    FT_Stroker_Set(stroker, static_cast<FT_Fixed>(OutlineWidth * 64), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_BEVEL, 0);
                    FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
                    FT_Glyph glyph;
                    FT_Get_Glyph(face->glyph, &glyph);
                    FT_Glyph_StrokeBorder(&glyph, stroker, false, true);
                    FT_Glyph_To_Bitmap(&glyph, textRenderMOde, nullptr, true);
                    FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
                    FreeTypeRenderer::BitmapProperties bitmapProperties = FreeTypeRenderer::GetBitmapGlyphProperties(bitmapGlyph->bitmap);
               
                    LLUtils::Buffer rasterizedGlyph = FreeTypeRenderer::RenderGlyphToBuffer({ bitmapGlyph , backgroundColor,outlineColor, bitmapProperties });

                    BlitBox source = {};
                    source.buffer = rasterizedGlyph.data();
                    source.width = bitmapProperties.width;
                    source.height = bitmapProperties.height;
                    source.pixelSizeInbytes = destPixelSize;
                    source.rowPitch = destPixelSize * bitmapProperties.width;

                    destOutline.left = static_cast<uint32_t>(penX + bitmapGlyph->left);
                    destOutline.top = baseVerticalPos - bitmapGlyph->top;
                    BlitBox::Blit(destOutline, source);

                    FT_Done_Glyph(glyph);

                }
                // Render text
                if (renderText)
                {
                    FT_Glyph glyph;
                    if (FT_Error error = FT_Get_Glyph(face->glyph, &glyph))
                        LL_EXCEPTION(LLUtils::Exception::ErrorCode::RuntimeError, "FreeType error, unable to render glyph");

                    if (glyph->format != FT_GLYPH_FORMAT_BITMAP)
                    {
                        if (FT_Error error = FT_Glyph_To_Bitmap(&glyph, textRenderMOde, nullptr, true); error != FT_Err_Ok)
                            LL_EXCEPTION(LLUtils::Exception::ErrorCode::RuntimeError, "FreeType error, unable to render glyph");
                    }

                    FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
                    FreeTypeRenderer::BitmapProperties bitmapProperties = FreeTypeRenderer::GetBitmapGlyphProperties(bitmapGlyph->bitmap);

                    LLUtils::Buffer rasterizedGlyph = FreeTypeRenderer::RenderGlyphToBuffer({ bitmapGlyph , backgroundColor, el.textColor , bitmapProperties });

                    BlitBox source = {};
                    source.buffer = rasterizedGlyph.data();
                    source.width = bitmapProperties.width;
                    source.height = bitmapProperties.height;
                    source.pixelSizeInbytes = destPixelSize;
                    source.rowPitch = destPixelSize * bitmapProperties.width;

                    dest.left = penX +  bitmapGlyph->left;
                    dest.top = baseVerticalPos - bitmapGlyph->top;
                    FT_GlyphSlot  slot = face->glyph;
                    penX += slot->advance.x >> 6;

                    FT_Done_Glyph(glyph);

                   BlitBox::Blit(dest, source);
                }
            }
        }

        if (renderOutline)
        {
            //Blend text buffer onto outline buffer.
            dest.left = 0;
            dest.top = 0;
            destOutline.left = 0;
            destOutline.top = 0;
            BlitBox::Blit(destOutline, dest);
        }

        out_bitmap.width = mesaureResult.rect.GetWidth();
        out_bitmap.height = mesaureResult.rect.GetHeight();
        out_bitmap.buffer = renderOutline ? std::move(outlineBuffer) : std::move(textBuffer);
        out_bitmap.PixelSize = destPixelSize;
        out_bitmap.rowPitch = destRowPitch;

    }
}