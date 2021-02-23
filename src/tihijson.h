#ifndef TIHIJSON_TIHIJSON_H_
#define TIHIJSON_TIHIJSON_H_

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

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

    int get_type() const;
    void set_type(Type v);

    double get_number() const;
    void set_number(double v);

    const std::string& get_str() const;
    void set_str(const std::string v);
    size_t get_str_size() const;

    const std::vector<ptr>& get_vec() const;
    void set_vec(const std::vector<ptr> v);
    size_t get_vec_size() const;
    void push_back_vec(JsonValue::ptr v);

    const std::unordered_map<std::string, ptr>& get_obj() const;
    void set_obj(const std::unordered_map<std::string, ptr> v);
    size_t get_obj_size() const;
    void insert_obj(const std::string& k, JsonValue::ptr v);
    const ptr get_value_from_obj_by_string(const std::string& s);

private:
    Type m_type;
    double m_number;
    std::string m_str;
    std::vector<ptr> m_vec; 
    std::unordered_map<std::string, ptr> m_obj;
};

class Json {
public:
    using ptr = std::shared_ptr<Json>;
    Json(JsonContxt::ptr context = JsonContxt::ptr(new JsonContxt));

    enum STATUS {
        PARSE_OK = 0,
        PARSE_EXPECT_VALUE = 1,       // 只含有空白
        PARSE_INVALID_VALUE = 2,      // 不是合法字符
        PARSE_ROOT_NOT_SINGULAR = 3,  // 值 空白 之后还有字符
        PARSE_MISS_QUOTATION_MARK = 4,
        PARSE_INVALID_STRING_ESCAPE = 5,
        PARSE_INVALID_STRING_CHAR = 6,
        PARSE_NUMBER_OUT_OF_RANGE = 7,
        PARSE_INVALID_UNICODE_HEX = 8,
        PARSE_MISS_COMMA_OR_SQUARE_BRACKET = 9,
        PARSE_MISS_KEY = 10,
        PARSE_MISS_BRACES = 11,
        PARSE_MISS_COLON = 12,
        PARSE_MISS_COMMA_OR_CURLY_BRACKET = 13,

        STRINGIFY_OK = 14,
        STRINGIFY_ERROR = 14,
    };

    STATUS parse(const std::string& str, JsonValue::ptr json_value);
    int stringify(std::string& str, JsonValue::ptr json_value);

private:
    STATUS parse_value(const std::string& str, JsonValue::ptr json_value);
    STATUS parse_null(const std::string& str, JsonValue::ptr json_value);
    STATUS parse_false(const std::string& str, JsonValue::ptr json_value);
    STATUS parse_true(const std::string& str, JsonValue::ptr json_value);
    bool is_number(const std::string& str);
    STATUS parse_number(const std::string& str, JsonValue::ptr json_value);
    STATUS parse_str(const std::string& str, JsonValue::ptr json_value);
    STATUS parse_str_raw(const std::string& str, std::string& ret);
    STATUS parse_vec(const std::string& str, JsonValue::ptr json_value);
    STATUS parse_obj(const std::string& str, JsonValue::ptr json_value);

private:
    JsonContxt::ptr m_context;
};

}  // end of namespace tihi

#endif  // TIHIJSON_TIHIJSON_H_