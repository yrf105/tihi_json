#ifndef TIHIJSON_TIHIJSON_H_
#define TIHIJSON_TIHIJSON_H_

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
    enum Type {
        JSON_UNKNOW = 0,
        JSON_NULL = 1,
        JSON_FALSE = 2,
        JSON_TRUE = 3,
        JSON_NUMBER = 4,
        JSON_STRING = 5,
        JSON_ARRAY = 6,
        JSON_OBJECT = 7
    };

    Type get_type() const { return m_type; }
    void set_type(Type v) { m_type = v; }

private:
    Type m_type;
};

class Json {
public:
    using ptr = std::shared_ptr<Json>;
    Json(JsonValue::ptr value, 
         JsonContxt::ptr context = JsonContxt::ptr(new JsonContxt));

    enum STATUS {
        PARSE_OK = 0,
        PARSE_EXPECT_VALUE = 1,         // 只含有空白
        PARSE_INVALID_VALUE = 2,        // 不是 null false true 三种字面值
        PARSE_ROOT_NOT_SINGULAR = 3     // 值 空白 之后还有字符
    };

    STATUS parse(const std::string& str);
    JsonValue::Type get_type() const { return m_value->get_type(); }
    void set_type(JsonValue::Type v) { m_value->set_type(v); }

private:
    STATUS parse_value(const std::string& str);
    STATUS parse_null(const std::string& str);
    STATUS parse_false(const std::string& str);
    STATUS parse_true(const std::string& str);

private:
    JsonValue::ptr m_value;
    JsonContxt::ptr m_context;
};

}  // end of namespace tihi

#endif  // TIHIJSON_TIHIJSON_H_