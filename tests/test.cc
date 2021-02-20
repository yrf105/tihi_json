#include <stdint.h>

#include <cstring>
#include <iostream>

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
                       std::string(expect) == std::string(expect), \
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
        EXPECT_EQ_STR(expect, json->get_value()->get_str().c_str(),          \
                      json->get_value()->get_str_size());                    \
    } while (0)

static void test_parse_str() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));
    TEST_PARSE_STR("", "\"\"");
    TEST_PARSE_STR("Hello", "\"Hello\"");
    TEST_PARSE_STR("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_PARSE_STR("\" \\ / \b \f \n \r \t",
                   "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
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
    TEST_PARSE_STR("\v", "\"\\v\"");
    TEST_PARSE_STR("\'", "\"\\'\"");
    TEST_PARSE_STR("\0", "\"\\0\"");
    TEST_PARSE_STR("\x12", "\"\\x12\"");
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

static void test() {
    // test_parse_value();
    // test_parse_number();
    test_parse_str();
    test_parse_missing_quotation_mark();
    test_parse_invalid_string_escape();
    test_parse_invalid_string_char();
}


static void mytest() {
    std::string str = "\"Hello\\nWorld\"";
    std::cout << str << std::endl;
    std::string tmp;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\') {
            // std::cout << str[i + 2] << std::endl;
            // ++i;
            // continue;
        }
        std::cout << str[i];
        tmp.push_back(str[i]);
    }
    std::cout << tmp << std::endl; 
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