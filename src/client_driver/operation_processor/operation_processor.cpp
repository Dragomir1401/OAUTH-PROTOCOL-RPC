#include "operation_processor.hpp"

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
    for (Operation *operation : operations)
    {
        if (operation->is_request())
        {
            // request access token
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