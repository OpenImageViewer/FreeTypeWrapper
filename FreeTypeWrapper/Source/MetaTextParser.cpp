#include "MetaTextParser.h"
#include <LLUtils/StringUtility.h>
namespace FreeType
{
    FormattedTextEntry FormattedTextEntry::Parse(const std::wstring& format, const std::wstring& text)
    {
        using namespace std;
        using namespace LLUtils;

        using string_type = std::remove_const_t<std::remove_reference_t<decltype(text)>>;
        FormattedTextEntry result {};

        wstring trimmed = StringUtility::ToLower(format);
        trimmed.erase(trimmed.find_last_not_of(L" >") + 1);
        trimmed.erase(0, trimmed.find_first_not_of(L" <"));

        

        using stringList = ListString<string_type>;

        bool isValid = false;
        stringList properties = StringUtility::split(trimmed, L';');
        stringstream ss;
        for (const string_type& prop : properties)
        {
            stringList trimmedList = StringUtility::split(prop, L'=');

            if (trimmedList.size() == 2)
            {
                const string_type key = StringUtility::ToLower(trimmedList[0]);
                const string_type& value = trimmedList[1];
                if (key == L"textcolor")
                {
                    result.textColor = Color::FromString(StringUtility::ToAString(value));
                }
                isValid = true;
            }
            /*else if (key == u8"backgroundcolor")
            {
                result.backgroundColor = Color::FromString(StringUtility::ToAString(value));
            }*/
            /*else if (key == u8"textsize")
            {
                result.size = std::atoi(StringUtility::ToAString(value).c_str());
            }*/
            /*else if (key == u8"outlineWidth")
            {
                result.outlineWidth = std::atoi(StringUtility::ToAString(value).c_str());
            }
            else if (key == u8"outlineColor")
            {
                result.outlineColor = std::atoi(StringUtility::ToAString(value).c_str());
            }*/
        }

        if (isValid)
            result.text = text;
        else
            result.text = format + text;


        return result;
    }



    VecFormattedTextEntry MetaText::GetFormattedText(std::wstring text)
    {
        using namespace std;
        using string_type = decltype(text);

        ptrdiff_t beginTag = -1;
        ptrdiff_t endTag = -1;
        VecFormattedTextEntry formattedText;

        if (text.empty() == true)
            return formattedText;


        for (size_t i = 0; i < text.length(); i++)
        {
            if (text[i] == '<')
            {
                if (endTag != -1)
                {
                    string_type tagContents = text.substr(static_cast<size_t>(beginTag), static_cast<size_t>(endTag - beginTag + 1));

                    string_type textInsideTag = text.substr(static_cast<size_t>(endTag + 1), static_cast<size_t>(static_cast<ptrdiff_t>(i) - (endTag + 1)));
                    beginTag = static_cast<ptrdiff_t>(i);
                    endTag = -1;

                    FormattedTextEntry entry = FormattedTextEntry::Parse(tagContents, textInsideTag);
                    entry.text = textInsideTag;
                    formattedText.push_back(entry);
                }
                else
                {
                    beginTag = static_cast<ptrdiff_t>(i);
                }


            }
            if (text[i] == '>')
            {
                endTag = static_cast<ptrdiff_t>(i);
            }
        }

        FormattedTextEntry entry;

        if (beginTag == -1)
        {
            //entry.textColor = LLUtils::Color(0xaa, 0xaa, 0xaa, 0xFF);;
            entry.text = text;
        }
        else
        {
            ptrdiff_t i = static_cast<ptrdiff_t>(text.length() - 1);
            string_type tagContents = text.substr(static_cast<size_t>(beginTag), static_cast<size_t>(endTag - beginTag + 1));

            string_type textInsideTag = text.substr(static_cast<size_t>(endTag + 1), static_cast<size_t>(i - endTag));
            beginTag = i;
            endTag = -1;

            entry = FormattedTextEntry::Parse(tagContents, textInsideTag);
            entry.text = textInsideTag;
        }
        formattedText.push_back(entry);

        return formattedText;
    }
}