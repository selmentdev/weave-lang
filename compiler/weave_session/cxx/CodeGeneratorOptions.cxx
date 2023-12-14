#include "weave/session/CodeGeneratorOptions.hxx"

namespace weave::session
{
    bool CodeGeneratorOptions::SetOption(std::string_view name, std::optional<std::string_view> value)
    {
        if (name == "debug")
        {
            return impl::ParseBoolean(this->Debug, value);
        }

        if (name == "checked")
        {
            return impl::ParseBoolean(this->Checked, value);
        }

        if (name == "opt")
        {
            return impl::ParseInteger(this->OptimizationLevel, value);
        }

        if (name == "sanitizers")
        {
            return SanitizersSpec::Parse(this->Sanitizers, value);
        }

        return false;
    }
}
