#ifndef TIHIJSON_TIHIJSON_H_
#define TIHIJSON_TIHIJSON_H_

#include <iostream>
#include <memory>
#include <string>

namespace tihi {

struct JsonContxt {
    using ptr = std::shared_ptr<JsonContxt>;

    size_t curr_pos = 0;
};

class JsonValue {
public:
    using ptr = std::shared_ptr<JsonValue>;
    // json 只有 6 中数据类型
    enum Type {
        // JSON_ERROR = 0,
        JSON_NULL = 1,
        JSON_FALSE = 2,
        JSON_TRUE = 3,
        JSON_NUMBER = 4,
        JSON_STRING = 5,
        JSON_ARRAY = 6,
        JSON_OBJECT = 7
    };

    Type get_type() const;
    void set_type(Type v);

    double get_number() const;
    void set_number(double v);

    const std::string& get_str() const;
    void set_str(const std::string v);
    size_t get_str_size() const;

private:
    Type m_type;
    double m_number;
    std::string m_str;
};

class Json {
public:
    using ptr = std::shared_ptr<Json>;
    Json(JsonValue::ptr value,
         JsonContxt::ptr context = JsonContxt::ptr(new JsonContxt));

    enum STATUS {
        PARSE_OK = 0,
        PARSE_EXPECT_VALUE = 1,       // 只含有空白
        PARSE_INVALID_VALUE = 2,      // 不是合法字符
        PARSE_ROOT_NOT_SINGULAR = 3,  // 值 空白 之后还有字符
        PARSE_MISS_QUOTATION_MARK = 4,
        PARSE_INVALID_STRING_ESCAPE = 5,
        PARSE_INVALID_STRING_CHAR = 6,
        PARSE_NUMBER_OUT_OF_RANGE = 7,
    };

    JsonValue::ptr get_value() const;

    STATUS parse(const std::string& str);

private:
    STATUS parse_value(const std::string& str);
    STATUS parse_null(const std::string& str);
    STATUS parse_false(const std::string& str);
    STATUS parse_true(const std::string& str);
    STATUS parse_number(const std::string& str);
    STATUS parse_str(const std::string& str);

private:
    JsonValue::ptr m_value;
    JsonContxt::ptr m_context;
};

}  // end of namespace tihi

#endif  // TIHIJSON_TIHIJSON_H_