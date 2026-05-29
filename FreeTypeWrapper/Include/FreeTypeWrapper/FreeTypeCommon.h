#pragma once
#include <LLUtils/StringDefs.h>

namespace FreeType
{
    using string_type = LLUtils::native_string_type;

    enum class RenderMode
    {
        Default
        , Aliased
        , Antialiased
        , SubpixelAntiAliased
    };

}