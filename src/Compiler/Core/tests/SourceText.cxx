#include "Weave.Core/Platform/Compiler.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END


#include "Weave.Core/SourceText.hxx"

TEST_CASE("Source Text")
{
    using namespace Weave;

    SECTION(R"(Empty text)")
    {
        SourceText const text{""};
        REQUIRE(text.GetLines().size() == 1);
        REQUIRE(text.GetLines()[0] == 0);

        {
            constexpr size_t index = 0;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 0);
            CHECK(optLine->End.Offset == 0);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 0);
            CHECK(optLineContent->End.Offset == 0);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.GetLine(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(Single \n)")
    {
        SourceText const text{"\n"};
        REQUIRE(text.GetLines().size() == 2);
        REQUIRE(text.GetLines()[0] == 0);
        REQUIRE(text.GetLines()[1] == 1);

        {
            constexpr size_t index = 0;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 0);
            CHECK(optLine->End.Offset == 1);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 0);
            CHECK(optLineContent->End.Offset == 0);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 1);
            CHECK(optLine->End.Offset == 1);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 1);
            CHECK(optLineContent->End.Offset == 1);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 2;

            auto const optLine = text.GetLine(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(Single \r)")
    {
        SourceText const text{"\r"};
        REQUIRE(text.GetLines().size() == 1);
        REQUIRE(text.GetLines()[0] == 0);

        {
            constexpr size_t index = 0;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 0);
            CHECK(optLine->End.Offset == 1);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 0);
            CHECK(optLineContent->End.Offset == 1);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "\r");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText == "\r");
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.GetLine(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(Single \r\n)")
    {
        SourceText const text{"\r\n"};
        REQUIRE(text.GetLines().size() == 2);
        REQUIRE(text.GetLines()[0] == 0);
        REQUIRE(text.GetLines()[1] == 2);

        {
            constexpr size_t index = 0;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 0);
            CHECK(optLine->End.Offset == 2);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 0);
            CHECK(optLineContent->End.Offset == 0);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "\r\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 2);
            CHECK(optLine->End.Offset == 2);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 2);
            CHECK(optLineContent->End.Offset == 2);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 2;

            auto const optLine = text.GetLine(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(Multiple \n)")
    {
        SourceText const text{"\n\n\n"};
        REQUIRE(text.GetLines().size() == 4);
        REQUIRE(text.GetLines()[0] == 0);
        REQUIRE(text.GetLines()[1] == 1);
        REQUIRE(text.GetLines()[2] == 2);
        REQUIRE(text.GetLines()[3] == 3);

        {
            constexpr size_t index = 0;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 0);
            CHECK(optLine->End.Offset == 1);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 0);
            CHECK(optLineContent->End.Offset == 0);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 1);
            CHECK(optLine->End.Offset == 2);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 1);
            CHECK(optLineContent->End.Offset == 1);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 2;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 2);
            CHECK(optLine->End.Offset == 3);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 2);
            CHECK(optLineContent->End.Offset == 2);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 3;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 3);
            CHECK(optLine->End.Offset == 3);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 3);
            CHECK(optLineContent->End.Offset == 3);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 4;

            auto const optLine = text.GetLine(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(Multiple \r)")
    {
        SourceText const text{"\r\r\r"};
        REQUIRE(text.GetLines().size() == 1);
        REQUIRE(text.GetLines()[0] == 0);

        {
            constexpr size_t index = 0;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 0);
            CHECK(optLine->End.Offset == 3);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 0);
            CHECK(optLineContent->End.Offset == 3);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "\r\r\r");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText == "\r\r\r");
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.GetLine(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(Multiple \r\n)")
    {
        SourceText const text{"\r\n\r\n\r\n"};
        REQUIRE(text.GetLines().size() == 4);
        REQUIRE(text.GetLines()[0] == 0);
        REQUIRE(text.GetLines()[1] == 2);
        REQUIRE(text.GetLines()[2] == 4);
        REQUIRE(text.GetLines()[3] == 6);

        {
            constexpr size_t index = 0;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 0);
            CHECK(optLine->End.Offset == 2);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 0);
            CHECK(optLineContent->End.Offset == 0);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "\r\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 2);
            CHECK(optLine->End.Offset == 4);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 2);
            CHECK(optLineContent->End.Offset == 2);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "\r\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 2;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 4);
            CHECK(optLine->End.Offset == 6);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 4);
            CHECK(optLineContent->End.Offset == 4);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "\r\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 3;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 6);
            CHECK(optLine->End.Offset == 6);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 6);
            CHECK(optLineContent->End.Offset == 6);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 4;

            auto const optLine = text.GetLine(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(Mixed new line characters)")
    {
        SourceText const text{"This\nis\r\nsome\ntext\n"};
        REQUIRE(text.GetLines().size() == 5);
        REQUIRE(text.GetLines()[0] == 0);
        REQUIRE(text.GetLines()[1] == 5);
        REQUIRE(text.GetLines()[2] == 9);
        REQUIRE(text.GetLines()[3] == 14);
        REQUIRE(text.GetLines()[4] == 19);

        {
            constexpr size_t index = 0;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 0);
            CHECK(optLine->End.Offset == 5);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 0);
            CHECK(optLineContent->End.Offset == 4);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "This\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText == "This");
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 5);
            CHECK(optLine->End.Offset == 9);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 5);
            CHECK(optLineContent->End.Offset == 7);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "is\r\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText == "is");
        }

        {
            constexpr size_t index = 2;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 9);
            CHECK(optLine->End.Offset == 14);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 9);
            CHECK(optLineContent->End.Offset == 13);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "some\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText == "some");
        }

        {
            constexpr size_t index = 3;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 14);
            CHECK(optLine->End.Offset == 19);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 14);
            CHECK(optLineContent->End.Offset == 18);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "text\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText == "text");
        }

        {
            constexpr size_t index = 4;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 19);
            CHECK(optLine->End.Offset == 19);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 19);
            CHECK(optLineContent->End.Offset == 19);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 5;

            auto const optLine = text.GetLine(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(No new line at end of text)")
    {
        SourceText const text{"This\nis\r\nsome\ntext"};
        REQUIRE(text.GetLines().size() == 4);
        REQUIRE(text.GetLines()[0] == 0);
        REQUIRE(text.GetLines()[1] == 5);
        REQUIRE(text.GetLines()[2] == 9);
        REQUIRE(text.GetLines()[3] == 14);

        {
            constexpr size_t index = 0;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 0);
            CHECK(optLine->End.Offset == 5);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 0);
            CHECK(optLineContent->End.Offset == 4);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "This\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText == "This");
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 5);
            CHECK(optLine->End.Offset == 9);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 5);
            CHECK(optLineContent->End.Offset == 7);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "is\r\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText == "is");
        }

        {
            constexpr size_t index = 2;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 9);
            CHECK(optLine->End.Offset == 14);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 9);
            CHECK(optLineContent->End.Offset == 13);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "some\n");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText == "some");
        }

        {
            constexpr size_t index = 3;

            auto const optLine = text.GetLine(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->Start.Offset == 14);
            CHECK(optLine->End.Offset == 18);

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->Start.Offset == 14);
            CHECK(optLineContent->End.Offset == 18);

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText == "text");

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText == "text");
        }

        {
            constexpr size_t index = 4;

            auto const optLine = text.GetLine(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.GetLineContent(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.GetLineText(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.GetLineContentText(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION("Range mapping")
    {
        static constexpr LinePosition Mapping[] =
            {
                LinePosition{0, 0},
                LinePosition{0, 1},
                LinePosition{1, 0},
                LinePosition{1, 1},
                LinePosition{1, 2},
                LinePosition{1, 3},
                LinePosition{2, 0},
                LinePosition{2, 1},
                LinePosition{2, 2},
                LinePosition{2, 3},
                LinePosition{2, 4},
                LinePosition{3, 0},
                LinePosition{3, 1},
                LinePosition{3, 2},
                LinePosition{3, 3},
                LinePosition{3, 4},
                LinePosition{3, 5},
                LinePosition{3, 6},
            };

        SourceText const text{"S\n\tt\r\nr\tu\r\ng"};

        for (uint32_t i = 0; i < std::size(Mapping); ++i)
        {
            LinePosition const& ie = Mapping[i];

            CHECK(text.GetLineIndex(i) == ie.Line);

            LinePosition const& p = text.GetLinePosition(SourcePosition{i});

            CHECK(ie.Line == p.Line);
            CHECK(ie.Column == p.Column);

            for (uint32_t j = i; j < std::size(Mapping); ++j)
            {
                LinePosition const& je = Mapping[j];

                SourceSpan const span{{i}, {j}};
                LineSpan const line = text.GetLineSpan(span);
                CHECK(line.Start.Line == ie.Line);
                CHECK(line.Start.Column == ie.Column);
                CHECK(line.End.Line == je.Line);
                CHECK(line.End.Column == je.Column);
            }
        }

        for (uint32_t i = 0; i < text.GetContent().size(); ++i)
        {
            for (uint32_t j = i; j < text.GetContent().size(); ++j)
            {
                SourceSpan const span{{i}, {j}};

                std::string_view const lineText = text.GetText(span);
                CHECK(lineText == text.GetContent().substr(i, j - i));
            }
        }
    }
}
