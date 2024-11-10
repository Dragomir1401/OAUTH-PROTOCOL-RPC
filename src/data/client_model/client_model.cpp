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
    // check if the user already has an access token, then update it and log
    if (this->user_to_access_token.find(user_id) != this->user_to_access_token.end())
    {
        log("Updating access token for user: " + user_id, 1);
    }
    else
    {
        log("Adding access token for user: " + user_id, 1);
    }

    this->user_to_access_token[user_id] = access_token;
}

void ClientModel::remove_access_token(std::string user_id)
{
    log("Removing access token for user: " + user_id, 1);
    this->user_to_access_token.erase(user_id);
}

std::unordered_map<std::string, access_token_t> ClientModel::get_user_to_access_token()
{
    return this->user_to_access_token;
}

void ClientModel::log(std::string message, int level)
{
    if (level == 1)
    {
        std::ofstream log_file("client_global_logging_file.txt", std::ios_base::app);
        log_file << message << std::endl;
    }
    else if (level == 2)
    {
        std::ofstream log_file("server_global_logging_file.txt", std::ios_base::app);
        log_file << message << std::endl;
    }
    else
    {
        std::cout << message << std::endl;
    }
}

void ClientModel::decrease_lifetime(std::string user_id)
{
    if (this->user_to_access_token.find(user_id) != this->user_to_access_token.end())
    {
        this->user_to_access_token[user_id].expiration--;
    }
}