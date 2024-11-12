#include "../../rpc/oauth.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>

// Storage model for the client
class ClientModel
{
public:
    ClientModel();
    ClientModel(
        std::unordered_map<std::string, access_token_t> user_to_access_token);
    ~ClientModel();
    std::unordered_map<std::string, access_token_t> get_user_to_access_token();
    void log(std::string log, int level);
    void remove_access_token(std::string user_id);
    void decrease_lifetime(std::string user_id);
    void add_access_token(std::string user_id, access_token_t access_token);

private:
    std::unordered_map<std::string, access_token_t> user_to_access_token;
};
