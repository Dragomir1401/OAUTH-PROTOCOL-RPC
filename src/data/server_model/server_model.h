#ifndef SERVER_MODEL_H
#define SERVER_MODEL_H

#include <string>
#include <unordered_map>
#include <vector>
#include "oauth.h"

extern std::unordered_map<std::string, access_token_t> token_to_access_token;
extern std::unordered_map<std::string, std::unordered_map<std::string, std::string>> token_to_token_permissions;
extern std::unordered_map<std::string, std::unordered_map<std::string, std::string>> user_to_user_permissions;
extern std::vector<std::string> user_list;
extern std::vector<std::string> resource_list;
extern std::vector<std::unordered_map<std::string, std::string>> user_to_approvals_list;

#endif
