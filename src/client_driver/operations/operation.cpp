#include "operation.hpp"

// Operation class which hold the information any type of operation
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

int Operation::get_type()
{
    if (action == REQUEST)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

std::string Operation::to_string()
{
    return "User: " + user_id + " Action: " + action + " Resource: " + resource + " Auto Refresh: " + std::to_string(auto_refresh);
}