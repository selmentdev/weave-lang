#include "weave/platform/compiler.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

#include "weave/source/SourceText.hxx"

TEST_CASE("Source Text")
{
    using namespace weave::source;

    SECTION(R"(Empty text)")
    {
        SourceText const text{""};
        REQUIRE(text.get_lines().size() == 1);
        REQUIRE(text.get_lines()[0] == 0);

        {
            constexpr size_t index = 0;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 0);
            CHECK(optLine->end.offset == 0);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 0);
            CHECK(optLineContent->end.offset == 0);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.get_line(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.get_line_content(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(Single \n)")
    {
        SourceText const text{"\n"};
        REQUIRE(text.get_lines().size() == 2);
        REQUIRE(text.get_lines()[0] == 0);
        REQUIRE(text.get_lines()[1] == 1);

        {
            constexpr size_t index = 0;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 0);
            CHECK(optLine->end.offset == 1);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 0);
            CHECK(optLineContent->end.offset == 0);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 1);
            CHECK(optLine->end.offset == 1);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 1);
            CHECK(optLineContent->end.offset == 1);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 2;

            auto const optLine = text.get_line(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.get_line_content(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(Single \r)")
    {
        SourceText const text{"\r"};
        REQUIRE(text.get_lines().size() == 1);
        REQUIRE(text.get_lines()[0] == 0);

        {
            constexpr size_t index = 0;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 0);
            CHECK(optLine->end.offset == 1);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 0);
            CHECK(optLineContent->end.offset == 1);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "\r");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText == "\r");
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.get_line(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.get_line_content(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(Single \r\n)")
    {
        SourceText const text{"\r\n"};
        REQUIRE(text.get_lines().size() == 2);
        REQUIRE(text.get_lines()[0] == 0);
        REQUIRE(text.get_lines()[1] == 2);

        {
            constexpr size_t index = 0;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 0);
            CHECK(optLine->end.offset == 2);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 0);
            CHECK(optLineContent->end.offset == 0);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "\r\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 2);
            CHECK(optLine->end.offset == 2);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 2);
            CHECK(optLineContent->end.offset == 2);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 2;

            auto const optLine = text.get_line(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.get_line_content(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(Multiple \n)")
    {
        SourceText const text{"\n\n\n"};
        REQUIRE(text.get_lines().size() == 4);
        REQUIRE(text.get_lines()[0] == 0);
        REQUIRE(text.get_lines()[1] == 1);
        REQUIRE(text.get_lines()[2] == 2);
        REQUIRE(text.get_lines()[3] == 3);

        {
            constexpr size_t index = 0;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 0);
            CHECK(optLine->end.offset == 1);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 0);
            CHECK(optLineContent->end.offset == 0);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 1);
            CHECK(optLine->end.offset == 2);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 1);
            CHECK(optLineContent->end.offset == 1);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 2;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 2);
            CHECK(optLine->end.offset == 3);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 2);
            CHECK(optLineContent->end.offset == 2);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 3;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 3);
            CHECK(optLine->end.offset == 3);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 3);
            CHECK(optLineContent->end.offset == 3);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 4;

            auto const optLine = text.get_line(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.get_line_content(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(Multiple \r)")
    {
        SourceText const text{"\r\r\r"};
        REQUIRE(text.get_lines().size() == 1);
        REQUIRE(text.get_lines()[0] == 0);

        {
            constexpr size_t index = 0;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 0);
            CHECK(optLine->end.offset == 3);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 0);
            CHECK(optLineContent->end.offset == 3);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "\r\r\r");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText == "\r\r\r");
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.get_line(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.get_line_content(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(Multiple \r\n)")
    {
        SourceText const text{"\r\n\r\n\r\n"};
        REQUIRE(text.get_lines().size() == 4);
        REQUIRE(text.get_lines()[0] == 0);
        REQUIRE(text.get_lines()[1] == 2);
        REQUIRE(text.get_lines()[2] == 4);
        REQUIRE(text.get_lines()[3] == 6);

        {
            constexpr size_t index = 0;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 0);
            CHECK(optLine->end.offset == 2);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 0);
            CHECK(optLineContent->end.offset == 0);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "\r\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 2);
            CHECK(optLine->end.offset == 4);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 2);
            CHECK(optLineContent->end.offset == 2);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "\r\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 2;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 4);
            CHECK(optLine->end.offset == 6);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 4);
            CHECK(optLineContent->end.offset == 4);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "\r\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 3;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 6);
            CHECK(optLine->end.offset == 6);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 6);
            CHECK(optLineContent->end.offset == 6);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 4;

            auto const optLine = text.get_line(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.get_line_content(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(Mixed new line characters)")
    {
        SourceText const text{"This\nis\r\nsome\ntext\n"};
        REQUIRE(text.get_lines().size() == 5);
        REQUIRE(text.get_lines()[0] == 0);
        REQUIRE(text.get_lines()[1] == 5);
        REQUIRE(text.get_lines()[2] == 9);
        REQUIRE(text.get_lines()[3] == 14);
        REQUIRE(text.get_lines()[4] == 19);

        {
            constexpr size_t index = 0;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 0);
            CHECK(optLine->end.offset == 5);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 0);
            CHECK(optLineContent->end.offset == 4);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "This\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText == "This");
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 5);
            CHECK(optLine->end.offset == 9);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 5);
            CHECK(optLineContent->end.offset == 7);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "is\r\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText == "is");
        }

        {
            constexpr size_t index = 2;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 9);
            CHECK(optLine->end.offset == 14);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 9);
            CHECK(optLineContent->end.offset == 13);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "some\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText == "some");
        }

        {
            constexpr size_t index = 3;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 14);
            CHECK(optLine->end.offset == 19);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 14);
            CHECK(optLineContent->end.offset == 18);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "text\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText == "text");
        }

        {
            constexpr size_t index = 4;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 19);
            CHECK(optLine->end.offset == 19);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 19);
            CHECK(optLineContent->end.offset == 19);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }

        {
            constexpr size_t index = 5;

            auto const optLine = text.get_line(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.get_line_content(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText.empty());
        }
    }

    SECTION(R"(No new line at end of text)")
    {
        SourceText const text{"This\nis\r\nsome\ntext"};
        REQUIRE(text.get_lines().size() == 4);
        REQUIRE(text.get_lines()[0] == 0);
        REQUIRE(text.get_lines()[1] == 5);
        REQUIRE(text.get_lines()[2] == 9);
        REQUIRE(text.get_lines()[3] == 14);

        {
            constexpr size_t index = 0;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 0);
            CHECK(optLine->end.offset == 5);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 0);
            CHECK(optLineContent->end.offset == 4);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "This\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText == "This");
        }

        {
            constexpr size_t index = 1;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 5);
            CHECK(optLine->end.offset == 9);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 5);
            CHECK(optLineContent->end.offset == 7);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "is\r\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText == "is");
        }

        {
            constexpr size_t index = 2;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 9);
            CHECK(optLine->end.offset == 14);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 9);
            CHECK(optLineContent->end.offset == 13);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "some\n");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText == "some");
        }

        {
            constexpr size_t index = 3;

            auto const optLine = text.get_line(index);
            REQUIRE(optLine.has_value());
            CHECK(optLine->start.offset == 14);
            CHECK(optLine->end.offset == 18);

            auto const optLineContent = text.get_line_content(index);
            REQUIRE(optLineContent.has_value());
            CHECK(optLineContent->start.offset == 14);
            CHECK(optLineContent->end.offset == 18);

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText == "text");

            auto const lineContentText = text.get_line_content_text(index);
            REQUIRE(lineContentText == "text");
        }

        {
            constexpr size_t index = 4;

            auto const optLine = text.get_line(index);
            REQUIRE_FALSE(optLine.has_value());

            auto const optLineContent = text.get_line_content(index);
            REQUIRE_FALSE(optLineContent.has_value());

            auto const lineText = text.get_line_text(index);
            REQUIRE(lineText.empty());

            auto const lineContentText = text.get_line_content_text(index);
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

            CHECK(text.get_line_index(i) == ie.line);

            LinePosition const& p = text.get_line_position(SourcePosition{i});

            CHECK(ie.line == p.line);
            CHECK(ie.column == p.column);

            for (uint32_t j = i; j < std::size(Mapping); ++j)
            {
                LinePosition const& je = Mapping[j];

                SourceSpan const span{{i}, {j}};
                LineSpan const line = text.get_line_span(span);
                CHECK(line.start.line == ie.line);
                CHECK(line.start.column == ie.column);
                CHECK(line.end.line == je.line);
                CHECK(line.end.column == je.column);
            }
        }

        for (uint32_t i = 0; i < text.get_content_view().size(); ++i)
        {
            for (uint32_t j = i; j < text.get_content_view().size(); ++j)
            {
                SourceSpan const span{{i}, {j}};

                std::string_view const lineText = text.get_text(span);
                CHECK(lineText == text.get_content_view().substr(i, j - i));
            }
        }
    }
}
