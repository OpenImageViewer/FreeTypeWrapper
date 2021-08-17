#pragma once

#ifdef __clang__
	#pragma clang diagnostic push

	#pragma clang diagnostic ignored "-Wdocumentation"
	#pragma clang diagnostic ignored "-Wdocumentation-pedantic"
	#pragma clang diagnostic ignored "-Wmicrosoft-enum-value"
	#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
	#pragma clang diagnostic ignored "-Wold-style-cast"
	#pragma clang diagnostic ignored "-Wlanguage-extension-token"

#endif


#include <ft2build.h>
#include <freetype/ftstroke.h>
#include <freetype/ftlcdfil.h>
#include <string>

#ifdef __clang__
	#pragma clang diagnostic pop
#endif


using u8string = std::basic_string<char>;
