#include "oauth.h"
#include <unordered_map>

class ClientModel
{
public:
    ClientModel();
    ClientModel(
        std::unordered_map<std::string, access_token_t> user_to_access_token);
    ~ClientModel();

private:
    std::unordered_map<std::string, access_token_t> user_to_access_token;
};
