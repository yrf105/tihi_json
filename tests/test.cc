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

#define TEST_PARSE_VALUE(type, status, str)      \
    do {                                         \
        EXPECT_EQ_INT(status, json->parse(str)); \
        EXPECT_EQ_INT(type, json->get_type());   \
    } while (0);

static void test() {
    tihi::JsonValue::ptr json_value = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));


    TEST_PARSE_VALUE(tihi::JsonValue::JSON_NULL, tihi::Json::PARSE_OK, "null");
    TEST_PARSE_VALUE(tihi::JsonValue::JSON_NULL, tihi::Json::PARSE_OK,
                     "null  ");
    TEST_PARSE_VALUE(tihi::JsonValue::JSON_NULL,
                     tihi::Json::PARSE_ROOT_NOT_SINGULAR, "null  \r\n");
    TEST_PARSE_VALUE(tihi::JsonValue::JSON_NULL,
                     tihi::Json::PARSE_ROOT_NOT_SINGULAR, "null  l");

    TEST_PARSE_VALUE(tihi::JsonValue::JSON_FALSE, tihi::Json::PARSE_OK,
                     "false");
    TEST_PARSE_VALUE(tihi::JsonValue::JSON_FALSE, tihi::Json::PARSE_OK,
                     "false  ");
    TEST_PARSE_VALUE(tihi::JsonValue::JSON_FALSE,
                     tihi::Json::PARSE_ROOT_NOT_SINGULAR, "false  \r\n");
    TEST_PARSE_VALUE(tihi::JsonValue::JSON_FALSE,
                     tihi::Json::PARSE_ROOT_NOT_SINGULAR, "false  l");

    TEST_PARSE_VALUE(tihi::JsonValue::JSON_TRUE, tihi::Json::PARSE_OK, "true");
    TEST_PARSE_VALUE(tihi::JsonValue::JSON_TRUE, tihi::Json::PARSE_OK,
                     "true  ");
    TEST_PARSE_VALUE(tihi::JsonValue::JSON_TRUE,
                     tihi::Json::PARSE_ROOT_NOT_SINGULAR, "true  \r\n");
    TEST_PARSE_VALUE(tihi::JsonValue::JSON_TRUE,
                     tihi::Json::PARSE_ROOT_NOT_SINGULAR, "true  l");
}


int main(int argc, char** argv) {
    test();
    std::cout << test_pass << "/" << test_count << " passed\n";
    return main_ret;
}