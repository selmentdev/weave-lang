#include "Weave.Core/Uri.hxx"

namespace Weave
{
    std::string EscapeUri(std::string_view value)
    {
        std::string result{};


        for (char const ch :  value)
        {
            if ((('a' <= ch) and (ch <= 'z')) or (('A' <= ch) and (ch <= 'Z')) or (('-' <= ch) and (ch <= ':')) or (('&' <= ch) and (ch <='*')))
            {
                result.push_back(ch);
            }
            else if ((ch == '!') or (ch == '=') or (ch == '?') or (ch == '_') or (ch == '~'))
            {
                result.push_back(ch);
            }
            else
            {
                result.push_back('%');
                result.push_back("0123456789ABCDEF"[(ch >> 4) & 0xF]);
                result.push_back("0123456789ABCDEF"[ch & 0xF]);
            }
        }

        return result;
    }
}
