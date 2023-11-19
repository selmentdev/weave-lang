#pragma once
#include <string>
#include <vector>
#include <span>

namespace weave::errors
{
    struct DiagnosticMessage
    {
        std::string Value{};
    };

    class Handler final
    {
    private:
        std::vector<DiagnosticMessage> _messages{};
        size_t _warnings{};
        size_t _errors{};

    public:
        constexpr void warning(std::string message)
        {
            this->_messages.emplace_back(std::move(message));
            ++this->_warnings;
        }

        constexpr void error(std::string message)
        {
            this->_messages.emplace_back(std::move(message));
            ++this->_errors;
        }

        [[nodiscard]] constexpr std::span<DiagnosticMessage const> messages() const
        {
            return this->_messages;
        }
    };
}
