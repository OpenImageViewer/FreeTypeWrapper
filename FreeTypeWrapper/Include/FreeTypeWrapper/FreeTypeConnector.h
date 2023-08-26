#pragma once

#include <map>
#include <cstdint>
#include <string>

#include <LLUtils/Color.h>
#include <LLUtils/Buffer.h>
#include <LLUtils/Singleton.h>
#include <LLUtils/Rect.h>
#include <LLUtils/EnumClassBitwise.h>


#pragma region FreeType forward declerations

struct  FT_StrokerRec_;
typedef struct FT_StrokerRec_* FT_Stroker;
typedef int  FT_Error;
typedef struct FT_LibraryRec_* FT_Library;
typedef enum  FT_Render_Mode_ FT_Render_Mode;

#pragma endregion FreeType forward declerations

namespace FreeType
{
    class FreeTypeFont;
    using FreeTypeFontUniquePtr = std::unique_ptr<FreeTypeFont>;

    enum class RenderMode
    {
          Default
        , Aliased
        , Antialiased
        , SubpixelAntiAliased
    };

    enum class TextCreateFlags
    {
          None
        , UseMetaText   = 1 << 0
        , Bidirectional = 1 << 1
    };

    LLUTILS_DEFINE_ENUM_CLASS_FLAG_OPERATIONS(TextCreateFlags)

    struct TextCreateParams
    {
        std::wstring fontPath;
        std::wstring text;
        uint16_t fontSize;
        LLUtils::Color textColor;
        LLUtils::Color backgroundColor;
        LLUtils::Color outlineColor;
        uint32_t outlineWidth;
        uint32_t maxWidthPx;
        RenderMode renderMode;
        uint16_t DPIx;
        uint16_t DPIy;
        uint16_t padding;
        TextCreateFlags flags;
    };


    struct TextMesureParams
    {
        TextCreateParams createParams;
    };

    struct TextMetrics
    {
        LLUtils::RectI32 rect;
        uint32_t rowHeight;
        uint32_t totalRows;
    };


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

     /*   enum class CreateMode
        {
              None
            , CreateMetrics
            , CreateBitmap
        };
   */

        using GlyphMappings = std::vector< LLUtils::RectI32>;


        void CreateBitmap(const TextCreateParams& textCreateParams, Bitmap& out_bitmap, TextMetrics* metrics, GlyphMappings* out_glyphMapping = nullptr);

        //void CreateBitmap(const TextCreateParams& textCreateParams
        //    , CreateMode createMode
        //    , TextMetrics* out_TextMetrics
        //    , GlyphMappings* out_glyphMapping
        //    , Bitmap* out_bitmap
        //);
        
        void MeasureText(const TextMesureParams& measureParams, TextMetrics& out_metrics);

    private:

     //private member methods

        FreeTypeConnector();
        FreeTypeFont* GetOrCreateFont(const std::wstring& fontPath);
        FT_Stroker GetStroker();
        static std::string GenerateFreeTypeErrorString(std::string userMessage, FT_Error error);
        FT_Render_Mode GetRenderMode(RenderMode renderMode) const;

        template <typename source_type, typename dest_type>
        void ResolvePremultipoliedBUffer(LLUtils::Buffer& dest, const LLUtils::Buffer& source, uint32_t width, uint32_t height);
        



    private:
        FT_Library fLibrary;
        FT_Stroker fStroker = nullptr;

        std::map<std::wstring, FreeTypeFontUniquePtr> fFontNameToFont;

    };
}