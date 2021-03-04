#pragma once
#include "fields.h"
#include "boost/regex.hpp"
#include "../controllers/errors.h"
#include <array>
#include <iostream>
namespace schema::fields {
// private stuff
    namespace {
        void checkString(const std::string_view &fieldName, const StringRules &stringRules,
                         const nlohmann::json &val) {
            const auto &str = static_cast<nlohmann::json::string_t>(val);
            // check length
            const auto size = str.size();
            const auto allowedMin = stringRules._minLength;
            const auto allowedMax = stringRules._maxLength;
            if (size < allowedMin || size > allowedMax) {
                throw ControllerError("String value of field " + singleQuote(fieldName) +
                                      " exceeds permissible size. Min size " + std::to_string(allowedMin) +
                                      ", max size " + std::to_string(allowedMax) + "."
                );
            }
            // check regex
            const auto regex = stringRules._regex;
            if (!regex.empty()) {
                boost::regex rgx{std::string(regex)};
                if (!boost::regex_match(str, rgx)) {
                    throw ControllerError("String value of field " +
                                          singleQuote(fieldName) + " is not of the correct format."
                    );
                }
            }
        }

        void checkUnsigned(const std::string_view &fieldName, const UnsignedRules &unsignedRules,
                           const nlohmann::json &val) {
            const auto &num = static_cast<nlohmann::json::number_unsigned_t>(val);
            // check range
            const auto allowedMin = unsignedRules._minVal;
            const auto allowedMax = unsignedRules._maxVal;
            if (num < allowedMin || num > allowedMax) {
                throw ControllerError("Unsigned number value of field " + singleQuote(fieldName) +
                                      " exceeds permissible range. Min val " + std::to_string(allowedMin) +
                                      ", max val " + std::to_string(allowedMax) + "."
                );
            }
        }

        void
        checkArray(const std::string_view &fieldName, const ArrayRules &arrayRules, const nlohmann::json &val) {
            using namespace schema;
            // check length
            const auto size = val.size();
            const auto allowedMin = arrayRules._minLength;
            const auto allowedMax = arrayRules._maxLength;
            if (size < allowedMin || size > allowedMax) {
                throw ControllerError("Array field " + singleQuote(fieldName) +
                                      " exceeds permissible size. Min size " + std::to_string(allowedMin) +
                                      ", max size " + std::to_string(allowedMax) + "."
                );
            }
            // inner type
            for (auto &innerVal : val) {
                if (innerVal.type() != arrayRules._innerType) {
                    throw ControllerError("Expected inner value of field " + singleQuote(fieldName) +
                                          " to be of type " + singleQuote(toString(arrayRules._innerType)) +
                                          ", but got type " + singleQuote(toString(innerVal.type())) + "."
                    );
                }
                // check string
                if (val.type() == ValType::string) {
                    checkString(fieldName, arrayRules._innerRules._stringRules, innerVal);
                }
                    // check unsigned
                else if (val.type() == ValType::number_unsigned) {
                    checkUnsigned(fieldName, arrayRules._innerRules._unsignedRules, innerVal);
                }
            }
        }

        void checkValueByType(const std::string_view &fieldName, const nlohmann::json::value_t requiredType,
                              const Rules &rule, const nlohmann::json &val) {
            using namespace schema;
            // check type
            if (val.type() != requiredType) {
                throw ControllerError("Expected value of field " + singleQuote(fieldName) +
                                      " to be of type " + singleQuote(toString(requiredType)) +
                                      ", but got type " + singleQuote(toString(val.type())) + "."
                );
            }
            // check string
            if (val.type() == ValType::string) {
                checkString(fieldName, rule._stringRules, val);
            }
                // check unsigned
            else if (val.type() == ValType::number_unsigned) {
                checkUnsigned(fieldName, rule._unsignedRules, val);
            }
                // check array
            else if (val.type() == ValType::array) {
                checkArray(fieldName, rule._arrayRules, val);
            }
        }

        void ifEmpty(nlohmann::json &req, const Field &field) {
            // obtain value: error if required but nothing read
            if (field._required)
                throw ControllerError("Mandatory field " + singleQuote(field._name) + " is missing.");
            // obtain value: error if required (due to absence of other fields) but nothing read
            unsigned nToCheck = 0;
            unsigned nPresent = 0;
            for (const auto &coField : field._requiredIfEmpty) {
                if (coField.empty())
                    break;
                nToCheck++;
                if (!req[std::string(coField)].empty())
                    nPresent++;
            }
            if (nToCheck > 0 && nPresent == 0) {
                throw ControllerError("Mandatory field group missing. "
                                      "One of the following must be specified: " +
                                      listFields(field._name, field._requiredIfEmpty)
                );
            }
        }
    }




    template<size_t N>
    void verifyFields(nlohmann::json &req, std::array<Field, N> fields) {
        using namespace std;

        for (const Field &field : fields) {
            // ignore if this is not a valid, initialized field.
            if (field.uninitialized()) continue;
            // else:
            #ifdef SERVER_DEBUG
            cout << "       Verifying field: '" << field._name << "'" << endl;
            #endif
            // obtain value: fetch
            auto &value = req[std::string(field._name)];
            // ** if nothing read **
            if (value.empty()) {
                ifEmpty(req, field);
                continue;
            }
            // ** else **
            // check for required coFields
            for (const auto &coField : field._requiredIfPresent) {
                if (coField.empty())
                    break;
                auto &coFieldValue = req[std::string(coField)];
                if (coFieldValue.empty()) {
                    throw ControllerError(
                            singleQuote(coField) + " is required because " +
                            singleQuote(field._name) + " is specified."
                    );
                }
            }
            // check type & type-specific constraints
            checkValueByType(field._name, field._type, field._rules, value);
        }
    }
}