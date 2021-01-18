#include "tihijson.h"

#include <iostream>

namespace tihi {
Json::Json(JsonValue::ptr value, JsonContxt::ptr context)
    : m_value(value), m_context(context) {}

Json::STATUS Json::parse(const std::string& str) {
    if (str.empty()) {
        return PARSE_EXPECT_VALUE;
    }

    m_context->curr_pos = str.find_first_not_of(" \t\r\n");

    if (m_context->curr_pos == str.size()) {
        return PARSE_EXPECT_VALUE;
    }

    m_value->set_type(JsonValue::JSON_ERROR);

    Json::STATUS ret = parse_value(str);

    if (ret == PARSE_OK && m_context->curr_pos != str.size()) {
        if (str[m_context->curr_pos] == ' ') {
            m_context->curr_pos =
                str.find_first_not_of(" \t", m_context->curr_pos);
            if (m_context->curr_pos != std::string::npos) {
                ret = PARSE_ROOT_NOT_SINGULAR;
                m_value->set_type(JsonValue::JSON_ERROR);
            }
        } else {
            ret = PARSE_INVALID_VALUE;
            m_value->set_type(JsonValue::JSON_ERROR);
        }
    }

    return ret;
}

JsonValue::Type Json::get_type() const {
    if (!m_value) {
        return JsonValue::JSON_ERROR;
    }
    return m_value->get_type();
}

void Json::set_type(JsonValue::Type v) {
    if (m_value) {
        m_value->set_type(v);
    }
}

double Json::get_number() const {
    if (!m_value) {
        return 0;
    }
    return m_value->get_number();
}

void Json::set_number(double v) {
    if (m_value) {
        m_value->set_number(v);
    }
}

Json::STATUS Json::parse_value(const std::string& str) {
    /*
    n ➔ null
    f ➔ false
    t ➔ true
    " ➔ string
    0-9/- ➔ number
    [ ➔ array
    { ➔ object
    */
    switch (str[m_context->curr_pos]) {
        case 'n':
            return parse_null(str);
        case 'f':
            return parse_false(str);
        case 't':
            return parse_true(str);
        case '\0':
            return PARSE_EXPECT_VALUE;
        default:
            return parse_number(str);
    }
}

#define XX(expect, actual, type, len)                         \
    do {                                                      \
        if (m_context->curr_pos + len - 1 >= str.size()) {    \
            return PARSE_INVALID_VALUE;                       \
        }                                                     \
        if (str.substr(m_context->curr_pos, len) != expect) { \
            return PARSE_INVALID_VALUE;                       \
        }                                                     \
        m_context->curr_pos += len;                           \
        m_value->set_type(type);                              \
        return PARSE_OK;                                      \
    } while (0)

/*null = "null"*/
Json::STATUS Json::parse_null(const std::string& str) {
    XX("null", str, JsonValue::JSON_NULL, 4);
}

/*false= "false"*/
Json::STATUS Json::parse_false(const std::string& str) {
    XX("false", str, JsonValue::JSON_FALSE, 5);
}

/*true = "true"*/
Json::STATUS Json::parse_true(const std::string& str) {
    XX("true", str, JsonValue::JSON_TRUE, 4);
}

#undef XX

#define ISDIGIT(ch) ((ch >= '0') && (ch <= '9'))

Json::STATUS Json::parse_number(const std::string& str) {
    char first_char = str[m_context->curr_pos];
    char last_char = str[str.size() - 1];
    if (!((first_char == '-' || ISDIGIT(first_char)) &&
          (last_char == ' ' || ISDIGIT(last_char)))) {
        return Json::PARSE_INVALID_VALUE;
    }

    std::size_t n = 0; /*已处理字符数*/
    double tmp = 0;
    try {
        tmp = std::stod(str, &n);
    } catch (...) {
        return Json::PARSE_INVALID_VALUE;
    }

    if (n == 0) {
        return Json::PARSE_INVALID_VALUE;
    }
    m_context->curr_pos += n;
    m_value->set_type(JsonValue::JSON_NUMBER);
    m_value->set_number(tmp);
    // std::cout << tmp << std::endl;
    return Json::PARSE_OK;
}

}  // end of namespace tihi