#include "client_instance.hpp"

ClientModelSingleton::ClientModelSingleton()
{
    instance = nullptr;
}

ClientModel *ClientModelSingleton::get_instance()
{
    if (instance == nullptr)
    {
        instance = new ClientModel();
    }
    return instance;
}

ClientModelSingleton::~ClientModelSingleton()
{
    delete instance;
}