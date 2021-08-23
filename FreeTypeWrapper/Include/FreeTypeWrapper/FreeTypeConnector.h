#pragma once

#include <map>
#include <cstdint>
#include <string>

#include <LLUtils/Color.h>
#include <LLUtils/Buffer.h>
#include <LLUtils/Singleton.h>
#include <LLUtils/Rect.h>


#pragma region FreeType forward declerations

struct  FT_StrokerRec_;
typedef struct FT_StrokerRec_* FT_Stroker;
typedef int  FT_Error;
typedef struct FT_LibraryRec_* FT_Library;

#pragma endregion FreeType forward declerations

namespace FreeType
{
    class FreeTypeFont;
    using FreeTypeFontUniquePtr = std::unique_ptr<FreeTypeFont>;

    class FreeTypeConnector : public LLUtils::Singleton<FreeTypeConnector>
    {
        friend class LLUtils::Singleton<FreeTypeConnector>;
    public:
        ~FreeTypeConnector();

        struct Bitmap
        {
            uint32_t width;
            uint32_t height;
            LLUtils::Buffer buffer;
            uint32_t PixelSize;
            uint32_t rowPitch;
        };

        enum class RenderMode
        {
              Default
            , Antialiased
            , SubpixelAntiAliased
        };

        struct TextCreateParams
        {
            std::wstring fontPath;
            std::wstring text;
            uint16_t fontSize;
            LLUtils::Color backgroundColor;
            LLUtils::Color outlineColor;
            uint32_t outlineWidth;
            RenderMode renderMode;
            uint16_t DPIx;
            uint16_t DPIy;
            uint16_t padding;
        };

        struct TextMesureParams
        {
            TextCreateParams createParams;
        };

        struct TextMesureResult
        {
            LLUtils::RectI32 rect;
            uint32_t rowHeight;
        };

        void CreateBitmap(const TextCreateParams& textCreateParams, Bitmap& out_bitmap);


    private:

     //private member methods

        FreeTypeConnector();
        FreeTypeFont* GetOrCreateFont(const std::wstring& fontPath);
        FT_Stroker GetStroker();
        static std::string GenerateFreeTypeErrorString(std::string userMessage, FT_Error error);
        void MeasureText(const TextMesureParams& measureParams, TextMesureResult& out_result);


    private:
        FT_Library fLibrary;
        FT_Stroker fStroker = nullptr;

        std::map<std::wstring, FreeTypeFontUniquePtr> fFontNameToFont;

    };
}