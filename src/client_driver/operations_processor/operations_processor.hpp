#include "../operations_parser/operations_parser.hpp"
#include "../operations/operation.hpp"
#include "../../data/client_model/client_model.hpp"
#include "oauth.h"

#define LOCALHOST "127.0.0.1"

// OperationProcessor class which is responsible for processing the operations
class OperationProcessor
{
public:
    OperationProcessor();
    OperationProcessor(std::vector<Operation *> operations);
    ~OperationProcessor();
    void process_operations();
    ClientModel get_client_model();
    access_token_request_t handle_authorization_request(Operation *operation, CLIENT *client);
    bool has_valid_access_token(const std::string &user_id);
    void log(std::string message, int level);
    void process_command(Operation *operation, CLIENT *client);
    void refresh_token_if_needed(const std::string &user_id, CLIENT *client);
    int logResponse(char *error, int level);
    int log_response(const char *response);
    char *str_to_char(const std::string &str);
    char **validate_action_request(Operation *operation, const std::string &access_token_str, CLIENT *client);

private:
    std::vector<Operation *> operations;
    ClientModel client_model;
};