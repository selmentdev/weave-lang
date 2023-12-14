#pragma once
#include "weave/session/Options.hxx"

namespace weave::session
{
    class EmitOptions : public Options
    {
    public:
        bool SetOption(std::string_view name, std::optional<std::string_view> value) override;

    public:
        bool Documentation = false;
        bool DependencyGraph = false;
        bool Metadata = false;
        bool AssemblyHeaders = false;
    };
}
