#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "oauth.h"

std::unordered_map<std::string, access_token_t> token_to_access_token;
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> token_to_token_permissions;
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> user_to_user_permissions;
std::vector<std::string> user_list;