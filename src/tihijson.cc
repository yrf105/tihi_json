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

    m_value->set_type(JsonValue::JSON_UNKNOW);

    Json::STATUS ret = parse_value(str);

    if (ret == PARSE_OK && m_context->curr_pos != str.size()) {
        if (str[m_context->curr_pos] == ' ') {
            m_context->curr_pos =
                str.find_first_not_of(" \t", m_context->curr_pos);
            if (m_context->curr_pos != std::string::npos) {
                ret = PARSE_ROOT_NOT_SINGULAR;
            }
        } else {
            ret = PARSE_INVALID_VALUE;
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
        case '\0':
            return PARSE_EXPECT_VALUE;
        default:
            return PARSE_INVALID_VALUE;
    }
}

/*null = "null"*/
Json::STATUS Json::parse_null(const std::string& str) {
    if (m_context->curr_pos + 3 >= str.size()) {
        return PARSE_INVALID_VALUE;
    }

    if (str.substr(m_context->curr_pos, 4) != "null") {
        return PARSE_INVALID_VALUE;
    }

    m_context->curr_pos += 4;
    m_value->set_type(JsonValue::JSON_NULL);
    return PARSE_OK;
}

/*false= "false"*/
Json::STATUS Json::parse_false(const std::string& str) {
    if (m_context->curr_pos + 4 >= str.size()) {
        return PARSE_INVALID_VALUE;
    }

    if (str.substr(m_context->curr_pos, 5) != "false") {
        return PARSE_INVALID_VALUE;
    }

    m_context->curr_pos += 5;
    m_value->set_type(JsonValue::JSON_FALSE);
    return PARSE_OK;
}

/*true = "true"*/
Json::STATUS Json::parse_true(const std::string& str) {
    if (m_context->curr_pos + 3 >= str.size()) {
        return PARSE_INVALID_VALUE;
    }

    if (str.substr(m_context->curr_pos, 4) != "true") {
        return PARSE_INVALID_VALUE;
    }

    m_context->curr_pos += 4;
    m_value->set_type(JsonValue::JSON_TRUE);
    return PARSE_OK;
}

}  // end of namespace tihi