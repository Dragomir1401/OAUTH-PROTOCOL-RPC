#include "../client_model/client_model.hpp"

class ClientModelSingleton
{
public:
    ClientModelSingleton();
    ~ClientModelSingleton();
    static ClientModel *get_instance();

private:
    static ClientModel *instance;
};