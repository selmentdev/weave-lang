#include "Weave.Core/Assert.hxx"
#include "Weave.Core/Process.hxx"
#include <span>
#include <string_view>
#include <vector>
#include <functional>
#include <variant>
#include <map>
#include <filesystem>
#include <expected>

namespace Weave::Cli
{
    enum class OptionArity
    {
        None,
        Single,
        Multiple,
    };

    enum class OptionUsage
    {
        Required,
        Optional,
    };

    struct Option
    {
        std::string_view Name{};
        std::string_view ShortName{};
        std::string_view Description{};
        std::string_view Hint{};
        OptionArity Arity{};
        OptionUsage Usage{};
    };

    class Parser;

    class Matched
    {
        friend class Parser;

    private:
        std::vector<std::string_view> m_Names{};
        std::vector<std::pair<size_t, std::string_view>> m_Values{};
        std::vector<std::string_view> m_Positional{};

    private:
        Matched(
            std::vector<std::string_view>&& names,
            std::vector<std::pair<size_t, std::string_view>>&& values,
            std::vector<std::string_view>&& positional)
            : m_Names{names}
            , m_Values{values}
            , m_Positional{positional}
        {
        }

    public:
        std::vector<std::string_view> GetValues(std::string_view name) const
        {
            std::vector<std::string_view> result{};

            for (auto const& [index, value] : this->m_Values)
            {
                if (this->m_Names[index] == name)
                {
                    result.emplace_back(value);
                }
            }

            return result;
        }

        std::optional<std::string_view> GetValue(std::string_view name) const
        {
            for (auto const& [index, value] : this->m_Values)
            {
                if (this->m_Names[index] == name)
                {
                    return value;
                }
            }

            return std::nullopt;
        }

        std::span<std::string_view const> GetPositional() const
        {
            return this->m_Positional;
        }

        bool HasFlag(std::string_view name) const
        {
            return std::find(this->m_Names.begin(), this->m_Names.end(), name) != this->m_Names.end();
        }
    };

    struct ParseError final
    {
        std::string_view Message{};
        std::string_view Option{};
    };

    class Parser final
    {
    private:
        std::vector<Option> m_Options{};

        Option const* FindOption(std::string_view name) const
        {
            for (auto const& option : this->m_Options)
            {
                if (option.Name == name || option.ShortName == name)
                {
                    return &option;
                }
            }

            return nullptr;
        }

    public:
        std::span<Option const> GetOptions() const
        {
            return this->m_Options;
        }

        void Add(std::string_view name, std::string_view shortName, std::string_view description, std::string_view hint, OptionArity arity, OptionUsage usage)
        {
            WEAVE_ASSERT(not name.empty());

            this->m_Options.push_back(Option{
                .Name = name,
                .ShortName = shortName,
                .Description = description,
                .Hint = hint,
                .Arity = arity,
                .Usage = usage,
            });
        }

        std::expected<Matched, ParseError> Parse(std::span<const char*> args) const&
        {
            std::vector<std::string_view> resultNames{};
            std::vector<std::pair<size_t, std::string_view>> resultValues{};
            std::vector<std::string_view> resultPositional{};

            bool parsePositional = false;

            for (auto it = args.begin(); it != args.end(); ++it)
            {
                std::string_view item{*it};

                if (item == "--")
                {
                    parsePositional = true;
                    continue;
                }

                if (!item.starts_with('-'))
                {
                    parsePositional = true;
                }

                if (parsePositional)
                {
                    resultPositional.emplace_back(item);
                }
                else
                {
                    if (Option const* option = this->FindOption(item); option != nullptr)
                    {
                        // Found option. Get or add it to names by index.
                        size_t optionIndex;
                        bool added = false;

                        if (auto optit = std::find(resultNames.begin(), resultNames.end(), option->Name); optit != resultNames.end())
                        {
                            optionIndex = std::distance(resultNames.begin(), optit);
                        }
                        else
                        {
                            optionIndex = resultNames.size();
                            resultNames.emplace_back(option->Name);
                            added = true;
                        }

                        // Verify if option can be set multiple times.
                        if (option->Arity != OptionArity::Multiple)
                        {
                            if (not added)
                            {
                                return std::unexpected(ParseError{.Message = "Option already parsed", .Option = item});
                            }
                        }

                        if (option->Arity != OptionArity::None)
                        {
                            ++it;

                            if (it == args.end())
                            {
                                return std::unexpected(ParseError{.Message = "Missing value", .Option = item});
                            }

                            std::string_view value{*it};

                            resultValues.emplace_back(optionIndex, value);
                        }
                    }
                    else
                    {
                        return std::unexpected(ParseError{.Message = "Unknown option", .Option = item});
                    }
                }
            }

            // Verify if all required options are present.
            for (auto const& option : this->m_Options)
            {
                if (option.Usage == OptionUsage::Required)
                {
                    if (std::find(resultNames.begin(), resultNames.end(), option.Name) == resultNames.end())
                    {
                        return std::unexpected(ParseError{.Message = "Missing required option", .Option = option.Name});
                    }
                }
            }

            return Matched{
                std::move(resultNames),
                std::move(resultValues),
                std::move(resultPositional),
            };
        }
    };

}

int main(int argc, char** argv)
{
    fmt::println("--- args-begin ---");
    for (int i = 0; i < argc; ++i)
    {
        fmt::println("{}: '{}'", i, argv[i]);
    }
    fmt::println("--- args-end ---");

    using namespace Weave::Cli;
    Parser parser{};
    parser.Add("--exe", "", "Path to executable", "FILE", OptionArity::Single, OptionUsage::Required);
    parser.Add("--working-directory", "-W", "Working directory", "PATH", OptionArity::Single, OptionUsage::Required);
    parser.Add("--codegen", "-C", "Code generator options", "OPT[=VALUE]", OptionArity::Multiple, OptionUsage::Optional);
    parser.Add("--config", "", "Configuration", "[release|debug|checked]", OptionArity::Single, OptionUsage::Optional);
    parser.Add("--verbose", "-v", "Use verbose output", "", OptionArity::None, OptionUsage::Optional);
    parser.Add("--version", "-V", "Prints version information", "", OptionArity::None, OptionUsage::Optional);
    parser.Add("--help", "-h", "Prints help", "", OptionArity::None, OptionUsage::Optional);

    if (auto r = parser.Parse(std::span{const_cast<const char**>(argv + 1), static_cast<size_t>(argc - 1)}); !r.has_value())
    {
        ParseError const& e = r.error();
        fmt::println(stderr, "{}: {}", e.Message, e.Option);
        return EXIT_FAILURE;
    }
    else
    {
        Matched const& m = *r;

        if (auto v = m.GetValue("--exe"))
        {
            fmt::println("exe: {}", *v);
        }

        for (auto v : m.GetValues("--codegen"))
        {
            fmt::println("codegen: {}", v);
        }

        for (auto v : m.GetPositional())
        {
            fmt::println("positional: {}", v);
        }

        if (m.HasFlag("--help"))
        {
            for (auto const& option : parser.GetOptions())
            {
                fmt::println("    {:>2} {} {}", option.ShortName, option.Name, option.Hint);

                if (not option.Description.empty())
                {
                    fmt::println("                {}", option.Description);
                }
            }

            return EXIT_SUCCESS;
        }

        auto exeName = m.GetValue("--exe");
        auto workingDirectory = m.GetValue("--working-directory");
        auto compilerArgs = m.GetValues("--codegen");

        // exeName = R"(D:\repos\weave-lang\out\build\x64-Debug\src\Compiler\Frontend\weave-frontend.exe)";
        // workingDirectory = R"(D:\repos\weave-lang\src\Compiler\Syntax\tests)";
        std::filesystem::path const wd{workingDirectory.value()};

        std::filesystem::recursive_directory_iterator it{wd};

        for (auto const& entry : it)
        {
            fmt::println("{}", entry.path().string());

            if (entry.path().extension() != ".source")
            {
                continue;
            }

            std::string output{};
            std::string error{};

            if (auto ret = Weave::Execute(std::string{exeName.value()}.c_str(), entry.path().string().c_str(), nullptr, output, error))
            {
                fmt::println("ret: {}", *ret);

                // fmt::println("output:");
                // puts(output.c_str());

                // fmt::println("error:");
                // puts(error.c_str());
            }
            else
            {
                fmt::println("Failed to spawn child process");
            }
        }
    }

    return EXIT_SUCCESS;
}
