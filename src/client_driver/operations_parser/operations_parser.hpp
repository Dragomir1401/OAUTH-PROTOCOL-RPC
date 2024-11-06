#pragma once
#include <vector>

class OperationsParser
{
public:
    OperationsParser();
    OperationsParser(char *input_file);
    ~OperationsParser();
    std::vector<Operation *> parse_operations();
    void set_input_file(char *input_file);

private:
    char *input_file;
};