#pragma once
#include <vector>
#include <string>
#include <LLUtils/Color.h>
#include "../Include/FreeTypeWrapper/FreeTypeCommon.h"

namespace FreeType
{
    struct FormattedTextEntry
    {
        LLUtils::Color textColor { 0,0,0,0 };
        //LLUtils::Color backgroundColor;
        //uint32_t size;
        //uint32_t outlineWidth;
        //uint32_t outlineColor;

        string_type text;
        static FormattedTextEntry Parse(const string_type& format, const string_type& text);
    };

    using VecFormattedTextEntry = std::vector<FormattedTextEntry>;

    class MetaText
    {
    public:
        static VecFormattedTextEntry GetFormattedText(string_type text);
    };
}