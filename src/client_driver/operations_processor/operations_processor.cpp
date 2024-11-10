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

// Helper to check if a user has a valid access token
bool OperationProcessor::has_valid_access_token(const std::string &user_id)
{
    return client_model.get_user_to_access_token().find(user_id) != client_model.get_user_to_access_token().end();
}

// Refresh the token if it is expired and auto-refresh is enabled
void OperationProcessor::refresh_token_if_needed(const std::string &user_id, CLIENT *client)
{
    if (client_model.get_user_to_access_token()[user_id].expiration <= 0)
    {
        log("Access token expired.", 1);

        if (std::string(client_model.get_user_to_access_token()[user_id].refresh_token) != "NULL")
        {
            log("Auto refresh enabled. Refreshing token", 1);

            access_token_t *result = refresh_access_1(&client_model.get_user_to_access_token()[user_id], client);
            if (result != nullptr)
            {
                log("Access token refreshed: " + std::string(result->access_token) + " with refresh token: " +
                        std::string(result->refresh_token) + " and expiration: " + std::to_string(result->expiration),
                    1);
                client_model.add_access_token(user_id, *result);
            }
            else
            {
                clnt_perror(client, "call failed");
            }
        }
        else
        {
            log("Auto refresh not enabled. Not sending the request", 1);
        }
    }
}

// Request validation of a delegated action
char **OperationProcessor::validate_action_request(Operation *operation, const std::string &access_token_str, CLIENT *client)
{
    delegated_action_request_t request;
    request.operation_type = strdup(operation->get_action().c_str());
    request.resource = strdup(operation->get_resource().c_str());
    request.access_token = strdup(access_token_str.c_str());

    char **result = validate_delegated_action_1(&request, client);
    if (result == nullptr)
    {
        clnt_perror(client, "call failed");
    }
    return result;
}

// Request and handle authorization for a user
access_token_request_t OperationProcessor::handle_authorization_request(Operation *operation, CLIENT *client)
{
    std::string user_id_str = operation->get_user_id();
    char *user_id = strdup(user_id_str.c_str());

    // Request to get authorization token from the server
    char **result_auth = request_authorization_1(&user_id, client);
    if (result_auth == nullptr)
    {
        clnt_perror(client, "call failed");
    }
    log("User with id " + user_id_str + " found with authorization token: " + std::string(*result_auth), 1);

    request_authorization_t auth_request;
    auth_request.user_id = user_id;
    auth_request.authentification_token = *result_auth;
    auth_request.refresh_token = operation->is_auto_refresh();

    char **result = approve_request_token_1(&auth_request, client);
    if (result == nullptr)
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

// Log response for validation result and return status code
int OperationProcessor::log_response(const char *response)
{
    return logResponse(const_cast<char *>(response), 0);
}

// Convert std::string to strdup(char*)
char *OperationProcessor::str_to_char(const std::string &str)
{
    return strdup(str.c_str());
}

void OperationProcessor::process_command(Operation *operation, CLIENT *client)
{
    std::string user_id = operation->get_user_id();

    if (!has_valid_access_token(user_id))
    {
        log("User " + user_id + " does not have an access token", 1);
    }
    else
    {
        refresh_token_if_needed(user_id, client);
    }

    std::string access_token_str = has_valid_access_token(user_id) ? client_model.get_user_to_access_token()[user_id].access_token : "";

    char **result = validate_action_request(operation, access_token_str, client);

    if (result != nullptr)
    {
        log(std::string(*result), 0);
        client_model.decrease_lifetime(user_id);
        log("New lifetime of the token: " + std::to_string(client_model.get_user_to_access_token()[user_id].expiration), 1);
        log("Delegated action validation result: " + std::string(*result), 1);
    }
}

void OperationProcessor::process_operations()
{
    CLIENT *client = clnt_create(LOCALHOST, OAUTH_PROTOCOL, OAUTH_VERSION, "udp");
    if (client == nullptr)
    {
        clnt_pcreateerror("localhost");
        exit(1);
    }

    for (Operation *operation : operations)
    {
        log(operation->to_string(), 1);

        if (operation->is_request())
        {
            access_token_request_t access_token_request = handle_authorization_request(operation, client);

            access_token_t *result = request_access_token_1(&access_token_request, client);
            if (result == nullptr)
            {
                clnt_perror(client, "call failed");
                continue;
            }

            log("Access token received: " + std::string(result->access_token) + " with refresh token: " +
                    std::string(result->refresh_token) + " and expiration: " + std::to_string(result->expiration),
                1);

            if (log_response(result->access_token) == -1)
            {
                log(std::string(access_token_request.authentification_token) + " -> " +
                        std::string(result->access_token) +
                        (strcmp(result->refresh_token, "NULL") ? "," + std::string(result->refresh_token) : ""),
                    0);
                client_model.add_access_token(access_token_request.user_id, *result);
            }
        }
        else
        {
            std::string user_id = operation->get_user_id();
            if (!has_valid_access_token(user_id))
            {
                log("User " + user_id + " does not have an access token", 1);
            }
            process_command(operation, client);
        }
    }
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