#include "operations_processor.hpp"
#include "../../utils/res_codes.hpp"
#include <iostream>
#include <fstream>

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
    std::string user_id = operation->get_user_id();
    std::string access_token_builder = "";

    if (client_model.get_user_to_access_token().find(user_id) == client_model.get_user_to_access_token().end())
    {
        log("User " + user_id + " does not have an access token", 1);
    }
    else if (client_model.get_user_to_access_token()[user_id].expiration <= 0)
    {
        log("Access token expired.", 1);

        // if auto refresh is enabled
        log("Comparing " + std::string(client_model.get_user_to_access_token()[user_id].refresh_token) + " with NULL", 1);
        if (strcmp(client_model.get_user_to_access_token()[user_id].refresh_token, "NULL") != 0)
        {
            log("Auto refresh enabled. Refreshing token", 1);

            // refresh the access token
            access_token_t *result = refresh_access_1(&client_model.get_user_to_access_token()[user_id], client);

            if (result == (access_token_t *)NULL)
            {
                clnt_perror(client, "call failed");
            }

            log("Access token refreshed: " + std::string(result->access_token) + " with refresh token: " + std::string(result->refresh_token) + " and expiration: " + std::to_string(result->expiration), 1);

            // update the access token in the client model
            client_model.add_access_token(user_id, *result);
        }
        else
        {
            log("Auto refresh not enabled. Not sending the request", 1);
        }
    }

    // convert the fields from the operation to char *
    char *operation_type = strdup(operation->get_action().c_str());
    char *resource = strdup(operation->get_resource().c_str());
    if (client_model.get_user_to_access_token().find(user_id) != client_model.get_user_to_access_token().end())
    {
        access_token_builder = client_model.get_user_to_access_token()[user_id].access_token;
    }

    std::string access_token_str = access_token_builder;
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

    // log on stream 0 the result of the delegated action validation
    log(std::string(*result), 0);

    // decrease the lifetime of the token in the user_to_access_token
    if (client_model.get_user_to_access_token().find(user_id) != client_model.get_user_to_access_token().end())
    {
        client_model.decrease_lifetime(user_id);
        // log the new lifetime of the token
        log("New lifetime of the token: " + std::to_string(client_model.get_user_to_access_token()[user_id].expiration), 1);
    }

    log("Delegated action validation result: " + std::string(*result), 1);
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
        log(operation->to_string(), 1);

        if (operation->is_request())
        {
            access_token_request_t access_token_request = process_request(operation, client);

            // request access token
            access_token_t *result = request_access_token_1(&access_token_request, client);

            if (result == (access_token_t *)NULL)
            {
                clnt_perror(client, "call failed");
            }

            log("Access token received: " + std::string(result->access_token) + " with refresh token: " + std::string(result->refresh_token) + " and expiration: " + std::to_string(result->expiration), 1);

            // if request is denied print it
            if (logResponse(result->access_token, 0) == -1)
            {
                // else success
                if (!strcmp(result->refresh_token, "NULL"))
                {
                    log(std::string(access_token_request.authentification_token) + " -> " + std::string(result->access_token), 0);
                }
                else
                {
                    log(std::string(access_token_request.authentification_token) + " -> " + std::string(result->access_token) + "," + std::string(result->refresh_token), 0);
                }
                // add the access token in the client model
                client_model.add_access_token(access_token_request.user_id, *result);
            }
        }
        else if (operation->is_modify() || operation->is_read() || operation->is_delete() || operation->is_insert() || operation->is_execute())
        {
            // check if the user has the access token
            std::unordered_map<std::string, access_token_t> user_to_access_token = client_model.get_user_to_access_token();
            std::string user_id = operation->get_user_id();

            if (user_to_access_token.find(user_id) == user_to_access_token.end())
            {
                log("User " + user_id + " does not have an access token", 1);
            }

            process_command(operation, client);
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
        log("User not found", 1);
    }
    else
    {
        log("User with id " + operation->get_user_id() + " found with authentification token: " + std::string(*result_auth), 1);
    }

    request_authorization_t request_authorization_v;
    request_authorization_v.user_id = user_id;
    request_authorization_v.authentification_token = *result_auth;
    request_authorization_v.refresh_token = operation->is_auto_refresh();

    log("Request signature for token: " + std::string(request_authorization_v.authentification_token) + " for user " + request_authorization_v.user_id, 1);

    // this call will attach the token with the approvals in the server
    char **result = approve_request_token_1(&request_authorization_v, client);

    if (result == (char **)NULL)
    {
        clnt_perror(client, "call failed");
    }

    log("Approve request token result: " + std::string(*result), 1);

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

void OperationProcessor::log(std::string message, int level)
{
    if (level == 1)
    {
        std::ofstream log_file("client_global_logging_file.txt", std::ios_base::app);
        log_file << message << std::endl;
    }
    else if (level == 2)
    {
        std::ofstream log_file("server_global_logging_file.txt", std::ios_base::app);
        log_file << message << std::endl;
    }
    else
    {
        std::cout << message << std::endl;
    }
}

int OperationProcessor::logResponse(char *error, int level)
{
    // if request is denied print it
    if (strcmp(error, ResponseCodes::getString(ResponseCodes::REQUEST_DENIED).c_str()) == 0)
    {
        log("REQUEST_DENIED", 0);
        return 0;
    }
    else if (strcmp(error, ResponseCodes::getString(ResponseCodes::USER_NOT_FOUND).c_str()) == 0)
    {
        log("USER_NOT_FOUND", 0);
        return 0;
    }
    else if (strcmp(error, ResponseCodes::getString(ResponseCodes::PERMISSION_DENIED).c_str()) == 0)
    {
        log("PERMISSION_DENIED", 0);
        return 0;
    }
    else if (strcmp(error, ResponseCodes::getString(ResponseCodes::PERMISSION_GRANTED).c_str()) == 0)
    {
        log("PERMISSION_GRANTED", 0);
        return 0;
    }
    else if (strcmp(error, ResponseCodes::getString(ResponseCodes::RESOURCE_NOT_FOUND).c_str()) == 0)
    {
        log("RESOURCE_NOT_FOUND", 0);
        return 0;
    }
    else if (strcmp(error, ResponseCodes::getString(ResponseCodes::OPERATION_NOT_PERMITTED).c_str()) == 0)
    {
        log("OPERATION_NOT_PERMITTED", 0);
        return 0;
    }

    return -1;
}