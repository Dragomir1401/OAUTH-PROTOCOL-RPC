#include "operations_processor.hpp"
#include "../../utils/res_codes.hpp"
#include <iostream>

OperationProcessor::OperationProcessor()
{
}

OperationProcessor::OperationProcessor(std::vector<Operation *> operations)
{
    this->operations = operations;
    this->client_model = ClientModel();
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

            // request access token
            access_token_t *result = request_access_token_1(&access_token_request, client);

            if (result == (access_token_t *)NULL)
            {
                clnt_perror(client, "call failed");
            }

            log("Access token received: " + std::string(result->access_token) + " with refresh token: " + std::string(result->refresh_token) + " and expiration: " + std::to_string(result->expiration));

            // add the access token in the client model
            client_model.add_access_token(access_token_request.user_id, *result);
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
    std::string user_id_str = operation->get_user_id();
    char *user_id = strdup(user_id_str.c_str());

    // make sure user id ends with null terminator
    user_id[operation->get_user_id().size()] = '\0';

    // request to get authentification token from the server
    char **result_auth = request_authorization_1(&user_id, client);

    if (result_auth == (char **)NULL)
    {
        clnt_perror(client, "call failed");
    }

    if (strcmp(*result_auth, ResponseCodes::getString(ResponseCodes::USER_NOT_FOUND).c_str()) == 0)
    {
        log("User not found");
    }
    else
    {
        log("User with id " + operation->get_user_id() + " found with authentification token: " + std::string(*result_auth));
    }

    request_authorization_t request_authorization_v;
    request_authorization_v.user_id = user_id;
    request_authorization_v.authentification_token = *result_auth;
    request_authorization_v.refresh_token = operation->is_auto_refresh();

    log("Request signature for token: " + std::string(request_authorization_v.authentification_token) + " for user " + request_authorization_v.user_id);

    // this call will attach the token with the approvals in the server
    char **result = approve_request_token_1(&request_authorization_v, client);

    if (result == (char **)NULL)
    {
        clnt_perror(client, "call failed");
    }

    log("Approve request token result: " + std::string(*result));

    access_token_request_t access_token_request;
    access_token_request.user_id = user_id;
    access_token_request.authentification_token = *result;
    access_token_request.auto_refresh = operation->is_auto_refresh();

    return access_token_request;
}

void OperationProcessor::log(std::string message)
{
    std::cout << message << std::endl;
}