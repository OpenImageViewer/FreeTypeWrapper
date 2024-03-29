#pragma once

#include <map>
#include <cstdint>
#include <string>

#include <FreeTypeWrapper/FreeTypeCommon.h>

#include <LLUtils/Color.h>
#include <LLUtils/Buffer.h>
#include <LLUtils/Rect.h>
#include <LLUtils/EnumClassBitwise.h>


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


    enum class TextCreateFlags
    {
          None
          // Use meta text for inline text style, e.g. color, size
        , UseMetaText               = 1 << 0
         // Use bidirectional text RTL and LTR.
        , Bidirectional             = 1 << 1
        // Add to the end of the line several pixels as descibed in the font , usefull for fixed width fonts.
        , LineEndFixedWidth         = 1 << 2
        // Don't Generate outline bitmaps when measuring text, use an estimation.
        //, OptimizeOutlineMetrics    = 1 << 3
    };

    LLUTILS_DEFINE_ENUM_CLASS_FLAG_OPERATIONS(TextCreateFlags)

    struct TextCreateParams
    {
        std::wstring fontPath;
        std::wstring text;
        uint16_t fontSize{};
        LLUtils::Color textColor;
        LLUtils::Color backgroundColor;
        LLUtils::Color outlineColor;
        uint32_t outlineWidth{};
        uint32_t maxWidthPx{};
        RenderMode renderMode{};
        uint16_t DPIx{};
        uint16_t DPIy{};
        uint16_t padding{};
        TextCreateFlags flags{};
    };


    struct TextMesureParams
    {
        TextCreateParams createParams;
    };

    struct LineMetrics
    {
        int32_t maxGlyphHeight;
    };

    struct TextMetrics
    {
        std::vector<LineMetrics> lineMetrics;
        LLUtils::RectI32 rect;
        uint32_t rowHeight{};
        int32_t minX = std::numeric_limits<int32_t>::max();
        int32_t maxX = std::numeric_limits<int32_t>::min();
    };


    class FreeTypeConnector
    {
    public:
        FreeTypeConnector();
        ~FreeTypeConnector();


        struct Bitmap
        {
            uint32_t width{};
            uint32_t height{};
            LLUtils::Buffer buffer{};
            uint32_t PixelSize{};
            uint32_t rowPitch{};
        };



        using GlyphMappings = std::vector< LLUtils::RectI32>;

        void CreateBitmap(const TextCreateParams& textCreateParams, Bitmap& out_bitmap, TextMetrics* metrics, GlyphMappings* out_glyphMapping = nullptr);
        void MeasureText(const TextMesureParams& measureParams, TextMetrics& out_metrics);

    private:
     //private member methods

        
        FreeTypeFont* GetOrCreateFont(const std::wstring& fontPath);
        FT_Stroker GetStroker();
        static std::string GenerateFreeTypeErrorString(std::string userMessage, FT_Error error);

        template <typename source_type, typename dest_type>
        void ResolvePremultipoliedBUffer(LLUtils::Buffer& dest, const LLUtils::Buffer& source, uint32_t width, uint32_t height);


    private:
        FT_Library fLibrary;
        FT_Stroker fStroker = nullptr;

        std::map<std::wstring, FreeTypeFontUniquePtr> fFontNameToFont;

    };
}