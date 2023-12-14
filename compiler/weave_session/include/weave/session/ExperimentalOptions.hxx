#pragma once
#include "weave/session/Options.hxx"
#include "weave/session/SanitizersSpec.hxx"

namespace weave::session
{
    class ExperimentalOptions : public Options
    {
    public:
        bool SetOption(std::string_view name, std::optional<std::string_view> value) override;

    public:
    };
}
