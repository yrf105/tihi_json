#include <stdint.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include "../src/tihijson.h"

static int main_ret = 0;
static uint32_t test_count = 0;
static uint32_t test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual)                         \
    do {                                                                 \
        ++test_count;                                                    \
        if (equality) {                                                  \
            ++test_pass;                                                 \
        } else {                                                         \
            main_ret = 1;                                                \
            std::cerr << "[error] " << __FILE__ << ':' << __LINE__       \
                      << ": expect: " << expect << " actual: " << actual \
                      << std::endl;                                      \
        }                                                                \
    } while (0)

#define EXPECT_EQ_INT(expect, actual) \
    EXPECT_EQ_BASE((expect) == (actual), expect, actual)

#define EXPECT_EQ_STR(expect, actual, alength)                     \
    EXPECT_EQ_BASE(sizeof(expect) - 1 == alength &&                \
                       std::string(expect) == std::string(actual), \
                   expect, actual)

#define TEST_PARSE_VALUE(status, type, str)                  \
    do {                                                     \
        EXPECT_EQ_INT(status, json->parse(str, json_value)); \
        EXPECT_EQ_INT(type, json_value->get_type());         \
    } while (0)

#define TEST_ERROR(error, str)                                    \
    do {                                                          \
        TEST_PARSE_VALUE(error, tihi::JsonValue::JSON_NULL, str); \
    } while (0)

static void test_parse_value() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json());


    TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_NULL, "null");
    TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_NULL,
                     "null  ");

    TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_FALSE,
                     "false");
    TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_FALSE,
                     "false  ");

    TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_TRUE, "true");
    TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_TRUE,
                     "true  ");

    TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_NULL,
                     "null  \r\n");
    TEST_ERROR(tihi::Json::PARSE_ROOT_NOT_SINGULAR, "null  l");

    TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_FALSE,
                     "false  \r\n");
    TEST_ERROR(tihi::Json::PARSE_ROOT_NOT_SINGULAR, "false  l");

    TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_TRUE,
                     "true  \r\n");
    TEST_ERROR(tihi::Json::PARSE_ROOT_NOT_SINGULAR, "true  l");

    TEST_ERROR(tihi::Json::PARSE_INVALID_VALUE, "?");
    TEST_ERROR(tihi::Json::PARSE_INVALID_VALUE, "nul");
}


#define EXPECT_EQ_DOUBLE(expect, actual) \
    do {                                 \
        EXPECT_EQ_INT(expect, actual);   \
    } while (0)

#define TEST_PARSE_NUMBER(status, expect, actual)                       \
    do {                                                                \
        TEST_PARSE_VALUE(status, tihi::JsonValue::JSON_NUMBER, actual); \
        if (status == tihi::Json::PARSE_OK) {                           \
            EXPECT_EQ_DOUBLE(expect, json_value->get_number());         \
        }                                                               \
    } while (0)

#define TEST_PARSE_NUMBER_OK(expect, actual)                     \
    do {                                                         \
        TEST_PARSE_NUMBER(tihi::Json::PARSE_OK, expect, actual); \
    } while (0)

static void test_parse_number() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json());

#if 1
    TEST_PARSE_NUMBER_OK(0.0, "0");
    TEST_PARSE_NUMBER_OK(0.0, "0");
    TEST_PARSE_NUMBER_OK(0.0, "-0");
    TEST_PARSE_NUMBER_OK(0.0, "-0.0");
    TEST_PARSE_NUMBER_OK(1.0, "1");
    TEST_PARSE_NUMBER_OK(-1.0, "-1");
    TEST_PARSE_NUMBER_OK(1.5, "1.5");
    TEST_PARSE_NUMBER_OK(-1.5, "-1.5");
    TEST_PARSE_NUMBER_OK(3.1416, "3.1416");
    TEST_PARSE_NUMBER_OK(1E10, "1E10");
    TEST_PARSE_NUMBER_OK(1e10, "1e10");
    TEST_PARSE_NUMBER_OK(1E+10, "1E+10");
    TEST_PARSE_NUMBER_OK(1E-10, "1E-10");
    TEST_PARSE_NUMBER_OK(-1E10, "-1E10");
    TEST_PARSE_NUMBER_OK(-1e10, "-1e10");
    TEST_PARSE_NUMBER_OK(-1E+10, "-1E+10");
    TEST_PARSE_NUMBER_OK(-1E-10, "-1E-10");
    TEST_PARSE_NUMBER_OK(1.234E+10, "1.234E+10");
    TEST_PARSE_NUMBER_OK(1.234E-10, "1.234E-10");
#endif

#if 0
    /* the smallest number > 1 */
    // TEST_PARSE_NUMBER_OK(1.0000000000000002, "1.0000000000000002"); 
    // /* minimum denormal */
    TEST_PARSE_NUMBER_OK( 4.9406564584124654e-324, "4.9406564584124654e-324");
    // TEST_PARSE_NUMBER_OK(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    // /* Max subnormal double */
    // TEST_PARSE_NUMBER_OK( 2.2250738585072009e-308, "2.2250738585072009e-308");  
    // TEST_PARSE_NUMBER_OK(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    // TEST_PARSE_NUMBER_OK( 2.2250738585072014e-308, "2.2250738585072014e-308");
    // /* Min normal positive double */  
    // TEST_PARSE_NUMBER_OK(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    // /* Max double */
    // TEST_PARSE_NUMBER_OK( 1.7976931348623157e+308, "1.7976931348623157e+308");  
    // TEST_PARSE_NUMBER_OK(-1.7976931348623157e+308, "-1.7976931348623157e+308");
#endif

#if 1
    /* must underflow */
    TEST_ERROR(tihi::Json::PARSE_NUMBER_OUT_OF_RANGE, "1e-10000");
    TEST_ERROR(tihi::Json::PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(tihi::Json::PARSE_INVALID_VALUE, "+1");
    /* at least one digit before '.' */
    TEST_ERROR(tihi::Json::PARSE_INVALID_VALUE, ".123");
    // /* at least one digit after '.' */
    TEST_ERROR(tihi::Json::PARSE_INVALID_VALUE, "1.");
    TEST_ERROR(tihi::Json::PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(tihi::Json::PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(tihi::Json::PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(tihi::Json::PARSE_INVALID_VALUE, "nan");

    /* invalid number */
    TEST_ERROR(tihi::Json::PARSE_INVALID_VALUE,
               "0123"); /* after zero should be '.' , 'E' , 'e' or nothing */
    TEST_ERROR(tihi::Json::PARSE_INVALID_VALUE, "0x0");
    TEST_ERROR(tihi::Json::PARSE_INVALID_VALUE, "0x123");

    TEST_ERROR(tihi::Json::PARSE_NUMBER_OUT_OF_RANGE, "1e309");
    TEST_ERROR(tihi::Json::PARSE_NUMBER_OUT_OF_RANGE, "-1e309");
#endif
}

#define TEST_PARSE_STR(expect, actual)                                       \
    do {                                                                     \
        TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_STRING, \
                         actual);                                            \
        EXPECT_EQ_STR(expect, json_value->get_str(),                         \
                      json_value->get_str_size());                           \
    } while (0)

static void test_parse_str() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json());
    TEST_PARSE_STR("", "\"\"");
    TEST_PARSE_STR("Hello", "\"Hello\"");
    TEST_PARSE_STR("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_PARSE_STR("\" \\ / \b \f \n \r \t",
                   "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");

    // TEST_PARSE_STR("Hello\0World", "\"Hello\\u0000World\"");
    TEST_PARSE_STR("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
    TEST_PARSE_STR("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
    TEST_PARSE_STR("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
    TEST_PARSE_STR("\xF0\x9D\x84\x9E",
                   "\"\\uD834\\uDD1E\""); /* G clef sign U+1D11E */
    TEST_PARSE_STR("\xF0\x9D\x84\x9E",
                   "\"\\ud834\\udd1e\""); /* G clef sign U+1D11E */
}

static void test_parse_missing_quotation_mark() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json());
    TEST_ERROR(tihi::Json::PARSE_MISS_QUOTATION_MARK, "\"");
    TEST_ERROR(tihi::Json::PARSE_MISS_QUOTATION_MARK, "\"abc");
}

static void test_parse_invalid_string_escape() {
#if 1
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json());
    TEST_ERROR(tihi::Json::PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");
#endif
}

static void test_parse_invalid_string_char() {
#if 1
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json());
    TEST_ERROR(tihi::Json::PARSE_INVALID_STRING_CHAR, "\"\x01\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_STRING_CHAR, "\"\x1F\"");
#endif
}

static void test_access_null() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json());

    json_value->set_str("a");
    json_value->set_type(tihi::JsonValue::JSON_NULL);
    EXPECT_EQ_INT(tihi::JsonValue::JSON_NULL, json_value->get_type());
}

static void test_access_boolean() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json());

    json_value->set_str("a");
    json_value->set_type(tihi::JsonValue::JSON_TRUE);
    EXPECT_EQ_INT(tihi::JsonValue::JSON_TRUE, json_value->get_type());
    json_value->set_type(tihi::JsonValue::JSON_FALSE);
    EXPECT_EQ_INT(tihi::JsonValue::JSON_FALSE, json_value->get_type());
}

static void test_access_number() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json());

    json_value->set_str("a");
    json_value->set_number(1234.5);
    EXPECT_EQ_DOUBLE(1234.5, json_value->get_number());
}

static void test_access_string() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json());

    json_value->set_str("");
    EXPECT_EQ_STR("", json_value->get_str(), json_value->get_str_size());
    json_value->set_str("hello");
    EXPECT_EQ_STR("hello", json_value->get_str(), json_value->get_str_size());
}

static void test_access() {
    test_access_null();
    test_access_boolean();
    test_access_number();
    test_access_string();
}

static void test_parse_invalid_unicode_hex() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json());

    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\u\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\u0\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\u01\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\u012\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\u/000\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\uG000\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\u0/00\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\u0G00\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\u00/0\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\u00G0\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\u000/\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\u000G\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\u 123\"");
}

static void test_parse_invalid_unicode_surrogate() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json());

    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\uD800\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\uDBFF\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\uD800\\\\\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\uD800\\uDBFF\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\uD800\\uE000\"");
}

#define EXPECT_EQ_SIZE_T(expect, actual)                              \
    EXPECT_EQ_BASE((expect) == (actual), static_cast<size_t>(expect), \
                   static_cast<size_t>(actual))

static void test_parse_array() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json());

    EXPECT_EQ_INT(tihi::Json::PARSE_OK, json->parse("[ ]", json_value));
    EXPECT_EQ_INT(tihi::JsonValue::JSON_ARRAY, json_value->get_type());
    EXPECT_EQ_SIZE_T(0, json_value->get_vec_size());
    json_value.reset();

    json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    EXPECT_EQ_INT(
        tihi::Json::PARSE_OK,
        json->parse("[null, true, false, \"hello\", 19]", json_value));
    EXPECT_EQ_INT(tihi::JsonValue::JSON_ARRAY, json_value->get_type());
    EXPECT_EQ_SIZE_T(5, json_value->get_vec_size());
    EXPECT_EQ_INT(tihi::JsonValue::JSON_NULL,
                  json_value->get_vec()[0]->get_type());
    EXPECT_EQ_INT(tihi::JsonValue::JSON_TRUE,
                  json_value->get_vec()[1]->get_type());
    EXPECT_EQ_INT(tihi::JsonValue::JSON_FALSE,
                  json_value->get_vec()[2]->get_type());
    EXPECT_EQ_INT(tihi::JsonValue::JSON_STRING,
                  json_value->get_vec()[3]->get_type());
    EXPECT_EQ_INT(tihi::JsonValue::JSON_NUMBER,
                  json_value->get_vec()[4]->get_type());
    json_value.reset();

    json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    EXPECT_EQ_INT(tihi::Json::PARSE_OK,
                  json->parse("[[], [0], [0, 1], [0,1,2]]", json_value));
    EXPECT_EQ_INT(tihi::JsonValue::JSON_ARRAY, json_value->get_type());
    EXPECT_EQ_SIZE_T(4, json_value->get_vec_size());
    std::vector<tihi::JsonValue::ptr> tmp_vec = json_value->get_vec();
    for (size_t i = 0; i < tmp_vec.size(); ++i) {
        for (size_t j = 0; j < i; ++j) {
            EXPECT_EQ_INT(tihi::JsonValue::JSON_NUMBER,
                          tmp_vec[i]->get_vec()[j]->get_type());
            EXPECT_EQ_DOUBLE(static_cast<double>(j),
                             tmp_vec[i]->get_vec()[j]->get_number());
        }
    }
}

static void test_parse_obj() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json);

    EXPECT_EQ_INT(tihi::Json::PARSE_OK, json->parse("{ }", json_value));
    EXPECT_EQ_INT(tihi::JsonValue::JSON_OBJECT, json_value->get_type());
    EXPECT_EQ_SIZE_T(0, json_value->get_obj_size());

    json_value.reset();
    json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    EXPECT_EQ_INT(tihi::Json::PARSE_OK,
                  json->parse(" { "
                              "\"n\" : null , "
                              "\"f\" : false , "
                              "\"t\" : true , "
                              "\"i\" : 123 , "
                              "\"s\" : \"abc\", "
                              "\"a\" : [ 1, 2, 3 ],"
                              "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
                              " } ",
                              json_value));
    EXPECT_EQ_INT(tihi::JsonValue::JSON_OBJECT, json_value->get_type());
    EXPECT_EQ_SIZE_T(7, json_value->get_obj_size());
    EXPECT_EQ_INT(tihi::JsonValue::JSON_OBJECT,
                  json_value->get_value_from_obj_by_string("o")->get_type());
    EXPECT_EQ_INT(tihi::JsonValue::JSON_NULL,
                  json_value->get_value_from_obj_by_string("n")->get_type());
}

static void test_parse_miss_comma_or_square_bracket() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json);

    TEST_ERROR(tihi::Json::PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1");
    //  TODO
    // TEST_ERROR(tihi::Json::PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1}");
    TEST_ERROR(tihi::Json::PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1 2");
    TEST_ERROR(tihi::Json::PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[[]");
}

static void test_parse_miss_key() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json);

    TEST_ERROR(tihi::Json::PARSE_MISS_KEY, "{:1,");
    TEST_ERROR(tihi::Json::PARSE_MISS_KEY, "{1:1,");
    TEST_ERROR(tihi::Json::PARSE_MISS_KEY, "{true:1,");
    TEST_ERROR(tihi::Json::PARSE_MISS_KEY, "{false:1,");
    TEST_ERROR(tihi::Json::PARSE_MISS_KEY, "{null:1,");
    TEST_ERROR(tihi::Json::PARSE_MISS_KEY, "{[]:1,");
    TEST_ERROR(tihi::Json::PARSE_MISS_KEY, "{{}:1,");
    //  TODO
    // TEST_ERROR(tihi::Json::PARSE_MISS_KEY, "{\"a\":1,");
}

static void test_parse_miss_colon() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json);

    TEST_ERROR(tihi::Json::PARSE_MISS_COLON, "{\"a\"}");
    TEST_ERROR(tihi::Json::PARSE_MISS_COLON, "{\"a\",\"b\"}");
}

static void test_parse_miss_comma_or_curly_bracket() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json);

    TEST_ERROR(tihi::Json::PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1");
    //  TODO
    // TEST_ERROR(tihi::Json::PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1]");
    // TEST_ERROR(tihi::Json::PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1
    // \"b\"");
    TEST_ERROR(tihi::Json::PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":{}");
}

#define TEST_ROUNDTRIP(json1)                                                     \
    do {                                                                    \
        tihi::JsonValue::ptr json_value =                                   \
            tihi::JsonValue::ptr(new tihi::JsonValue);                      \
        tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json);             \
        EXPECT_EQ_INT(tihi::Json::PARSE_OK, json->parse(json1, json_value)); \
        std::string json2;                                                  \
        EXPECT_EQ_INT(tihi::Json::STRINGIFY_OK,                             \
                      json->stringify(json2, json_value));                  \
        EXPECT_EQ_STR(json1, json2, json2.size());                           \
    } while (0)

static void test_stringify_number() {
    TEST_ROUNDTRIP("0");
    TEST_ROUNDTRIP("-0");
    TEST_ROUNDTRIP("1");
    TEST_ROUNDTRIP("-1");
    TEST_ROUNDTRIP("1.5");
    TEST_ROUNDTRIP("-1.5");
    TEST_ROUNDTRIP("3.25");
    TEST_ROUNDTRIP("1e+20");
    TEST_ROUNDTRIP("1.234e+20");
    TEST_ROUNDTRIP("1.234e-20");

    // TEST_ROUNDTRIP("1.0000000000000002"); /* the smallest number > 1 */
    // TEST_ROUNDTRIP("4.9406564584124654e-324"); /* minimum denormal */
    // TEST_ROUNDTRIP("-4.9406564584124654e-324");
    // TEST_ROUNDTRIP("2.2250738585072009e-308");  /* Max subnormal double */
    // TEST_ROUNDTRIP("-2.2250738585072009e-308");
    // TEST_ROUNDTRIP("2.2250738585072014e-308");  /* Min normal positive double */
    // TEST_ROUNDTRIP("-2.2250738585072014e-308");
    // TEST_ROUNDTRIP("1.7976931348623157e+308");  /* Max double */
    // TEST_ROUNDTRIP("-1.7976931348623157e+308");
}

static void test_stringify_string() {
    TEST_ROUNDTRIP("\"\"");
    TEST_ROUNDTRIP("\"Hello\"");
    TEST_ROUNDTRIP("\"Hello\\nWorld\"");
    TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
    TEST_ROUNDTRIP("\"Hello\\u0000World\"");
}

static void test_stringify_array() {
    TEST_ROUNDTRIP("[]");
    TEST_ROUNDTRIP("[null,false,true,123,\"abc\",[1,2,3]]");
}

static void test_stringify_object() {
    TEST_ROUNDTRIP("{}");
    TEST_ROUNDTRIP("{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}");
}

static void test_stringify() {
    TEST_ROUNDTRIP("null");
    TEST_ROUNDTRIP("true");
    TEST_ROUNDTRIP("false");
    test_stringify_number();
    test_stringify_string();
    test_stringify_array();
    test_stringify_object();
}

static void test() {
    test_parse_value();
    test_parse_number();
    test_parse_str();
    test_parse_missing_quotation_mark();
    test_parse_invalid_string_escape();
    test_parse_invalid_string_char();
    test_access();
    test_parse_invalid_unicode_hex();
    test_parse_invalid_unicode_surrogate();
    test_parse_array();
    test_parse_obj();
    test_parse_miss_comma_or_square_bracket();
    test_parse_miss_key();
    test_parse_miss_colon();
    test_parse_miss_comma_or_curly_bracket();

    test_stringify();
}

struct MyStruct {
    using ptr = std::shared_ptr<MyStruct>;
    int i;
};

static void mytest() {
    std::unordered_map<std::string, MyStruct::ptr> u;
    u["h"] = MyStruct::ptr(new MyStruct());
    std::cout << u["h"]->i << std::endl;
}

int main(int argc, char** argv) {
#if 1
    test();
    std::cout << test_pass << "/" << test_count << " passed\n";
#endif

#if 0
    mytest();
#endif

    return main_ret;
}