#pragma once
#include <string>
#include <string_view>
namespace utils::html {


    inline constexpr const char* em = "<em>";
    inline constexpr const char* _em = "</em>";
    inline constexpr const char* sup = "<sup>";
    inline constexpr const char* _sup = "</sup>";


    inline const std::string& str(const std::string& in) { return in; }


    inline std::string lnk(const std::string& url) {
        return "<a href='" + url + "' target='_blank' rel='noreferrer noopener nofollow'>" + url + "</a>";
    }


    inline std::string ord(const unsigned n) {
        const unsigned j = n % 10;
        const unsigned k = n % 100;

        if (j==1 && k!=11) {
            return std::to_string(n) + sup + "st" + _sup;
        }
        else if (j==2 && k!=12) {
            return std::to_string(n) + sup + "nd" + _sup;
        }
        else if (j==3 && k!=13) {
            return std::to_string(n) + sup + "rd" + _sup;
        }
        else {
            return std::to_string(n) + sup + "th" + _sup;
        }
    }


}