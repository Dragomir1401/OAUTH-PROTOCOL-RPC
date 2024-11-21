# SCD 2024-2025 - Tema 1 - Client/Server Authorization using OAuth and RPC

### Student: Dragomir Andrei-Mihai

## Project Overview

This project implements a client-server system that simulates an OAuth-based authorization process using Remote Procedure Calls (RPC). The solution authorizes RIMDX (Read, Insert, Modify, Delete, Execute) operations on a server's file system manager.

### Contributors

- **Student:** Dragomir Andrei-Mihai

## ⚠️ **IMPORTANT NOTE** ⚠️

**DO NOT** regenerate the RPC files using `rpcgen`. These files were **manually modified** to fit the specific requirements of this project, and regenerating them will override these custom changes.

**MAKE SURE** you have RPC running on your machine.
Run `sudo systemctl status rpcbind` - make sure it is active (running)

---

## Objectives

1. Develop a client-server application for resource authorization.
2. Implement OAuth-based authorization using RPC for secure RIMDX operations.
3. Utilize data structures to manage users, permissions, and tokens.

## Architecture

The system includes:

- **Authorization Server:** Manages user authorization and access tokens.
- **Resource Server:** Validates and executes client-requested operations on resources.
- **Client Application:** Requests access tokens and performs file operations on the server.

## Setup Instructions

1. **Requirements:** C/C++ compiler, `rpcgen`.
2. **Build & Run:**
   - Build the project: `make`
   - Start server: `./server <client_file> <resource_file> <approvals_file> <token_validity>`
   - Start client: `./client <server_address> <operations_file>`
3. **Testing:** Run provided tests with `check.sh`.

## Files & Directory Structure

- **Source Code:** `/src`
- **Tests:** `/tests`
- **Build Scripts:** `Makefile`, `check.sh`

## Key Components and Functionalities

- **Token Management**: The server generates, validates, and refreshes access tokens using a randomized token generator.

  - `generate_and_log_token()`: Generates an access token for users and logs the token details.
  - `refresh_access_1_svc()`: Handles token refresh requests, generating new tokens when the previous ones expire.

- **Authorization and Resource Access**: Implements RIMDX operations through OAuth-based authorization.

  - `request_authorization_1_svc()`: Validates user IDs and generates tokens for access authorization.
  - `validate_delegated_action_1_svc()`: Validates client operations by checking permissions and resource existence.
  - `approve_request_token_1_svc()`: Approves access tokens based on user-specified permissions.

- **Error and Response Codes**: Defined in `res_codes.hpp` and used throughout to standardize server responses.

  - Common responses include `USER_NOT_FOUND`, `PERMISSION_DENIED`, and `TOKEN_EXPIRED`.

- **Logging**: Logs client and server events for debugging and monitoring purposes.
  - `log()`: Custom logging function with severity levels to log key events and errors to designated log files.

## Directory Structure

- `client_driver`: Manages client-side requests and processing.
  - `operation.hpp`, `operations_parser.hpp`, `operations_processor.hpp`: Defines client-side operations, parsing, and processing.
- `data`: Contains shared data structures like `server_model` and `client_model` for managing token, user, and resource data.
- `rpc`: Contains RPC-generated files (`oauth.h`, `oauth.x`).
- `utils`: Utility files, including `token.h` for token generation and `res_codes.hpp` for standardized response codes.

## Running and Testing

1. **Compile**: Use `make` to build the client and server executables.
2. **Run Server**: `./server <users_file> <resources_file> <approvals_file> <token_lifetime>`
3. **Run Client**: `./client <server_address> <operation_file>`
4. **Logging**: Log files are created in the root directory for monitoring client and server operations.

## Additional Notes

- **Error Handling**: The server logs any unauthorized or invalid access attempts for debugging.
- **Token Expiry**: Tokens expire based on the specified `token_lifetime` parameter, with optional automatic refresh.

## Note

The stubs for client and server were modified as following:

- The oauth_svc.c was modified to parse input data for the server.
- The oauth_clnt.c was mnot modified.
- The oauth_client.cpp as an extended stub was modified to instantiate the OperationsParser and OperationsProcessor classes.
- The oauth_server.cpp as an extended stub was modified to instantiate the ServerModel class and all the server logic was implemented in this file.

## Resources

- **RPC Documentation**:  
  Learn more about Remote Procedure Call (RPC) and its usage in distributed systems:  
  [RPC Programming Guide](https://docs.oracle.com/cd/E36784_01/html/E36874/rpcgen-1.html)

- **OAuth 2.0 Specification**:  
  Detailed information on the OAuth 2.0 authorization framework used in this project:  
  [OAuth 2.0 Framework](https://tools.ietf.org/html/rfc6749)

- **Logging Best Practices**:  
  Guidance on setting up effective logging for debugging and monitoring client-server applications:  
  [Logging in C++](https://www.fluentd.org/guides/overview/logging-cpp)

- **C++ Standard Library Documentation**:  
  Useful reference for various C++ standard library functions and classes used in this project:  
  [C++ Reference](https://en.cppreference.com/w/)
