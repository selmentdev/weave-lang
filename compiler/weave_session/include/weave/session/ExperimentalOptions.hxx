#pragma once
#include "weave/session/Options.hxx"

namespace weave::session
{
    enum class PrintFormat
    {
        /// @brief Print nothing.
        None,

        /// @brief Print the token stream of given source file.
        Tokens,

        /// @brief Print the AST of given source file.
        AST,
    };

    class ExperimentalOptions : public Options
    {
    public:
        bool SetOption(std::string_view name, std::optional<std::string_view> value) override;

    public:
        PrintFormat Format = PrintFormat::None;
    };
}
