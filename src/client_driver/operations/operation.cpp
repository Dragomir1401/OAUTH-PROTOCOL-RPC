#include "operation.hpp"

Operation::Operation(std::string user_id, std::string action, std::string resource, int auto_refresh)
{
    this->user_id = user_id;
    this->action = action;
    this->resource = resource;
    this->auto_refresh = auto_refresh;
}

Operation::~Operation()
{
}

std::string Operation::get_user_id()
{
    return user_id;
}

std::string Operation::get_action()
{
    return action;
}

std::string Operation::get_resource()
{
    return resource;
}

int Operation::is_auto_refresh()
{
    return auto_refresh;
}

bool Operation::is_request()
{
    return action == REQUEST;
}

bool Operation::is_modify()
{
    return action == MODIFY;
}

bool Operation::is_insert()
{
    return action == INSERT;
}

std::string Operation::to_string()
{
    return "User: " + user_id + " Action: " + action + " Resource: " + resource + " Auto Refresh: " + std::to_string(auto_refresh);
}