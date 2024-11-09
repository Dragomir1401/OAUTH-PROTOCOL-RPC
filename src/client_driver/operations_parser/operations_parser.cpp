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

        int auto_refresh = NOT_APPLICABLE;
        std::string resource = NO_RESOURCE;

        if (action == REQUEST)
        {
            // rest of the line is the auto_refresh
            line = line.substr(line.find(',') + 1);
            auto_refresh = std::stoi(line);
        }
        else if (action == MODIFY || action == INSERT || action == DELETE || action == READ || action == EXECUTE)
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
        // add operation to operations list
        operations.push_back(operation);
    }

    return operations;
}

void OperationsParser::set_input_file(char *input_file)
{
    this->input_file = input_file;
}

void OperationsParser::log(std::string message, int level)
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