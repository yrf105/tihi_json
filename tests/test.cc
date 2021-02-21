#include <stdint.h>

#include <cstring>
#include <iostream>
#include <sstream>

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

#define TEST_PARSE_VALUE(status, type, str)                 \
    do {                                                    \
        EXPECT_EQ_INT(status, json->parse(str));            \
        EXPECT_EQ_INT(type, json->get_value()->get_type()); \
    } while (0)

#define TEST_ERROR(error, str)                                    \
    do {                                                          \
        TEST_PARSE_VALUE(error, tihi::JsonValue::JSON_NULL, str); \
    } while (0)

static void test_parse_value() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));


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

    TEST_ERROR(tihi::Json::PARSE_ROOT_NOT_SINGULAR, "null  \r\n");
    TEST_ERROR(tihi::Json::PARSE_ROOT_NOT_SINGULAR, "null  l");

    TEST_ERROR(tihi::Json::PARSE_ROOT_NOT_SINGULAR, "false  \r\n");
    TEST_ERROR(tihi::Json::PARSE_ROOT_NOT_SINGULAR, "false  l");

    TEST_ERROR(tihi::Json::PARSE_ROOT_NOT_SINGULAR, "true  \r\n");
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
            EXPECT_EQ_DOUBLE(expect, json->get_value()->get_number());  \
        }                                                               \
    } while (0)

#define TEST_PARSE_NUMBER_OK(expect, actual)                     \
    do {                                                         \
        TEST_PARSE_NUMBER(tihi::Json::PARSE_OK, expect, actual); \
    } while (0)

static void test_parse_number() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));

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
    TEST_ERROR(tihi::Json::PARSE_INVALID_VALUE, "0123"); /* after zero should be '.' , 'E' , 'e' or nothing */
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
        EXPECT_EQ_STR(expect, json->get_value()->get_str(),          \
                      json->get_value()->get_str_size());                    \
    } while (0)

static void test_parse_str() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));
    // TEST_PARSE_STR("", "\"\"");
    // TEST_PARSE_STR("Hello", "\"Hello\"");
    // TEST_PARSE_STR("Hello\nWorld", "\"Hello\\nWorld\"");
    // TEST_PARSE_STR("\" \\ / \b \f \n \r \t",
    //                "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    
    // TEST_PARSE_STR("Hello\0World", "\"Hello\\u0000World\"");
    TEST_PARSE_STR("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
    TEST_PARSE_STR("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
    TEST_PARSE_STR("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
    TEST_PARSE_STR("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
    TEST_PARSE_STR("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}

static void test_parse_missing_quotation_mark() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));
    TEST_ERROR(tihi::Json::PARSE_MISS_QUOTATION_MARK, "\"");
    TEST_ERROR(tihi::Json::PARSE_MISS_QUOTATION_MARK, "\"abc");
}

static void test_parse_invalid_string_escape() {
#if 1
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));
    TEST_ERROR(tihi::Json::PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");
#endif
}

static void test_parse_invalid_string_char() {
#if 1
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));
    TEST_ERROR(tihi::Json::PARSE_INVALID_STRING_CHAR, "\"\x01\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_STRING_CHAR, "\"\x1F\"");
#endif
}

static void test_access_null() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));

    json->get_value()->set_str("a");
    json->get_value()->set_type(tihi::JsonValue::JSON_NULL);
    EXPECT_EQ_INT(tihi::JsonValue::JSON_NULL, json->get_value()->get_type());
}

static void test_access_boolean() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));

    json->get_value()->set_str("a");
    json->get_value()->set_type(tihi::JsonValue::JSON_TRUE);
    EXPECT_EQ_INT(tihi::JsonValue::JSON_TRUE, json->get_value()->get_type());
    json->get_value()->set_type(tihi::JsonValue::JSON_FALSE);
    EXPECT_EQ_INT(tihi::JsonValue::JSON_FALSE, json->get_value()->get_type());
}

static void test_access_number() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));

    json->get_value()->set_str("a");
    json->get_value()->set_number(1234.5);
    EXPECT_EQ_DOUBLE(1234.5, json->get_value()->get_number());
}

static void test_access_string() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));

    json->get_value()->set_str("");
    EXPECT_EQ_STR("", json->get_value()->get_str(), json->get_value()->get_str_size());
    json->get_value()->set_str("hello");
    EXPECT_EQ_STR("hello", json->get_value()->get_str(), json->get_value()->get_str_size());
}

static void test_access() {
    test_access_null();
    test_access_boolean();
    test_access_number();
    test_access_string();
}

static void test_parse_invalid_unicode_hex() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));

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
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));

    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\uD800\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\uDBFF\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\uD800\\\\\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\uD800\\uDBFF\"");
    TEST_ERROR(tihi::Json::PARSE_INVALID_UNICODE_HEX, "\"\\uD800\\uE000\"");
}

static void test() {
    // test_parse_value();
    // test_parse_number();
    test_parse_str();
    // test_parse_missing_quotation_mark();
    // test_parse_invalid_string_escape();
    // test_parse_invalid_string_char();
    // test_access();
    test_parse_invalid_unicode_hex();
    test_parse_invalid_unicode_surrogate();
}


static void mytest() {
    std::stringstream ss;
    ss << char(0x24);
    std::cout << ss.str() << std::endl;
    
    std::cout << "\x0024" << std::endl;

    printf("%x\n", (0x24));
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