#pragma once
#include "FreeTypeHeaders.h"
#include <string>
#include <LLUtils/Exception.h>
#include <LLUtils/StringUtility.h>
#include <limits>
namespace FreeType
{
    class FreeTypeFont
    {
    public:
        FreeTypeFont(FT_Library ftLibrary, const string_type& fileName)
        {
            fName = fileName;
            fLibrary = ftLibrary;

            if (fileName.empty())
                LL_EXCEPTION(LLUtils::Exception::ErrorCode::InvalidState, "Font file path must be specified");

#if LLUTILS_PLATFORM == LLUTILS_PLATFORM_WIN32
            // The bundled ft system calls CreateFileA. Keep this legacy code-page
            // boundary here; StringUtility narrow strings always mean UTF-8.
            std::string nativePath;
            const UINT codePage = AreFileApisANSI() ? GetACP() : GetOEMCP();
            if (codePage == CP_UTF8)
                nativePath = LLUtils::StringUtility::ConvertString<std::string>(fName);
            else
            {
                // Borrow native UTF-16 in the viewer. Standalone narrow builds supply
                // UTF-8, which must first be decoded for this legacy Windows boundary.
                const auto widePath = []<class Char>(std::basic_string_view<Char> text)
                {
                    if constexpr (std::is_same_v<Char, wchar_t>)
                        return text;
                    else
                        return LLUtils::StringUtility::ConvertString<std::wstring>(text);
                }(std::basic_string_view(fName));
                if (widePath.size() > static_cast<size_t>((std::numeric_limits<int>::max)()))
                    LL_EXCEPTION(LLUtils::Exception::ErrorCode::RuntimeError, "Font path is too long");
                const int length = static_cast<int>(widePath.size());
                BOOL substituted = FALSE;
                const int size   = WideCharToMultiByte(codePage, WC_NO_BEST_FIT_CHARS, widePath.data(), length, nullptr,
                                                       0, nullptr, &substituted);
                if (size == 0 || substituted)
                    LL_EXCEPTION(LLUtils::Exception::ErrorCode::RuntimeError,
                                 "Font path cannot be represented by the Windows file API code page");
                nativePath.resize(static_cast<size_t>(size));
                if (WideCharToMultiByte(codePage, WC_NO_BEST_FIT_CHARS, widePath.data(), length, nativePath.data(),
                                        size, nullptr, &substituted) != size ||
                    substituted)
                    LL_EXCEPTION(LLUtils::Exception::ErrorCode::RuntimeError, "Font path conversion failed");
            }
#else
            const auto& nativePath = fName;
#endif
            FT_Error error = FT_New_Face(fLibrary, nativePath.c_str(), 0, &fFace);
            if (error)
                LL_EXCEPTION(LLUtils::Exception::ErrorCode::RuntimeError, FT_Error_String(error));
        }
        ~FreeTypeFont()
        {
            FT_Error error = FT_Done_Face(fFace);
            if (error != FT_Err_Ok)
            {
                // TODO: Log en error
            }
        }

        void SetSize(uint16_t fontSize, uint16_t DPIx, uint16_t DPIy)
        {
            fFontSize = fontSize;
            FT_Error error =
                FT_Set_Char_Size(
                    fFace,    /* handle to face object           */
                    0,       /* char_width in 1/64th of points  */
                    fontSize << 6,   /* char_height in 1/64th of points */
                    DPIx,     /* horizontal device resolution    */
                    DPIy);   /* vertical device resolution      */


            if (error != FT_Err_Ok)
                LL_EXCEPTION(LLUtils::Exception::ErrorCode::RuntimeError, "FreeType error, can't set char size");
        }

        FT_Face GetFace()
        {
            return fFace;
        }



    private:
        string_type fName;
        FT_Face fFace = nullptr;
        FT_Library fLibrary = nullptr;
        uint16_t fFontSize = 0;
    };

    using FreeTypeFontUniquePtr = std::unique_ptr<FreeTypeFont>;
}