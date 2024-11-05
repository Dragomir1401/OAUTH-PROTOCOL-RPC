#include "server_instance.hpp"

ServerModelSingleton::ServerModelSingleton()
{
    instance = nullptr;
}

ServerModel *ServerModelSingleton::get_instance()
{
    if (instance == nullptr)
    {
        instance = new ServerModel();
    }
    return instance;
}

ServerModelSingleton::~ServerModelSingleton()
{
    delete instance;
}