#pragma once
#include <string>

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
    bool is_delete();
    bool is_read();
    std::string to_string();

private:
    std::string user_id;
    std::string action;
    std::string resource;
    int auto_refresh;
};