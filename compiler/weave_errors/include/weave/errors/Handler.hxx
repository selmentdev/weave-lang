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
        constexpr void AddWarning(std::string message)
        {
            this->_messages.emplace_back(std::move(message));
            ++this->_warnings;
        }

        constexpr void AddError(std::string message)
        {
            this->_messages.emplace_back(std::move(message));
            ++this->_errors;
        }

        [[nodiscard]] constexpr std::span<DiagnosticMessage const> GetMessages() const
        {
            return this->_messages;
        }

        [[nodiscard]] bool HasErrors() const
        {
            return this->_errors > 0;
        }

        [[nodiscard]] bool HasWarnings() const
        {
            return this->_warnings > 0;
        }
    };
}
