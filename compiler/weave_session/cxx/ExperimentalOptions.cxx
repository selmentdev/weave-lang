#include "weave/session/ExperimentalOptions.hxx"

namespace weave::session::impl
{
    bool ParsePrintFormat(PrintFormat& result, std::optional<std::string_view> value)
    {
        if (value)
        {
        if (*value == "tokens")
        {
            result = PrintFormat::Tokens;
            return true;
        }

        if (*value == "ast")
        {
            result = PrintFormat::AST;
            return true;
        }
        }

        return false;
    }
}
    
namespace weave::session
{
    bool ExperimentalOptions::SetOption(std::string_view name, std::optional<std::string_view> value)
    {
        if (name == "print")
        {
            return impl::ParsePrintFormat(this->Format, value);
        }

        return false;
    }
}
