#include "../operations_parser/operations_parser.hpp"
#include "../operations/operation.hpp"
#include "../../data/client_model/client_model.hpp"
#include "oauth.h"

#define LOCALHOST "127.0.0.1"

class OperationProcessor
{
public:
    OperationProcessor();
    OperationProcessor(std::vector<Operation *> operations);
    ~OperationProcessor();
    void process_operations();
    access_token_request_t process_request(Operation *operation, CLIENT *client);
    void log(std::string message, int level);
    int logError(char *error, int level);
    ClientModel get_client_model();
    void process_command(Operation *operation, CLIENT *client);

private:
    std::vector<Operation *> operations;
    ClientModel client_model;
};