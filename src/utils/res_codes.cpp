#include <string>
#include <unordered_map>
#include "res_codes.hpp"

const std::string &ResponseCodes::getString(res_codes code)
{
    initialize();
    return codes_map[code];
}

const bool ResponseCodes::compare(res_codes code, const std::string &str)
{
    return getString(code) == str;
}

// Use lazy initialization to avoid static initialization order fiasco
void ResponseCodes::initialize()
{
    static bool initialized = false;
    if (!initialized)
    {
        codes_map[TOKEN_EXPIRED] = "TOKEN_EXPIRED";
        codes_map[USER_NOT_FOUND] = "USER_NOT_FOUND";
        codes_map[REQUEST_DENIED] = "REQUEST_DENIED";
        codes_map[PERMISSION_DENIED] = "PERMISSION_DENIED";
        codes_map[PERMISSION_GRANTED] = "PERMISSION_GRANTED";
        codes_map[RESOURCE_NOT_FOUND] = "RESOURCE_NOT_FOUND";
        codes_map[OPERATION_NOT_PERMITTED] = "OPERATION_NOT_PERMITTED";

        initialized = true;
    }
}

std::unordered_map<ResponseCodes::res_codes, std::string> ResponseCodes::codes_map;