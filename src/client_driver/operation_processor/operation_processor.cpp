#include "operation_processor.hpp"
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

    client = clnt_create(LOCALHOST, OAUTH_PROTOCOL, OAUTH_VERSION, "tcp");
    if (client == NULL)
    {
        clnt_pcreateerror("localhost");
        exit(1);
    }

    for (Operation *operation : operations)
    {
        if (operation->is_request())
        {
            char *user_id = new char[operation->get_user_id().length() + 1];
            strcpy(user_id, operation->get_user_id().c_str());

            char **rc = attemp_auth_1(&user_id, client);
            if (rc == NULL)
            {
                clnt_perror(client, "call failed");
            }

            if (ResponseCodes::compare(ResponseCodes::USER_NOT_FOUND, *rc))
            {
                std::cout << "User not found" << std::endl;
            }
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