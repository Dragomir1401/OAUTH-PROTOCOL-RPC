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
    int logResponse(char *error, int level);
    ClientModel get_client_model();
    void process_command(Operation *operation, CLIENT *client);
    char *str_to_char(const std::string &str);
    int log_response(const char *response);
    bool has_valid_access_token(const std::string &user_id);
    void refresh_token_if_needed(const std::string &user_id, CLIENT *client);
    char **validate_action_request(Operation *operation, const std::string &access_token_str, CLIENT *client);
    access_token_request_t handle_authorization_request(Operation *operation, CLIENT *client);

private:
    std::vector<Operation *> operations;
    ClientModel client_model;
};