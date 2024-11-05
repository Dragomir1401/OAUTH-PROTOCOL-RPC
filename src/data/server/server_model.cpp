#include "server_model.hpp"

ServerModel::ServerModel()
{
    user_count = 0;
}

ServerModel::ServerModel(
    int user_count,
    std::unordered_map<std::string, access_token> token_to_access_token,
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> token_to_token_permissions,
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> user_to_user_permissions,
    std::vector<std::string> user_list)
{
    this->user_count = user_count;
    this->token_to_access_token = token_to_access_token;
    this->token_to_token_permissions = token_to_token_permissions;
    this->user_to_user_permissions = user_to_user_permissions;
    this->user_list = user_list;
}

ServerModel::~ServerModel()
{
}