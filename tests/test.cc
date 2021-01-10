#include <stdint.h>
#include "../src/tihijson.h"

#include <iostream>

static int main_ret = 0;
static uint32_t test_count = 0;
static uint32_t test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual)        \
    do {                                                \
        ++test_count;                                   \
        if (equality) {                                 \
            ++test_pass;                                \
        } else {                                        \
            main_ret = 1;                               \
            std::cerr << __FILE__ << ':' << __LINE__    \
                      << ": expect: " << expect         \
                      << " actual: " << actual          \
                      << std::endl;                     \
        }                                               \
    } while(0);

#define EXPECT_EQ_INT(expect, actual) \
    EXPECT_EQ_BASE((expect)==(actual), expect, actual)

static void test_parse_null() {
    tihi::JsonValue::ptr json_value 
        = tihi::JsonValue::ptr(new tihi::JsonValue);
    tihi::Json::ptr json = tihi::Json::ptr(new tihi::Json(json_value));
    EXPECT_EQ_INT(tihi::Json::PARSE_OK, json->parse("null"));
    EXPECT_EQ_INT(tihi::JsonValue::JSON_NULL, json->get_type());
}

static void test() {
    test_parse_null();
}

int main(int argc, char** argv) {
    test();
    std::cout << test_pass << "/" << test_count << " passed\n";
    return main_ret;
}