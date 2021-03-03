#ifndef HARVARD_REFERENCES_SERVER_FIELD_H
#define HARVARD_REFERENCES_SERVER_FIELD_H
#include <string_view>
#include <array>
#include "../includes/json.h"

namespace schema {
    using type_t = nlohmann::json::value_t;
    inline std::string to_string(const nlohmann::json::value_t type) noexcept
    {
        using value_t = nlohmann::json::value_t;
        {
            switch (type)
            {
                case value_t::null:
                    return "null";
                case value_t::object:
                    return "object";
                case value_t::array:
                    return "array";
                case value_t::string:
                    return "string";
                case value_t::boolean:
                    return "boolean";
                case value_t::binary:
                    return "binary";
                case value_t::discarded:
                    return "discarded";
                default:
                    return "number";
            }
        }
    }

    struct StringRules {
        unsigned _minLength;
        unsigned _maxLength;
        std::string_view _regex;
    };

    struct UnsignedRules {
        unsigned _minVal;
        unsigned _maxVal;
    };

    union ArrayElementRules {
        StringRules _stringRules;
        UnsignedRules _unsignedRules;
    };

    struct ArrayRules {
        unsigned _minLength;
        unsigned _maxLength;
        type_t _innerType;
        ArrayElementRules _innerRules;
    };

    union Rules {
        StringRules _stringRules;
        UnsignedRules _unsignedRules;
        ArrayRules _arrayRules;
    };

    struct Field {
        std::string_view _name;
        type_t _type;
        bool _required;
        std::array<std::string_view, 10> _requiredIfPresent;
        std::array<std::string_view, 10> _requiredIfEmpty;
        Rules _rules;

        constexpr Field operator()() const {
            Field f{
                    this->_name,
                    this->_type,
                    this->_required,
                    this->_requiredIfPresent,
                    this->_requiredIfEmpty,
                    this->_rules
            };
            return f;
        }

        constexpr Field required() const {
            Field f = (*this)();
            f._required = true;
            return f;
        }

        constexpr Field optional() const {
            Field f = (*this)();
            f._required = false;
            return f;
        }

        template<typename... names>
        constexpr Field requiredIfPresent(names... fields) const {
            Field f{
                    this->_name,
                    this->_type,
                    this->_required,
                    {fields...},
                    this->_requiredIfEmpty,
                    this->_rules
            };
            return f;
        }

        template<typename... names>
        constexpr Field requiredIfEmpty(names... fields) const {
            Field f{
                    this->_name,
                    this->_type,
                    this->_required,
                    this->_requiredIfPresent,
                    {fields...},
                    this->_rules
            };
            return f;
        }

        constexpr bool uninitialized() const {
            return (_name.empty());
        }
    };

    inline constexpr const char* REGEX_URL = R"(https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{2,256}\.[a-z]{2,4}\b([-a-zA-Z0-9@:%_\+.~#?&//=]*))";
    inline constexpr const char* REGEX_DATE = R"((\d\d\d\d)-(0?1|0?2|0?3|0?4|0?5|0?6|0?7|0?8|0?9|10|11|12)-(0[1-9]|[12][0-9]|3[01]))";
}
#endif //HARVARD_REFERENCES_SERVER_FIELD_H