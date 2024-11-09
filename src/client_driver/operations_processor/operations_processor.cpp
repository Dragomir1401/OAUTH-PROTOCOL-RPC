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

void OperationProcessor::process_command(Operation *operation, CLIENT *client)
{
    // check if the user has the access token
    std::unordered_map<std::string, access_token_t> user_to_access_token = client_model.get_user_to_access_token();
    std::string user_id = operation->get_user_id();

    if (user_to_access_token.find(user_id) == user_to_access_token.end())
    {
        log("User " + user_id + " does not have an access token");
    }
    else
    {
        log("User " + user_id + " has an access token");

        // if the access token is expired
        if (user_to_access_token[user_id].expiration <= 0)
        {
            log("Access token expired.");

            // if auto refresh is enabled
            if (user_to_access_token[user_id].refresh_token != "")
            {
                log("Auto refresh enabled. Refreshing token");

                // refresh the access token
                access_token_t *result = refresh_access_1(&user_to_access_token[user_id], client);

                if (result == (access_token_t *)NULL)
                {
                    clnt_perror(client, "call failed");
                }

                log("Access token refreshed: " + std::string(result->access_token) + " with refresh token: " + std::string(result->refresh_token) + " and expiration: " + std::to_string(result->expiration));

                // update the access token in the client model
                client_model.add_access_token(user_id, *result);
            }
            else
            {
                log("Auto refresh not enabled. Not sending the request");
            }
        }
        else
        {
            log("Access token not expired. Using it");

            // convert the fields from the operation to char *
            char *operation_type = strdup(operation->get_action().c_str());
            char *resource = strdup(operation->get_resource().c_str());
            std::string access_token_str = user_to_access_token[user_id].access_token;
            char *access_token = strdup(access_token_str.c_str());

            // create a delegated action request
            delegated_action_request_t delegated_action_request;
            delegated_action_request.operation_type = operation_type;
            delegated_action_request.resource = resource;
            delegated_action_request.access_token = access_token;

            // request to validate the delegated action
            char **result = validate_delegated_action_1(&delegated_action_request, client);

            if (result == (char **)NULL)
            {
                clnt_perror(client, "call failed");
            }

            log("Delegated action validation result: " + std::string(*result));
        }
    }
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
        else if (operation->is_modify() || operation->is_read() || operation->is_delete() || operation->is_insert())
        {
            // check if the user has the access token
            std::unordered_map<std::string, access_token_t> user_to_access_token = client_model.get_user_to_access_token();
            std::string user_id = operation->get_user_id();

            if (user_to_access_token.find(user_id) == user_to_access_token.end())
            {
                log("User " + user_id + " does not have an access token");
            }
            else
            {
                log("User " + user_id + " has an access token");
                process_command(operation, client);
            }
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

ClientModel OperationProcessor::get_client_model()
{
    return this->client_model;
}

void OperationProcessor::log(std::string message)
{
    std::cout << message << std::endl;
}