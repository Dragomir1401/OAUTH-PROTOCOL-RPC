#include "../operations_parser/operations_parser.hpp"
#include "../operations/operation.hpp"
#include "oauth.h"

#define LOCALHOST "127.0.0.1"

class OperationProcessor
{
public:
    OperationProcessor();
    OperationProcessor(std::vector<Operation *> operations);
    ~OperationProcessor();
    void process_operations();

private:
    std::vector<Operation *> operations;
};