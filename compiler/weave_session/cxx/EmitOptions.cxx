#include "weave/session/EmitOptions.hxx"

namespace weave::session
{
    bool EmitOptions::SetOption(std::string_view name, std::optional<std::string_view> value)
    {
        if (value.has_value())
        {
            return false;
        }

        if (name == "doc")
        {
            this->Documentation = true;
            return true;
        }

        if (name == "deps")
        {
            this->DependencyGraph = true;
            return true;
        }

        if (name == "meta")
        {
            this->Metadata = true;
            return true;
        }

        if (name == "asm-headers")
        {
            this->AssemblyHeaders = true;
            return true;
        }

        return false;
    }

    void EmitOptions::Dump()
    {
        fmt::println("EmitOptions:");
        fmt::println("  Documentation: {}", this->Documentation);
        fmt::println("  DependencyGraph: {}", this->DependencyGraph);
        fmt::println("  Metadata: {}", this->Metadata);
        fmt::println("  AssemblyHeaders: {}", this->AssemblyHeaders);
    }
}
