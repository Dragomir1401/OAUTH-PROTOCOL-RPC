#include "client_model.hpp"

ClientModel::ClientModel()
{
    this->user_to_access_token = std::unordered_map<std::string, access_token_t>();
}

ClientModel::ClientModel(
    std::unordered_map<std::string, access_token_t> user_to_access_token)
{
    this->user_to_access_token = user_to_access_token;
}

ClientModel::~ClientModel()
{
}

void ClientModel::add_access_token(std::string user_id, access_token_t access_token)
{
    log("Adding access token for user: " + user_id);
    this->user_to_access_token[user_id] = access_token;
}

void ClientModel::remove_access_token(std::string user_id)
{
    log("Removing access token for user: " + user_id);
    this->user_to_access_token.erase(user_id);
}

void ClientModel::log(std::string log)
{
    std::cout << log << std::endl;
}