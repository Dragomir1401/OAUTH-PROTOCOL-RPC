#include "server_model.h"

std::vector<std::string> user_list;
std::vector<std::string> resource_list;
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> user_to_approvals_list;
std::vector<Token> auth_token_list;
std::unordered_map<std::string, Token> user_to_access_token;
int global_token_lifetime;

Token::Token()
{
}

Token::Token(std::string token, std::string user_id, int lifetime, TOKEN_STATUS status)
{
    this->token = token;
    this->user_id = user_id;
    this->lifetime = lifetime;
    this->status = status;
}

Token::Token(std::string token, std::string refresh_token, std::string user_id, int lifetime, TOKEN_STATUS status)
{
    this->token = token;
    this->refresh_token = refresh_token;
    this->user_id = user_id;
    this->lifetime = lifetime;
    this->status = status;
}

Token::Token(std::unordered_map<std::string, std::string> approvals, std::string token, std::string user_id, int lifetime, TOKEN_STATUS status)
{
    this->approvals = approvals;
    this->token = token;
    this->user_id = user_id;
    this->lifetime = lifetime;
    this->status = status;
}

Token::~Token()
{
}

std::unordered_map<std::string, std::string> Token::get_approvals()
{
    return this->approvals;
}

void Token::add_approval(std::string resource, std::string permission)
{
    this->approvals[resource] = permission;
}

void Token::add_approvals(std::unordered_map<std::string, std::string> approvals)
{
    // add new approvals as deep copy for the ones in the param
    for (auto const &approval : approvals)
    {
        this->approvals[approval.first] = approval.second;
        log("Approval added: " + approval.first + " " + approval.second + " for token " + this->token);
    }
}

std::string Token::get_token()
{
    return this->token;
}

std::string Token::get_user_id()
{
    return this->user_id;
}

int Token::get_lifetime()
{
    return this->lifetime;
}

Token::TOKEN_STATUS Token::get_status()
{
    return this->status;
}

void Token::add_refresh_token(std::string refresh_token)
{
    this->refresh_token = refresh_token;
}

void Token::sign()
{
    log("Token with id " + this->token + " will be signed");

    this->status = AUTH_SIGNED;
    // add .sgn to the token
    this->token += "+.sgn";

    log("Token with id " + this->token + " has been signed");
}

void Token::log(std::string message)
{
    std::cout << message << std::endl;
}

void Token::decrease_lifetime()
{
    this->lifetime--;
}
