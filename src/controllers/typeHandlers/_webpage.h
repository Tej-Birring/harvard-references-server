#pragma once
#include "_typeHandler.h"
#include "../../schema/schema.h"
#include "../utils/utils.h"
inline constexpr const ReferenceTypeHandler webpage {
        "webpage",
        {
                schema::fields::webpageTitle.required(),
                schema::fields::websiteTitle,

                schema::fields::authors,
                schema::fields::organization,

                schema::fields::yearPublished,

                schema::fields::url.required(),
                schema::fields::dateAccessed.required(),
        },
        [](nlohmann::json &req) -> std::string {
            using namespace std;
            using namespace utils::names;
            using namespace utils::html;
            using namespace utils::dateTime;
            stringstream oHtml;

            // title
            const bool hasWebsiteTitle = !req["website title"].empty();
            // contributors
            const bool hasAuthors = !req["authors"].empty();
            const bool hasOrganizationName = !req["organization"].empty();
            // publication info
            const bool hasYearPublished = !req["year published"].empty();

            /* attribution string */
            if (hasAuthors) {
                oHtml << joinList(reverseAbbreviatedNames(req["authors"]));
            }
            else if (hasOrganizationName) {
                oHtml << str(req["organization"]);
            }
            else {
                oHtml << em << str(req["webpage title"]) << _em;
            }
            if (hasYearPublished) {
                oHtml << " (" << req["year published"] << ").";
            }
            else {
                oHtml << " (n.d.).";
            }
            /* webpage title string */
            if (hasAuthors || hasOrganizationName) {
                oHtml << " " << em << str(req["webpage title"]) << _em << " [Online].";
            } else {
                oHtml << " [Online].";
            }
            /* website title string */
            if (hasWebsiteTitle) {
                oHtml << " " << str(req["website title"]) << ".";
            }
            /* access info */
            oHtml << " Available from: " << lnk(req["url"]);
            oHtml << " [Accessed " << toLongDateString(req["date accessed"]) << "].";

            return oHtml.str();
        }
};