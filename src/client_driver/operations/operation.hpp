#pragma once
#include <string>

// Type of operations
#define REQUEST "REQUEST"
#define MODIFY "MODIFY"
#define INSERT "INSERT"
#define DELETE "DELETE"
#define READ "READ"
#define NO_RESOURCE "NO_RESOURCE"
#define EXECUTE "EXECUTE"
#define ON 1
#define OFF 0
#define NOT_APPLICABLE -1

// Operation class which hold the information any type of operation
class Operation
{
public:
    Operation(std::string user_id, std::string action, std::string resource, int auto_refresh);
    ~Operation();
    std::string get_user_id();
    std::string get_action();
    std::string get_resource();
    int is_auto_refresh();
    int get_type();
    std::string to_string();

private:
    std::string user_id;
    std::string action;
    std::string resource;
    int auto_refresh;
};