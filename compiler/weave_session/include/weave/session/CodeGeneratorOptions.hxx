#pragma once
#include "weave/session/Options.hxx"
#include "weave/session/SanitizersSpec.hxx"

namespace weave::session
{
    class CodeGeneratorOptions : public Options
    {
    public:
        bool SetOption(std::string_view name, std::optional<std::string_view> value) override;

    public:
        bool Checked = false;
        bool Debug = false;
        uint32_t OptimizationLevel = 0;
        SanitizersSpec Sanitizers{};
    };
}
