#include "tihijson.h"

#include <assert.h>

#include <iostream>
#include <unordered_map>

namespace tihi {

#define ASSERT(expr)  \
    do {              \
        assert(expr); \
    } while (0)

#define ASSERT2(expr, info)                 \
    do {                                    \
        if (!(expr)) {                      \
            std::cerr << info << std::endl; \
        }                                   \
        assert(expr);                       \
    } while (0)

JsonValue::Type JsonValue::get_type() const { return m_type; }

void JsonValue::set_type(Type v) {
    std::string().swap(m_str);
    m_type = v;
}

double JsonValue::get_number() const {
    ASSERT2(m_type == JSON_NUMBER, "类型错误");
    return m_number;
}
void JsonValue::set_number(double v) {
    m_type = JSON_NUMBER;
    std::string().swap(m_str);
    m_number = v;
}

const std::string& JsonValue::get_str() const {
    ASSERT2(m_type == JSON_STRING, "类型错误");
    return m_str;
}
void JsonValue::set_str(const std::string v) {
    m_type = JSON_STRING;
    m_str = v;
}

size_t JsonValue::get_str_size() const {
    ASSERT2(m_type == JSON_STRING, "类型错误");
    return m_str.size();
}


Json::Json(JsonValue::ptr value, JsonContxt::ptr context)
    : m_value(value), m_context(context) {
    if (m_value == nullptr) {
        std::cerr << "给定的根节点为空" << std::endl;
        m_value = JsonValue::ptr(new JsonValue());
    }
}

JsonValue::ptr Json::get_value() const { return m_value; }

Json::STATUS Json::parse(const std::string& str) {
    if (str.empty()) {
        return PARSE_EXPECT_VALUE;
    }

    m_context->curr_pos = str.find_first_not_of(" \t\r\n");

    if (m_context->curr_pos == str.npos) {
        return PARSE_EXPECT_VALUE;
    }

    m_value->set_type(JsonValue::JSON_NULL);

    Json::STATUS ret = parse_value(str);

    if (ret == PARSE_OK && m_context->curr_pos < str.size()) {
        if (str[m_context->curr_pos] == ' ') {
            m_context->curr_pos =
                str.find_first_not_of(" \t", m_context->curr_pos);
            if (m_context->curr_pos != std::string::npos) {
                ret = PARSE_ROOT_NOT_SINGULAR;
                m_value->set_type(JsonValue::JSON_NULL);
            }
        } else {
            ret = PARSE_INVALID_VALUE;
            m_value->set_type(JsonValue::JSON_NULL);
        }
    }

    return ret;
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
        case '\"':
            return parse_str(str);
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

enum State {
    STATE_INITIAL = 0,
    STATE_INI_SIGN = 1,
    STATE_INI_ZERO = 2,
    STATE_DIGIT = 3,
    STATE_POINT = 4,
    STATE_FRACTION = 5,
    STATE_EXP = 6,
    STATE_EXP_SIGN = 7,
    STATE_EXP_NUMBER = 8,
    STATE_END = 9,
};

enum CharType {
    CHAR_DIGIT,
    CHAR_ZERO,
    CHAR_EXP,
    CHAR_POINT,
    CHAR_POSITIVE_SING,
    CHAR_NEGATIVE_SING,
    CHAR_SPACE,
    CHAR_ILLEGAL,
};

CharType to_char_type(char ch) {
    if (ch > '0' && ch <= '9') {
        return CHAR_DIGIT;
    } else if (ch == '0') {
        return CHAR_ZERO;
    } else if (ch == 'e' || ch == 'E') {
        return CHAR_EXP;
    } else if (ch == '+') {
        return CHAR_POSITIVE_SING;
    } else if (ch == '-') {
        return CHAR_NEGATIVE_SING;
    } else if (ch == '.') {
        return CHAR_POINT;
    } else if (ch == ' ') {
        return CHAR_SPACE;
    } else {
        return CHAR_ILLEGAL;
    }
}

bool is_number(const std::string& str) {
    std::unordered_map<State, std::unordered_map<CharType, State>> transfer {
        {
            STATE_INITIAL, {
                {CHAR_SPACE, STATE_INITIAL},
                {CHAR_DIGIT, STATE_DIGIT},
                {CHAR_NEGATIVE_SING, STATE_INI_SIGN},
                {CHAR_ZERO, STATE_INI_ZERO}
            }
        }, {
            STATE_INI_SIGN, {
                {CHAR_DIGIT, STATE_DIGIT},
                {CHAR_ZERO, STATE_INI_ZERO}
            }
        }, {
            STATE_INI_ZERO, {
                {CHAR_POINT, STATE_POINT},
                {CHAR_SPACE, STATE_END}
            }
        }, {
            STATE_DIGIT, {
                {CHAR_DIGIT, STATE_DIGIT},
                {CHAR_ZERO, STATE_DIGIT},
                {CHAR_EXP, STATE_EXP},
                {CHAR_POINT, STATE_POINT},
                {CHAR_SPACE, STATE_END}
            }
        }, {
            STATE_POINT, {
                {CHAR_DIGIT, STATE_FRACTION},
                {CHAR_ZERO, STATE_FRACTION}
            }
        }, {
            STATE_FRACTION, {
                {CHAR_DIGIT, STATE_FRACTION},
                {CHAR_ZERO, STATE_FRACTION},
                {CHAR_EXP, STATE_EXP},
                {CHAR_SPACE, STATE_END}
            }
        }, {
            STATE_EXP, {
                {CHAR_POSITIVE_SING, STATE_EXP_SIGN},
                {CHAR_NEGATIVE_SING, STATE_EXP_SIGN},
                {CHAR_DIGIT, STATE_EXP_NUMBER},
                {CHAR_ZERO, STATE_EXP_NUMBER}
            }
        }, {
            STATE_EXP_SIGN, {
                {CHAR_DIGIT, STATE_EXP_NUMBER},
                {CHAR_ZERO, STATE_EXP_NUMBER},
            }
        }, {
            STATE_EXP_NUMBER, {
                {CHAR_DIGIT, STATE_EXP_NUMBER},
                {CHAR_ZERO, STATE_EXP_NUMBER},
                {CHAR_SPACE, STATE_END}
            }
        }, {
            STATE_END, {
                {CHAR_SPACE, STATE_END}
            }
        }
    };

    int len = str.size();
    State st = STATE_INITIAL;

    for (int i = 0; i < len; ++i) {
        CharType typ = to_char_type(str[i]);
        // std::cout << "typ: " << typ << std::endl;
        if (transfer[st].find(typ) == transfer[st].end()) {
            return false;
        } else {
            st = transfer[st][typ];
        }
    }

    // std::cout << "st: " << st << std::endl;

    return st == STATE_END || st == STATE_DIGIT || st == STATE_FRACTION
           || st == STATE_EXP_NUMBER || st == STATE_INI_ZERO; 
}

Json::STATUS Json::parse_number(const std::string& str) {

    if (!is_number(str)) {
        return Json::PARSE_INVALID_VALUE;
    }

    // std::cout << "是数字，开始解析" << std::endl;

    std::size_t n = 0; /*已处理字符数*/
    double tmp = 0;
    try {
        tmp = std::stod(str, &n); 
    } catch (...) {
        return Json::PARSE_NUMBER_OUT_OF_RANGE;
    }

    if (n == 0) {
        return Json::PARSE_INVALID_VALUE;
    }
    m_context->curr_pos += n;
    m_value->set_number(tmp);
    return Json::PARSE_OK;
}

static std::unordered_map<char, char> CHAR2ESCAPE {
    {'b', '\b'}, {'f', '\f'}, {'n', '\n'}, {'r', '\r'}, {'t', '\t'},
    {'\"', '\"'}, {'/', '/'}, {'\\', '\\'}
};

Json::STATUS Json::parse_str(const std::string& str) {
    int sz = str.size();
    if (sz < 2) {
        return Json::PARSE_MISS_QUOTATION_MARK;
    }

    int end = m_context->curr_pos + 1;
    std::string tmp;
    while (end < sz && str[end] != '\"') {
        if (str[end] == '\\') {
            if (CHAR2ESCAPE.find(str[end + 1]) != CHAR2ESCAPE.end()) {
                tmp.push_back(CHAR2ESCAPE[str[end + 1]]);
                end += 2;
            } else {
                m_value->set_type(JsonValue::JSON_NULL);
                return PARSE_INVALID_STRING_ESCAPE;
            }
            continue;
        }

        if (str[end] <= 31) {
            m_value->set_type(JsonValue::JSON_NULL);
            return PARSE_INVALID_STRING_CHAR;
        }

        tmp.push_back(str[end]);
        ++end;
    }

    if (end >= sz) {
        return Json::PARSE_MISS_QUOTATION_MARK;
    }

    m_value->set_str(tmp);
    m_context->curr_pos += end + 1;
    return Json::PARSE_OK;
}


}  // end of namespace tihi