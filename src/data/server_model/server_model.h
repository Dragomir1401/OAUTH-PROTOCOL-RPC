#ifndef SERVER_MODEL_H
#define SERVER_MODEL_H

#include <string>
#include <unordered_map>
#include <vector>
#include "oauth.h"
#include <iostream>
#include <fstream>

class Token
{
public:
    enum TOKEN_STATUS
    {
        ACCESS,
        AUTH_SIGNED,
        AUTH_UNSIGNED
    };
    Token();
    Token(std::string token, std::string user_id, int lifetime, TOKEN_STATUS status);
    Token(std::string token, std::string refresh_token, std::string user_id, int lifetime, TOKEN_STATUS status);
    Token(std::unordered_map<std::string, std::string> approvals, std::string token, std::string user_id, int lifetime, TOKEN_STATUS status);
    ~Token();
    std::unordered_map<std::string, std::string> get_approvals();
    void add_approval(std::string resource, std::string permission);
    void add_approvals(std::unordered_map<std::string, std::string> approvals);
    std::string get_token();
    std::string get_user_id();
    int get_lifetime();
    TOKEN_STATUS get_status();
    void add_refresh_token(std::string refresh_token);
    void sign();
    void log(std::string message, int level);
    void decrease_lifetime();
    std::unordered_map<std::string, std::string> get_operation_to_code();

private:
    void initialize_operation_to_code();
    std::unordered_map<std::string, std::string> approvals;
    std::string token;
    std::string refresh_token;
    std::string user_id;
    int lifetime;
    TOKEN_STATUS status;
    std::unordered_map<std::string, std::string> operation_to_code;
};

extern std::vector<std::string> user_list;
extern std::vector<std::string> resource_list;
extern std::vector<std::unordered_map<std::string, std::string>> user_to_approvals_list;
extern std::vector<Token> auth_token_list;
extern std::unordered_map<std::string, Token> user_to_access_token;
extern int global_token_lifetime;

#endif
