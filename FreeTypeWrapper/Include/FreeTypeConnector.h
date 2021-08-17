#pragma once

#include <map>
#include <cstdint>
#include <string>

#include <LLUtils/Color.h>
#include <LLUtils/Buffer.h>
#include <LLUtils/Singleton.h>


#pragma region FreeType forward declerations

class FreeTypeFont;
using FreeTypeFontUniquePtr = std::unique_ptr<FreeTypeFont>;



struct  FT_StrokerRec_;
typedef struct FT_StrokerRec_* FT_Stroker;
typedef int  FT_Error;
typedef struct FT_LibraryRec_* FT_Library;

#pragma endregion FreeType forward declerations


class FreeTypeConnector : public LLUtils::Singleton<FreeTypeConnector>
{

    ///*** workaround to solve***

    static const size_t ExtraWidth = 2;

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
        std::string fontPath;
        std::string text;
        uint16_t fontSize;
        LLUtils::Color backgroundColor;
        LLUtils::Color outlineColor;
        uint32_t outlineWidth;
        RenderMode renderMode;
        uint16_t DPIx;
        uint16_t DPIy;
    };

    struct TextMesureParams
    {
        TextCreateParams createParams;
    };

    struct TextMesureResult
    {
        uint32_t width;
        uint32_t height;
        uint32_t rowHeight;
        int32_t descender;
        uint32_t maxXAdvance;
    };

    void CreateBitmap(const TextCreateParams& textCreateParams, Bitmap & out_bitmap);
    void MesaureText(const TextMesureParams& measureParams, TextMesureResult& out_result);


private:
       // private types
   
    //private member methods

    FreeTypeConnector();
    FreeTypeFont* GetOrCreateFont(std::string fontPath);
    FT_Stroker GetStroker();
    std::string GenerateFreeTypeErrorString(std::string userMessage, FT_Error error);
    

private:
    FT_Library fLibrary;
    FT_Stroker fStroker = nullptr;

    std::map<std::string, FreeTypeFontUniquePtr> fFontNameToFont;
    
};
