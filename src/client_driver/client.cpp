
#include "operations_parser/operations_parser.hpp"
#include "operations/operation.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Wrong. Correct usage: %s <client_addr> <operation_file>\n", argv[0]);
        exit(1);
    }

    char *input_file = argv[2];
    OperationsParser *operations_parser = new OperationsParser(input_file);
    std::vector<Operation *> operations = operations_parser->parse_operations();

    delete operations_parser;
    return 0;
}