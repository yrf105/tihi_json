#include <stdint.h>

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
    } while (0);

#define EXPECT_EQ_INT(expect, actual) \
    EXPECT_EQ_BASE((expect) == (actual), expect, actual)

#define TEST_PARSE_VALUE(status, type, str)      \
    do {                                         \
        EXPECT_EQ_INT(status, json->parse(str)); \
        EXPECT_EQ_INT(type, json->get_type());   \
    } while (0)

#define TEST_PARSE_INVALID(actual)                             \
    do {                                                       \
        TEST_PARSE_VALUE(tihi::Json::PARSE_INVALID_VALUE,      \
                         tihi::JsonValue::JSON_ERROR, actual); \
    } while (0)

#define TEST_PARSE_NOT_SINGULAR(actual)                              \
    do {                                                       \
        TEST_PARSE_VALUE(tihi::Json::PARSE_ROOT_NOT_SINGULAR,  \
                         tihi::JsonValue::JSON_ERROR, actual); \
    } while (0)

static void test_parse_value() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));


    TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_NULL, "null");
    TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_NULL,
                     "null  ");
    TEST_PARSE_NOT_SINGULAR("null  \r\n");
    TEST_PARSE_NOT_SINGULAR("null  l");

    TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_FALSE,
                     "false");
    TEST_PARSE_VALUE( tihi::Json::PARSE_OK, tihi::JsonValue::JSON_FALSE,
                     "false  ");
    TEST_PARSE_NOT_SINGULAR("false  \r\n");
    TEST_PARSE_NOT_SINGULAR("false  l");

    TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_TRUE, "true");
    TEST_PARSE_VALUE(tihi::Json::PARSE_OK, tihi::JsonValue::JSON_TRUE, 
                     "true  ");
    TEST_PARSE_NOT_SINGULAR("true  \r\n");
    TEST_PARSE_NOT_SINGULAR("true  l");
}

#define EXPECT_EQ_DOUBLE(expect, actual) \
    do {                                 \
        EXPECT_EQ_INT(expect, actual)    \
    } while (0)

#define TEST_PARSE_NUMBER(status, expect, actual)                       \
    do {                                                                \
        TEST_PARSE_VALUE(status, tihi::JsonValue::JSON_NUMBER, actual); \
        if (status == tihi::Json::PARSE_OK) {                           \
            EXPECT_EQ_DOUBLE(expect, json->get_number());               \
        }                                                               \
    } while (0)

#define TEST_PARSE_NUMBER_OK(expect, actual)                     \
    do {                                                         \
        TEST_PARSE_NUMBER(tihi::Json::PARSE_OK, expect, actual); \
    } while (0)

static void test_parse_number() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));
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

    TEST_PARSE_INVALID("1e-10000"); /* must underflow */
    TEST_PARSE_INVALID("+0");
    TEST_PARSE_INVALID("+1");
    TEST_PARSE_INVALID(".123"); /* at least one digit before '.' */
    TEST_PARSE_INVALID("1.");   /* at least one digit after '.' */
    TEST_PARSE_INVALID("INF");
    TEST_PARSE_INVALID("inf");
    TEST_PARSE_INVALID("NAN");
    TEST_PARSE_INVALID("nan");
}

static void test() {
    test_parse_value();
    test_parse_number();
}


int main(int argc, char** argv) {
    test();
    std::cout << test_pass << "/" << test_count << " passed\n";
    return main_ret;
}