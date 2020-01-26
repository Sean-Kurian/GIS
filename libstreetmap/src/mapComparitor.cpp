#include <string>
#include <strings.h>

struct mapCaseCompare {
    bool operator()(const std::string& street1, const std::string& street2) {
        return strcasecmp(street1.c_str(), street2.c_str()) < 0;
    }
};