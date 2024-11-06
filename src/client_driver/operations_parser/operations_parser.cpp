#include "operations_parser.hpp"
#include "../operations/operation.hpp"
#include <iostream>
#include <fstream>

OperationsParser::OperationsParser(char *input_file)
{
    this->input_file = input_file;
}

OperationsParser::~OperationsParser()
{
}

std::vector<Operation *> OperationsParser::parse_operations()
{
    std::vector<Operation *> operations;

    std::ifstream file(input_file);
    std::string line;
    while (std::getline(file, line))
    {
        // extract user_id, action, resource/auto_refresh
        std::string user_id = line.substr(0, line.find(','));
        line = line.substr(line.find(',') + 1);
        std::string action = line.substr(0, line.find(','));

        int auto_refresh = 0;
        std::string resource = "";

        if (action == REQUEST)
        {
            // rest of the line is the auto_refresh
            line = line.substr(line.find(',') + 1);
            auto_refresh = std::stoi(line);
        }
        else if (action == MODIFY || action == INSERT)
        {
            // rest of the line is the resource
            line = line.substr(line.find(',') + 1);
            resource = line;
        }
        else
        {
            std::cout << "Invalid action" << std::endl;
        }

        Operation *operation = new Operation(user_id, action, resource, auto_refresh);
        operations.push_back(operation);
    }

    return operations;
}

void OperationsParser::set_input_file(char *input_file)
{
    this->input_file = input_file;
}