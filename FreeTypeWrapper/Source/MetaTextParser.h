#pragma once
#include <vector>
#include <string>
#include <LLUtils/Color.h>

namespace FreeType
{
    struct FormattedTextEntry
    {
        LLUtils::Color textColor;
        //LLUtils::Color backgroundColor;
        //uint32_t size;
        //uint32_t outlineWidth;
        //uint32_t outlineColor;

        std::wstring text;
        static FormattedTextEntry Parse(const std::wstring& format, const std::wstring& text);
    };

    using VecFormattedTextEntry = std::vector<FormattedTextEntry>;

    class MetaText
    {
    public:
        static VecFormattedTextEntry GetFormattedText(std::wstring text);
    };
}