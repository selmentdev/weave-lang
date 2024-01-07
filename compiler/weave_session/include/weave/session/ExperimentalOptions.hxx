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
        void Dump();

    public:
        PrintFormat Format = PrintFormat::None;
    };
}

template <>
struct fmt::formatter<weave::session::PrintFormat> : fmt::formatter<std::string_view>
{
    template <typename FormatContext>
    auto format(weave::session::PrintFormat const& value, FormatContext& context)
    {
        std::string_view result = "unknown";

        switch (value)
        {
        case weave::session::PrintFormat::None:
            result = "none";
            break;
        case weave::session::PrintFormat::Tokens:
            result = "tokens";
            break;
        case weave::session::PrintFormat::AST:
            result = "ast";
            break;
        }

        return fmt::formatter<std::string_view>::format("none", context);
    }
};
