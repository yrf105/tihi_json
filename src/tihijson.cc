#include "tihijson.h"

#include <assert.h>

#include <iostream>
#include <sstream>
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

#define SKIP_WS                                                    \
    do {                                                           \
        m_context->curr_pos =                                      \
            str.find_first_not_of(" \t\r\n", m_context->curr_pos); \
    } while (0)

int JsonValue::get_type() const { return m_type; }

void JsonValue::set_type(Type v) {
    std::string().swap(m_str);
    std::vector<ptr>().swap(m_vec);
    m_type = v;
}

double JsonValue::get_number() const {
    ASSERT2(m_type == JSON_NUMBER, "类型错误");
    return m_number;
}
void JsonValue::set_number(double v) {
    m_type = JSON_NUMBER;
    std::string().swap(m_str);
    std::vector<ptr>().swap(m_vec);
    m_number = v;
}

const std::string& JsonValue::get_str() const {
    ASSERT2(m_type == JSON_STRING, "类型错误");
    return m_str;
}
void JsonValue::set_str(const std::string v) {
    m_type = JSON_STRING;
    std::vector<ptr>().swap(m_vec);
    m_str = v;
}

size_t JsonValue::get_str_size() const {
    ASSERT2(m_type == JSON_STRING, "类型错误");
    return m_str.size();
}

const std::vector<JsonValue::ptr>& JsonValue::get_vec() const {
    ASSERT2(m_type == JSON_ARRAY, "类型错误");
    return m_vec;
}

void JsonValue::set_vec(const std::vector<JsonValue::ptr> v) {
    m_type = JSON_ARRAY;
    std::string().swap(m_str);
    m_vec = v;
}

size_t JsonValue::get_vec_size() const {
    ASSERT2(m_type == JSON_ARRAY, "类型错误");
    return m_vec.size();
}

void JsonValue::push_back_vec(JsonValue::ptr v) {
    m_type = JsonValue::JSON_ARRAY;
    m_vec.push_back(v);
}

const std::unordered_map<std::string, JsonValue::ptr>& JsonValue::get_obj()
    const {
    ASSERT2(m_type == JSON_OBJECT, "类型错误");
    return m_obj;
}

void JsonValue::set_obj(const std::unordered_map<std::string, ptr> v) {
    m_type = JSON_OBJECT;
    std::string().swap(m_str);
    std::vector<JsonValue::ptr>().swap(m_vec);
    m_obj = v;
}

size_t JsonValue::get_obj_size() const {
    ASSERT2(m_type == JSON_OBJECT, "类型错误");
    return m_obj.size();
}

void JsonValue::insert_obj(const std::string& k, JsonValue::ptr v) {
    m_type = JSON_OBJECT;
    m_obj[k] = v;
}

const JsonValue::ptr JsonValue::get_value_from_obj_by_string(
    const std::string& s) {
    ASSERT2(m_type == JSON_OBJECT, "类型错误");
    if (m_obj.find(s) == m_obj.end()) {
        return nullptr;
    }

    return m_obj[s];
}

Json::Json(JsonContxt::ptr context) : m_context(context) {}

Json::STATUS Json::parse(const std::string& str, JsonValue::ptr json_value) {
    m_context->curr_pos = 0;

    if (str.empty()) {
        return PARSE_EXPECT_VALUE;
    }

    m_context->curr_pos = str.find_first_not_of(" \t\r\n", m_context->curr_pos);

    if (m_context->curr_pos == str.npos) {
        return PARSE_EXPECT_VALUE;
    }

    json_value->set_type(JsonValue::JSON_NULL);

    Json::STATUS ret = parse_value(str, json_value);

    if (ret == PARSE_OK && m_context->curr_pos < str.size()) {
        m_context->curr_pos =
            str.find_first_not_of(" \t\r\n", m_context->curr_pos);
        if (m_context->curr_pos != std::string::npos) {
            ret = PARSE_ROOT_NOT_SINGULAR;
            json_value->set_type(JsonValue::JSON_NULL);
        }
    }

    return ret;
}

static std::unordered_map<char, std::string> ESCAPE2CHAR{
    {'\b', "\\b"}, {'\f', "\\f"},  {'\n', "\\n"}, {'\r', "\\r"},
    {'\t', "\\t"}, {'\"', "\\\""}, {'/', "/"},  {'\\', "\\\\"}};

int Json::stringify(std::string& str, JsonValue::ptr json_value) {
    std::string().swap(str);

    if (json_value == nullptr) {
        return Json::STRINGIFY_ERROR;
    }

    std::stringstream ss;

    /*
        JSON_NULL = 1,
        JSON_FALSE = 2,
        JSON_TRUE = 3,
        JSON_NUMBER = 4,
        JSON_STRING = 5,
        JSON_ARRAY = 6,
        JSON_OBJECT = 7
    */
    switch (json_value->get_type()) {
        case JsonValue::JSON_NULL: {
            ss << "null";
            break;
        }
        case JsonValue::JSON_TRUE: {
            ss << "true";
            break;
        }
        case JsonValue::JSON_FALSE: {
            ss << "false";
            break;
        }
        case JsonValue::JSON_NUMBER: {
            ss << json_value->get_number();
            break;
        }
        case JsonValue::JSON_STRING: {
            const std::string& str_tmp = json_value->get_str();
            ss << "\"";
            for (auto c : str_tmp) {
                if (ESCAPE2CHAR.find(c) != ESCAPE2CHAR.end()) {
                    ss << ESCAPE2CHAR[c];
                } else if (c < 0x20) {
                    char tmp_s[7];
                    sprintf(tmp_s, "\\u%04X", c);
                    ss << tmp_s;
                } else {
                    ss << c;
                }
            }
            ss << "\"";
            break;
        }
        case JsonValue::JSON_ARRAY: {
            const std::vector<JsonValue::ptr>& tmp_vec = json_value->get_vec();
            size_t sz = tmp_vec.size();

            if (sz == 0) {
                ss << "[]";
                break;
            }

            ss << '[';
            std::string tmp_str;
            int ret = stringify(tmp_str, tmp_vec[0]);
            if (ret != STRINGIFY_OK) {
                return ret;
            }
            ss << tmp_str;

            for (size_t i = 1; i < sz; ++i) {
                ss << ',';
                stringify(tmp_str, tmp_vec[i]);
                ss << tmp_str;
            }
            ss << ']';

            break;
        }
        case JsonValue::JSON_OBJECT: {
            const std::unordered_map<std::string, JsonValue::ptr>& obj_tmp = 
                json_value->get_obj();
            size_t sz = obj_tmp.size();
            
            if (sz == 0) {
                ss << "{}";
                break;
            }

            ss << '{';
            std::string tmp_str;
            size_t pair_counts = 0;
            for (const auto& p : obj_tmp) {
                int ret = stringify(tmp_str, p.second);
                if (ret != Json::STRINGIFY_OK) {
                    return Json::STRINGIFY_ERROR;
                }

                if (pair_counts == sz - 1) {
                    ss << '\"' << p.first << '\"' << ':' << tmp_str;
                    ++pair_counts;
                    break;
                }
                
                ss << '\"' << p.first << '\"' << ':' << tmp_str << ',';
                ++pair_counts;
            }
            ss << '}';
            break;
        }
    }

    str = ss.str();

    return STRINGIFY_OK;
}

Json::STATUS Json::parse_value(const std::string& str,
                               JsonValue::ptr json_value) {
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
            return parse_null(str, json_value);
        case 'f':
            return parse_false(str, json_value);
        case 't':
            return parse_true(str, json_value);
        case '\"':
            return parse_str(str, json_value);
        case '[':
            return parse_vec(str, json_value);
        case '{':
            return parse_obj(str, json_value);
        case '\0':
            return PARSE_EXPECT_VALUE;
        default:
            return parse_number(str, json_value);
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
        json_value->set_type(type);                           \
        return PARSE_OK;                                      \
    } while (0)

/*null = "null"*/
Json::STATUS Json::parse_null(const std::string& str,
                              JsonValue::ptr json_value) {
    XX("null", str, JsonValue::JSON_NULL, 4);
}

/*false= "false"*/
Json::STATUS Json::parse_false(const std::string& str,
                               JsonValue::ptr json_value) {
    XX("false", str, JsonValue::JSON_FALSE, 5);
}

/*true = "true"*/
Json::STATUS Json::parse_true(const std::string& str,
                              JsonValue::ptr json_value) {
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
    STATE_END = 9
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
    CHAR_SQUARE_BRACKET_BRACES,
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
    } else if (ch == ']' || ch == '}' || ch == ',') {
        return CHAR_SQUARE_BRACKET_BRACES;
    } else {
        return CHAR_ILLEGAL;
    }
}

bool Json::is_number(const std::string& str) {
    std::unordered_map<State, std::unordered_map<CharType, State>> transfer{
        {STATE_INITIAL,
         {{CHAR_SPACE, STATE_INITIAL},
          {CHAR_DIGIT, STATE_DIGIT},
          {CHAR_NEGATIVE_SING, STATE_INI_SIGN},
          {CHAR_ZERO, STATE_INI_ZERO}}},
        {STATE_INI_SIGN,
         {{CHAR_DIGIT, STATE_DIGIT}, {CHAR_ZERO, STATE_INI_ZERO}}},
        {STATE_INI_ZERO,
         {{CHAR_POINT, STATE_POINT},
          {CHAR_SPACE, STATE_END},
          {CHAR_SQUARE_BRACKET_BRACES, STATE_END}}},
        {STATE_DIGIT,
         {{CHAR_DIGIT, STATE_DIGIT},
          {CHAR_ZERO, STATE_DIGIT},
          {CHAR_EXP, STATE_EXP},
          {CHAR_POINT, STATE_POINT},
          {CHAR_SPACE, STATE_END},
          {CHAR_SQUARE_BRACKET_BRACES, STATE_END}}},
        {STATE_POINT,
         {{CHAR_DIGIT, STATE_FRACTION}, {CHAR_ZERO, STATE_FRACTION}}},
        {STATE_FRACTION,
         {{CHAR_DIGIT, STATE_FRACTION},
          {CHAR_ZERO, STATE_FRACTION},
          {CHAR_EXP, STATE_EXP},
          {CHAR_SPACE, STATE_END},
          {CHAR_SQUARE_BRACKET_BRACES, STATE_END}}},
        {STATE_EXP,
         {{CHAR_POSITIVE_SING, STATE_EXP_SIGN},
          {CHAR_NEGATIVE_SING, STATE_EXP_SIGN},
          {CHAR_DIGIT, STATE_EXP_NUMBER},
          {CHAR_ZERO, STATE_EXP_NUMBER}}},
        {STATE_EXP_SIGN,
         {{CHAR_DIGIT, STATE_EXP_NUMBER}, {CHAR_ZERO, STATE_EXP_NUMBER}}},
        {STATE_EXP_NUMBER,
         {{CHAR_DIGIT, STATE_EXP_NUMBER},
          {CHAR_ZERO, STATE_EXP_NUMBER},
          {CHAR_SPACE, STATE_END},
          {CHAR_SQUARE_BRACKET_BRACES, STATE_END}}},
        {STATE_END,
         {{CHAR_SPACE, STATE_END}, {CHAR_SQUARE_BRACKET_BRACES, STATE_END}}}};

    int len = str.size();
    State st = STATE_INITIAL;

    for (int i = m_context->curr_pos; i < len; ++i) {
        CharType typ = to_char_type(str[i]);
        // std::cout << "typ: " << typ << std::endl;
        if (transfer[st].find(typ) == transfer[st].end()) {
            return false;
        } else {
            st = transfer[st][typ];
            if (st == STATE_END) {
                return true;
            }
        }
    }

    return st == STATE_END || st == STATE_DIGIT || st == STATE_FRACTION ||
           st == STATE_EXP_NUMBER || st == STATE_INI_ZERO;
}

Json::STATUS Json::parse_number(const std::string& str,
                                JsonValue::ptr json_value) {
    if (!is_number(str)) {
        return Json::PARSE_INVALID_VALUE;
    }

    std::size_t n = 0; /*已处理字符数*/
    double tmp = 0;
    try {
        tmp = std::stod(str.substr(m_context->curr_pos), &n);
    } catch (...) {
        return Json::PARSE_NUMBER_OUT_OF_RANGE;
    }

    if (n == 0) {
        return Json::PARSE_INVALID_VALUE;
    }
    m_context->curr_pos += n;
    json_value->set_number(tmp);
    return Json::PARSE_OK;
}

std::unordered_map<char, uint8_t> CHAR2U8{
    {'0', 0},  {'1', 1},  {'2', 2},  {'3', 3},  {'4', 4},  {'5', 5},
    {'6', 6},  {'7', 7},  {'8', 8},  {'9', 9},  {'A', 10}, {'a', 10},
    {'B', 11}, {'b', 11}, {'C', 12}, {'c', 12}, {'D', 13}, {'d', 13},
    {'E', 14}, {'e', 14}, {'F', 15}, {'f', 15},
};

static bool parse_hex4(const std::string& str, uint16_t& u) {
    int n = str.size();
    if (n != 4) {
        return 0;
    }

    u = 0;
    for (int i = 0; i < n; ++i) {
        if (CHAR2U8.find(str[i]) == CHAR2U8.end()) {
            u = 0;
            return false;
        }
        u |= CHAR2U8[str[i]];
        if (i == n - 1) {
            break;
        }
        u = u << 4;
    }

    return true;
}

static const std::string decode_utf8(const std::string& str, size_t& pos) {
    std::stringstream ss;
    size_t n = str.size();
    if (pos + 4 >= n) {
        return ss.str();
    }

    uint32_t u = 0;
    uint16_t u_h = 0;
    uint16_t u_l = 0;
    if (!parse_hex4(str.substr(pos, 4), u_h)) {
        return ss.str();
    }
    pos += 4;

    if (u_h >= 0xd800 && u_h <= 0xdbff) {
        if (pos + 1 >= n) {
            return ss.str();
        }

        if (str[pos] == '\\' && str[pos + 1] == 'u') {
            pos += 2;
            if (pos + 4 >= n) {
                return ss.str();
            }
            if (!parse_hex4(str.substr(pos, 4), u_l)) {
                return ss.str();
            }

            if (!(u_l >= 0xdc00 && u_l <= 0xdfff)) {
                return ss.str();
            }

            pos += 4;
            u = 0x10000 + ((u_h - 0xD800) << 10) + (u_l - 0xDC00);
        } else {
            return ss.str();
        }
    } else {
        u = u_h;
    }

    if (u >= 0x0000 && u <= 0x007f) {
        ss << char(u & 0x7f);
    } else if (u >= 0x0080 && u <= 0x07ff) {
        ss << char(0xc0 | ((u >> 6) & 0x1f));
        ss << char(0x80 | (u & 0x3f));
    } else if (u >= 0x0800 && u <= 0xffff) {
        ss << char(0xe0 | ((u >> 12) & 0x0f));
        ss << char(0x80 | ((u >> 6) & 0x3f));
        ss << char(0x80 | ((u)&0x3f));
    } else if (u >= 0x10000 && 0x10ffff) {
        ss << char(0xf0 | ((u >> 18) & 0x07));
        ss << char(0x80 | ((u >> 12) & 0x3f));
        ss << char(0x80 | ((u >> 6) & 0x3f));
        ss << char(0x80 | ((u)&0x3f));
    }

    return ss.str();
}

static std::unordered_map<char, char> CHAR2ESCAPE{
    {'b', '\b'}, {'f', '\f'},  {'n', '\n'}, {'r', '\r'},
    {'t', '\t'}, {'\"', '\"'}, {'/', '/'},  {'\\', '\\'}};

Json::STATUS Json::parse_str(const std::string& str,
                             JsonValue::ptr json_value) {
    size_t sz = str.size();

    ++(m_context->curr_pos);
    std::string tmp;
    while (m_context->curr_pos < sz && str[m_context->curr_pos] != '\"') {
        if (str[m_context->curr_pos] == '\\') {
            ++(m_context->curr_pos);
            if (CHAR2ESCAPE.find(str[m_context->curr_pos]) !=
                CHAR2ESCAPE.end()) {
                tmp.push_back(CHAR2ESCAPE[str[m_context->curr_pos]]);
                ++(m_context->curr_pos);
            } else if (str[m_context->curr_pos] == 'u') {
                ++(m_context->curr_pos);
                std::string tmp_utf8 = decode_utf8(str, m_context->curr_pos);
                if (tmp_utf8.empty()) {
                    json_value->set_type(JsonValue::JSON_NULL);
                    return PARSE_INVALID_UNICODE_HEX;
                }

                tmp += tmp_utf8;
            } else {
                json_value->set_type(JsonValue::JSON_NULL);
                return PARSE_INVALID_STRING_ESCAPE;
            }
            continue;
        }

        if (str[m_context->curr_pos] <= 31) {
            json_value->set_type(JsonValue::JSON_NULL);
            return PARSE_INVALID_STRING_CHAR;
        }

        tmp.push_back(str[m_context->curr_pos]);
        ++(m_context->curr_pos);
    }

    if (m_context->curr_pos >= sz) {
        return Json::PARSE_MISS_QUOTATION_MARK;
    }

    json_value->set_str(tmp);
    ++(m_context->curr_pos);
    return Json::PARSE_OK;
}

Json::STATUS Json::parse_str_raw(const std::string& str, std::string& ret) {
    size_t sz = str.size();

    if (str[m_context->curr_pos] != '\"') {
        return Json::PARSE_MISS_KEY;
    }

    ++(m_context->curr_pos);

    std::string tmp;
    while (m_context->curr_pos < sz && str[m_context->curr_pos] != '\"') {
        if (str[m_context->curr_pos] == '\\') {
            ++(m_context->curr_pos);
            if (CHAR2ESCAPE.find(str[m_context->curr_pos]) !=
                CHAR2ESCAPE.end()) {
                tmp.push_back(CHAR2ESCAPE[str[m_context->curr_pos]]);
                ++(m_context->curr_pos);
            } else if (str[m_context->curr_pos] == 'u') {
                ++(m_context->curr_pos);
                std::string tmp_utf8 = decode_utf8(str, m_context->curr_pos);
                if (tmp_utf8.empty()) {
                    return PARSE_INVALID_UNICODE_HEX;
                }

                tmp += tmp_utf8;
            } else {
                return PARSE_INVALID_STRING_ESCAPE;
            }
            continue;
        }

        if (str[m_context->curr_pos] <= 31) {
            return PARSE_INVALID_STRING_CHAR;
        }

        tmp.push_back(str[m_context->curr_pos]);
        ++(m_context->curr_pos);
    }

    if (m_context->curr_pos >= sz) {
        return Json::PARSE_MISS_QUOTATION_MARK;
    }

    ret = tmp;
    ++(m_context->curr_pos);
    return Json::PARSE_OK;
}

Json::STATUS Json::parse_vec(const std::string& str,
                             JsonValue::ptr json_value) {
    size_t sz = str.size();
    if (sz < 2) {
        return PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
    }

    ++(m_context->curr_pos);
    m_context->curr_pos = str.find_first_not_of(" \t\r\n", m_context->curr_pos);

    if (str[m_context->curr_pos] == ']') {
        ++(m_context->curr_pos);
        json_value->set_vec({});
        return PARSE_OK;
    }

    while (m_context->curr_pos < sz) {
        JsonValue::ptr tmp_json_value = JsonValue::ptr(new JsonValue);
        Json::STATUS ret = parse_value(str, tmp_json_value);
        if (ret != Json::PARSE_OK) {
            json_value->set_type(JsonValue::JSON_NULL);
            return ret;
        }

        json_value->push_back_vec(tmp_json_value);

        m_context->curr_pos =
            str.find_first_not_of(" \t\r\n", m_context->curr_pos);
        if (m_context->curr_pos == str.npos) {
            json_value->set_type(JsonValue::JSON_NULL);
            return PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
        }

        if (str[(m_context->curr_pos)] == ',') {
            ++(m_context->curr_pos);
            m_context->curr_pos =
                str.find_first_not_of(" \t\r\n", m_context->curr_pos);
        } else if (str[(m_context->curr_pos)] == ']') {
            ++(m_context->curr_pos);
            return Json::PARSE_OK;
        }
    }

    json_value->set_type(JsonValue::JSON_NULL);
    return Json::PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
}

Json::STATUS Json::parse_obj(const std::string& str,
                             JsonValue::ptr json_value) {
    size_t sz = str.size();
    if (sz - m_context->curr_pos < 2) {
        return Json::PARSE_MISS_BRACES;
    }

    ++(m_context->curr_pos);
    m_context->curr_pos = str.find_first_not_of(" \t\r\n", m_context->curr_pos);

    json_value->set_obj(std::unordered_map<std::string, JsonValue::ptr>());

    if (str[m_context->curr_pos] == '}') {
        ++(m_context->curr_pos);
        return PARSE_OK;
    }

    while (m_context->curr_pos < sz) {
        std::string tmp_key;
        Json::STATUS ret = parse_str_raw(str, tmp_key);
        if (ret != Json::PARSE_OK) {
            json_value->set_type(JsonValue::JSON_NULL);
            return ret;
        }

        m_context->curr_pos =
            str.find_first_not_of(" \t\r\n", m_context->curr_pos);

        if (str[m_context->curr_pos] != ':') {
            json_value->set_type(JsonValue::JSON_NULL);
            return Json::PARSE_MISS_COLON;
        }

        ++(m_context->curr_pos);
        m_context->curr_pos =
            str.find_first_not_of(" \t\r\n", m_context->curr_pos);
        JsonValue::ptr tmp_json_value = JsonValue::ptr(new JsonValue);
        ret = parse_value(str, tmp_json_value);
        if (ret != Json::PARSE_OK) {
            json_value->set_type(JsonValue::JSON_NULL);
            return ret;
        }

        json_value->insert_obj(tmp_key, tmp_json_value);

        m_context->curr_pos =
            str.find_first_not_of(" \t\r\n", m_context->curr_pos);
        if (str[m_context->curr_pos] == ',') {
            ++(m_context->curr_pos);
            m_context->curr_pos =
                str.find_first_not_of(" \t\r\n", m_context->curr_pos);
        } else if (str[m_context->curr_pos] == '}') {
            ++(m_context->curr_pos);
            return PARSE_OK;
        }
    }

    json_value->set_type(JsonValue::JSON_NULL);
    return Json::PARSE_MISS_COMMA_OR_CURLY_BRACKET;
}
}  // end of namespace tihi