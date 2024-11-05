#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include "oauth.h"

class ServerModel
{
public:
    ServerModel();
    ServerModel(
        int user_count,
        std::unordered_map<std::string, access_token> token_to_access_token,
        std::unordered_map<std::string, unordered_map<std::string, std::string>> token_to_token_permissions,
        std::unordered_map<std::string, unordered_map<std::string, std::string>> user_to_user_permissions,
        std::vector<std::string> user_list);
    ~ServerModel();

private:
    int user_count;
    std::unordered_map<std::string, access_token> token_to_access_token;
    std::unordered_map<std::string, unordered_map<std::string, std::string>> token_to_token_permissions;
    std::unordered_map<std::string, unordered_map<std::string, std::string>> user_to_user_permissions;
    std::vector<std::string> user_list;
};