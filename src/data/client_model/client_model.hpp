#include "../../rpc/oauth.h"
#include <unordered_map>
#include <string>
#include <iostream>

class ClientModel
{
public:
    ClientModel();
    ClientModel(
        std::unordered_map<std::string, access_token_t> user_to_access_token);
    ~ClientModel();
    void add_access_token(std::string user_id, access_token_t access_token);
    void remove_access_token(std::string user_id);
    void log(std::string log);

private:
    std::unordered_map<std::string, access_token_t> user_to_access_token;
};
