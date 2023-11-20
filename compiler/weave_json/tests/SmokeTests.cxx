#include "weave/platform/Compiler.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

#include "weave/json/JsonReader.hxx"

TEST_CASE("Basic json")
{
    using namespace weave::json;

    std::string_view source = R"(
{
    "name": "name-value",
    "array": [
        1,
        2,
        3,
        4
    ],
    "value": {
        "key": "value",
        "empty-array": [],
        "empty-object": {},
        "bool": true,
        "nil": null,
        "false": false,
        "float": 21.37
    }
}
)";

    JsonReader reader{source};

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::StartObject);
    REQUIRE(reader.GetValue().empty());

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::Key);
    REQUIRE(reader.GetValue() == "name");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::StringValue);
    REQUIRE(reader.GetValue() == "name-value");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::Key);
    REQUIRE(reader.GetValue() == "array");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::StartArray);
    REQUIRE(reader.GetValue().empty());

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::NumberValue);
    REQUIRE(reader.GetValue() == "1");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::NumberValue);
    REQUIRE(reader.GetValue() == "2");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::NumberValue);
    REQUIRE(reader.GetValue() == "3");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::NumberValue);
    REQUIRE(reader.GetValue() == "4");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::EndArray);
    REQUIRE(reader.GetValue().empty());

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::Key);
    REQUIRE(reader.GetValue() == "value");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::StartObject);
    REQUIRE(reader.GetValue().empty());

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::Key);
    REQUIRE(reader.GetValue() == "key");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::StringValue);
    REQUIRE(reader.GetValue() == "value");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::Key);
    REQUIRE(reader.GetValue() == "empty-array");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::StartArray);
    REQUIRE(reader.GetValue().empty());

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::EndArray);
    REQUIRE(reader.GetValue().empty());

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::Key);
    REQUIRE(reader.GetValue() == "empty-object");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::StartObject);
    REQUIRE(reader.GetValue().empty());

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::EndObject);
    REQUIRE(reader.GetValue().empty());

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::Key);
    REQUIRE(reader.GetValue() == "bool");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::TrueValue);
    REQUIRE(reader.GetValue().empty());

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::Key);
    REQUIRE(reader.GetValue() == "nil");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::NullValue);
    REQUIRE(reader.GetValue().empty());

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::Key);
    REQUIRE(reader.GetValue() == "false");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::FalseValue);
    REQUIRE(reader.GetValue().empty());

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::Key);
    REQUIRE(reader.GetValue() == "float");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::NumberValue);
    REQUIRE(reader.GetValue() == "21.37");

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::EndObject);
    REQUIRE(reader.GetValue().empty());

    REQUIRE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::EndObject);
    REQUIRE(reader.GetValue().empty());

    REQUIRE_FALSE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::EndOfStream);
    REQUIRE(reader.GetValue().empty());

    REQUIRE_FALSE(reader.Next());
    REQUIRE(reader.GetEvent() == JsonEvent::EndOfStream);
    REQUIRE(reader.GetValue().empty());
}

TEST_CASE("Unterminated object")
{
    using namespace weave::json;

    std::string_view source = R"(
{
    "name": "value",
)";

    JsonReader reader{source};

    CHECK(reader.Next());
    CHECK(reader.GetEvent() == JsonEvent::StartObject);
    CHECK(reader.GetValue().empty());

    CHECK(reader.Next());
    CHECK(reader.GetEvent() == JsonEvent::Key);
    CHECK(reader.GetValue() == "name");

    CHECK(reader.Next());
    CHECK(reader.GetEvent() == JsonEvent::StringValue);
    CHECK(reader.GetValue() == "value");

    CHECK_FALSE(reader.Next());
    CHECK(reader.GetEvent() == JsonEvent::EndOfStream);
    CHECK(reader.GetValue().empty());
}

TEST_CASE("Unterminated key")
{
    using namespace weave::json;

    std::string_view source = R"(
{
    "name
)";

    JsonReader reader{source};

    CHECK(reader.Next());
    CHECK(reader.GetEvent() == JsonEvent::StartObject);
    CHECK(reader.GetValue().empty());

    CHECK_FALSE(reader.Next());
    CHECK(reader.GetEvent() == JsonEvent::Error);
    CHECK(reader.GetValue() == "name");

    CHECK_FALSE(reader.Next());
    CHECK(reader.GetEvent() == JsonEvent::Error);
    CHECK(reader.GetValue() == "name");
}


TEST_CASE("Unterminated string value")
{
    using namespace weave::json;

    std::string_view source = R"(
{
    "name": "value
)";

    JsonReader reader{source};

    CHECK(reader.Next());
    CHECK(reader.GetEvent() == JsonEvent::StartObject);
    CHECK(reader.GetValue().empty());

    CHECK(reader.Next());
    CHECK(reader.GetEvent() == JsonEvent::Key);
    CHECK(reader.GetValue() == "name");

    CHECK_FALSE(reader.Next());
    CHECK(reader.GetEvent() == JsonEvent::Error);
    CHECK(reader.GetValue() == "value");

    CHECK_FALSE(reader.Next());
    CHECK(reader.GetEvent() == JsonEvent::Error);
    CHECK(reader.GetValue() == "value");
}
