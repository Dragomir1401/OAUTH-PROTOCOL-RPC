#include "server_model/server_model.hpp"

class ServerModelSingleton
{
public:
    ServerModelSingleton();
    static ServerModel *get_instance();
    ~ServerModelSingleton();

private:
    static ServerModel *instance;
};