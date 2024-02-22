#pragma once
#include <span>
#include <string_view>
#include <optional>

namespace weave::commandline
{
    class CommandLineParser
    {
    protected:
        static bool TryParseOption(
            std::string_view argument,
            std::string_view& name,
            std::optional<std::string_view>& value);

        static std::optional<uint32_t> TryParseUInt32(
            std::optional<std::string_view> const& value);

        static std::optional<int32_t> TryParseInt32(
            std::optional<std::string_view> const& value);

        static std::optional<uint64_t> TryParseUInt64(
            std::optional<std::string_view> const& value);

        static std::optional<int64_t> TryParseInt64(
            std::optional<std::string_view> const& value);

        static std::optional<bool> TryParseBool(
            std::optional<std::string_view> const& value);

        static std::optional<std::string_view> TryParseString(
            std::optional<std::string_view> const& value);

        static std::optional<std::string_view> TryParseFilePath(
            std::optional<std::string_view> const& value);

    protected:
        virtual void OnPrintHelp() const = 0;

        virtual void OnPositionalArgument(std::string_view value) = 0;

        virtual bool OnOption(std::string_view name, std::optional<std::string_view> value) = 0;

    public:
        CommandLineParser() = default;
        CommandLineParser(const CommandLineParser&) = default;
        CommandLineParser(CommandLineParser&&) = default;
        CommandLineParser& operator=(const CommandLineParser&) = default;
        CommandLineParser& operator=(CommandLineParser&&) = default;
        virtual ~CommandLineParser() = default;

    public:
        bool Parse(
            int argc,
            char** argv);

        void PrintHelp() const
        {
            return this->OnPrintHelp();
        }
    };
}
