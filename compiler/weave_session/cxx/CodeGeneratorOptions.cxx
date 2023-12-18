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

    void CodeGeneratorOptions::Dump()
    {
        fmt::println("CodeGeneratorOptions:");
        fmt::println("  Debug: {}", this->Debug);
        fmt::println("  Checked: {}", this->Checked);
        fmt::println("  OptimizationLevel: {}", this->OptimizationLevel);
        fmt::println("  Sanitizers.AddressSanitizer: {}", this->Sanitizers.AddressSanitizer);
        fmt::println("  Sanitizers.ThreadSanitizer: {}", this->Sanitizers.ThreadSanitizer);
        fmt::println("  Sanitizers.MemorySanitizer: {}", this->Sanitizers.MemorySanitizer);
        fmt::println("  Sanitizers.LeakSanitizer: {}", this->Sanitizers.LeakSanitizer);
        fmt::println("  Sanitizers.UndefinedBehaviorSanitizer: {}", this->Sanitizers.UndefinedBehaviorSanitizer);
    }
}
