#include <string>
#include <functional>
#include <sstream>
#include <iomanip>


std::string hashed(const std::string& password) {
    std::hash<std::string> hasher;
    size_t hash_value = hasher(password);


    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << hash_value;
    return oss.str();
}
