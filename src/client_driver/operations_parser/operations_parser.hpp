#pragma once
#include <vector>
#include "../operations/operation.hpp"

// OperationsParser class which is responsible for parsing the operations from the input file
class OperationsParser
{
public:
    OperationsParser();
    OperationsParser(char *input_file);
    ~OperationsParser();
    std::vector<Operation *> parse_operations();
    void set_input_file(char *input_file);
    void log(std::string message, int level);

private:
    char *input_file;
};