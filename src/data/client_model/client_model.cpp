#include "client_model.hpp"

ClientModel::ClientModel()
{
}

ClientModel::ClientModel(
    std::unordered_map<std::string, access_token_t> user_to_access_token)
{
    this->user_to_access_token = user_to_access_token;
}

ClientModel::~ClientModel()
{
}