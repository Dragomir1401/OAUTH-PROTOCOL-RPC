#include "operations_processor.hpp"
#include "../../utils/res_codes.hpp"
#include <iostream>

OperationProcessor::OperationProcessor()
{
}

OperationProcessor::OperationProcessor(std::vector<Operation *> operations)
{
    this->operations = operations;
}

OperationProcessor::~OperationProcessor()
{
}

void OperationProcessor::process_operations()
{
    CLIENT *client;

    client = clnt_create(LOCALHOST, OAUTH_PROTOCOL, OAUTH_VERSION, "udp");
    if (client == NULL)
    {
        clnt_pcreateerror("localhost");
        exit(1);
    }

    for (Operation *operation : operations)
    {
        // log operation
        log(operation->to_string());

        if (operation->is_request())
        {
            access_token_request_t access_token_request = process_request(operation, client);
        }
        else if (operation->is_modify())
        {
            // modify access token
        }
        else if (operation->is_insert())
        {
            // insert access token
        }
        else
        {
        }
    }
}

access_token_request_t OperationProcessor::process_request(Operation *operation, CLIENT *client)
{
    // transform user id from string to char *
    char *user_id = (char *)operation->get_user_id().c_str();

    // request to get authentification token from the server
    char **result_auth = request_authorization_1(&user_id, client);

    if (strcmp(*result_auth, ResponseCodes::getString(ResponseCodes::USER_NOT_FOUND).c_str()) == 0)
    {
        log("User not found");
    }
    else
    {
        log("User found with authentification token: " + std::string(*result_auth));
    }

    access_token_request_t access_token_request;
    access_token_request.user_id = user_id;
    access_token_request.authentification_token = *result_auth;
    access_token_request.auto_refresh = operation->is_auto_refresh();

    return access_token_request;
}

void OperationProcessor::log(std::string message)
{
    std::cout << message << std::endl;
}