#include "tihijson.h"

namespace tihi {
Json::Json(JsonValue::ptr value) : m_value(value) {

}

int Json::parse(const std::string& str) {
    return PARSE_OK;
}

} // end of namespace tihi