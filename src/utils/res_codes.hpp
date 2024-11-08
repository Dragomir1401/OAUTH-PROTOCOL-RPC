#include <string>
#include <unordered_map>

class ResponseCodes
{
public:
    enum res_codes
    {
        TOKEN_EXPIRED,
        USER_NOT_FOUND,
        REQUEST_DENIED,
        PERMISSION_DENIED,
        PERMISSION_GRANTED,
        RESOURCE_NOT_FOUND,
        OPERATION_NOT_PERMITTED
    };

    static const std::string &getString(res_codes code);
    static const bool compare(res_codes code, const std::string &str);

private:
    static std::unordered_map<res_codes, std::string> codes_map;
    static void initialize();
};
