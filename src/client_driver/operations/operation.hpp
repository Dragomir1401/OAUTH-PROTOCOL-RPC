#pragma once
#include <string>

#define REQUEST "REQUEST"
#define MODIFY "MODIFY"
#define INSERT "INSERT"

class Operation
{
public:
    Operation(std::string user_id, std::string action, std::string resource, int auto_refresh);
    ~Operation();
    std::string get_user_id();
    std::string get_action();
    std::string get_resource();
    int is_auto_refresh();
    bool is_request();
    bool is_modify();
    bool is_insert();

private:
    std::string user_id;
    std::string action;
    std::string resource;
    int auto_refresh;
};