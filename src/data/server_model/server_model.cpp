#include "server_model.h"

std::unordered_map<std::string, access_token_t> token_to_access_token;
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> token_to_token_permissions;
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> user_to_user_permissions;
std::vector<std::string> user_list;
std::vector<std::string> resource_list;
std::vector<std::unordered_map<std::string, std::string>> user_to_approvals_list;
