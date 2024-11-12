/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "oauth.h"
#include "../../data/server_model/server_model.h"
#include "../client_driver/operations/operation.hpp"
#include "../utils/token.h"
#include "../utils/res_codes.hpp"
#include <iostream>
#include <algorithm>

// Simple helper to log messages to a file
void log_to_file(const std::string &message, const std::string &filename)
{
	std::ofstream log_file(filename, std::ios_base::app);
	log_file << message << std::endl;
}

// Level logger
void log(const std::string &message, int level = 0)
{
	// Level logger
	// 1 - low severity client logs
	// 2 - low severity server logs
	// 0 - high severity logs
	switch (level)
	{
	case 1:
		log_to_file(message, "client_global_logging_file.txt");
		break;
	case 2:
		log_to_file(message, "server_global_logging_file.txt");
		break;
	default:
		std::cout << message << std::endl;
		break;
	}
}

// Helper to check if a user exists in the user list
bool user_exists(const std::string &user_id)
{
	return std::find(user_list.begin(), user_list.end(), user_id) != user_list.end();
}

// Helper to generate and log a token
char *generate_and_log_token(const std::string &user_id)
{
	// Duplicate to get a non-const char*
	char *user_id_copy = strdup(user_id.c_str());
	char *token = generate_access_token(user_id_copy);

	// Free the duplicated string after use
	free(user_id_copy);

	// Log at low severity server logs
	log("Token generated: " + std::string(token), 2);
	return token;
}

// Endpoint to request authorization to resources
char **request_authorization_1_svc(char **argp, struct svc_req *rqstp)
{
	// Log header at low severity server logs
	log("========= REQUEST AUTHORIZATION =========", 2);

	static char *result;
	std::string user_id = argp[0];

	// Check if the user exists
	if (user_exists(user_id))
	{
		log("User found", 2);

		// Generate the token for the user and log it
		result = generate_and_log_token(user_id);
	}
	else
	{
		log("User not found", 2);

		// Return USER_NOT_FOUND if the user does not exist
		result = strdup(ResponseCodes::getString(ResponseCodes::USER_NOT_FOUND).c_str());
	}

	return &result;
}

// Helper to set an error response
void set_error_response(access_token_t &result, const std::string &code)
{
	// Use strdup to avoid memory problems
	result.access_token = strdup(code.c_str());
	result.refresh_token = strdup(code.c_str());
	result.expiration = -1;
}

// Helper to check if a token exists in the auth_token_list
bool token_exists(const std::string &token_str)
{
	// Check if the token exists in the auth_token_list
	return std::any_of(auth_token_list.begin(), auth_token_list.end(),
					   [&token_str](const Token &token)
					   {
						   return token.get_token() == token_str;
					   });
}

// Helper to get a signed token from the auth_token_list
Token *get_signed_token(const std::string &token_str)
{
	// Find the token in the auth_token_list
	for (Token &token : auth_token_list)
	{
		// Only if the token is signed
		if (token.get_token() == token_str && token.get_status() == Token::AUTH_SIGNED)
		{
			return &token;
		}
	}
	return nullptr;
}

// Helper to generate a refresh token from an access token
std::string generate_refresh_token(const std::string &access_token)
{
	// Duplicate to get a non-const char*
	char *access_token_copy = strdup(access_token.c_str());

	// Generate a refresh token from the access token
	std::string refresh_token = generate_access_token(access_token_copy);

	// Free the duplicated string
	free(access_token_copy);

	return refresh_token;
}

// Helper to add an access token to a user
void add_access_token_to_user(const std::string &user_id, const Token &access_token)
{
	log("Adding access token " + access_token.get_token() + " to user " + user_id, 2);
	user_to_access_token[user_id] = access_token;
}

// Endpoint to approve a request token
access_token_t *request_access_token_1_svc(access_token_request_t *argp, struct svc_req *rqstp)
{
	// Log header at low severity server logs
	log("========= REQUEST ACCESS TOKEN =========", 2);

	static access_token_t result;

	// Parse the arguments
	std::string user_id = argp->user_id;
	std::string token_received = argp->authentification_token;
	int auto_refresh = argp->auto_refresh;

	// Log the request at low severity server logs
	log("BEGIN " + user_id + " AUTHZ", 0);

	// If the user does not exist, we can stop here
	if (!user_exists(user_id))
	{
		log("User not found. Returning USER_NOT_FOUND", 2);
		set_error_response(result, ResponseCodes::getString(ResponseCodes::USER_NOT_FOUND));
		return &result;
	}

	// Check if the token exists
	log("  RequestToken = " + token_received, 0);

	// Check if the token exists, if not then return REQUEST_DENIED
	Token *token = get_signed_token(token_received);
	if (!token)
	{
		log("Token not found or not signed. Returning REQUEST_DENIED", 2);
		set_error_response(result, ResponseCodes::getString(ResponseCodes::REQUEST_DENIED));
		return &result;
	}

	// Log the token found and signed
	log("Token found and signed. Generating access token", 2);

	// Convert `token_received` to `char*` for `generate_access_token`
	char *token_received_copy = strdup(token_received.c_str());
	std::string access_token_str = generate_access_token(token_received_copy);

	// Free the copy after use
	free(token_received_copy);

	// Create the access token and add the approvals
	Token access_token(access_token_str, user_id, global_token_lifetime, Token::ACCESS);
	access_token.add_approvals(token->get_approvals());

	if (auto_refresh)
	{
		// If the request has auto-refresh enabled, generate a refresh token
		log("Auto-refresh enabled. Generating refresh token", 2);
		std::string refresh_token = generate_refresh_token(access_token_str);
		access_token.add_refresh_token(refresh_token);
		result.refresh_token = strdup(refresh_token.c_str());
	}
	else
	{
		// No refresh token if auto-refresh is disabled
		result.refresh_token = strdup("NULL");
	}

	add_access_token_to_user(user_id, access_token);

	// Check approvals for permission
	auto approvals = access_token.get_approvals();
	if (approvals.empty() || (approvals.find("*") != approvals.end() && approvals["*"] == "-"))
	{
		log("No approvals found. Returning REQUEST_DENIED", 2);
		set_error_response(result, ResponseCodes::getString(ResponseCodes::REQUEST_DENIED));
		return &result;
	}

	// Set access token response
	result.access_token = strdup(access_token_str.c_str());
	result.expiration = global_token_lifetime;

	// Log high severity server logs
	log("  AccessToken = " + access_token_str, 0);
	if (auto_refresh)
		log("  RefreshToken = " + std::string(result.refresh_token), 0);

	return &result;
}

// Helper to log validation result at level 0, with optional handling for empty or expired tokens
void log_validation_result(const std::string &status, const std::string &operation_type,
						   const std::string &resource, const std::string &access_token_str, int validity, bool omit_token = false)
{
	// Log the validation result
	std::string log_message = status + " (" + operation_type + "," + resource + ",";

	// Omit the token if requested
	log_message += omit_token ? "" : access_token_str;
	log_message += "," + std::to_string(validity) + ")";

	log(log_message, 0);
}

// Helper to check if the access token is empty
bool is_empty_token(const std::string &access_token_str, const std::string &operation_type, const std::string &resource, char *&result)
{
	if (access_token_str.empty())
	{
		log("Access token is empty. Returning PERMISSION_DENIED", 2);
		log_validation_result("DENY", operation_type, resource, access_token_str, 0);
		result = strdup(ResponseCodes::getString(ResponseCodes::PERMISSION_DENIED).c_str());
		return true;
	}
	return false;
}

// Helper to find the token in user_to_access_token map
bool find_access_token(const std::string &access_token_str, Token &access_token, std::string &user_id)
{
	for (const auto &user_access_token : user_to_access_token)
	{
		// If the token is found, fill the access_token and user_id
		if (user_access_token.second.get_token() == access_token_str)
		{
			access_token = user_access_token.second;
			user_id = user_access_token.first;
			return true;
		}
	}
	return false;
}

// Helper to validate operation type
bool is_valid_operation(const std::string &operation_type)
{
	return operation_type == REQUEST || operation_type == MODIFY || operation_type == INSERT ||
		   operation_type == DELETE || operation_type == READ || operation_type == NO_RESOURCE ||
		   operation_type == EXECUTE;
}

// Helper to check if the resource exists
bool is_valid_resource(const std::string &resource)
{
	return std::find(resource_list.begin(), resource_list.end(), resource) != resource_list.end();
}

// Helper to check if the user has the required approval
bool has_required_approval(const Token &access_token, const std::string &resource, const std::string &operation_type)
{
	const auto &approvals = access_token.get_approvals();
	auto it = approvals.find(resource);

	// Check if the user has the required approval from the token
	return it != approvals.end() && it->second.find(access_token.get_operation_to_code()[operation_type]) != std::string::npos;
}

// Endpoint to validate a Action on a resource
char **validate_delegated_action_1_svc(delegated_action_request_t *argp, struct svc_req *rqstp)
{
	// Log header at low severity server logs
	log("========= VALIDATE DELEGATED ACTION =========", 2);

	static char *result;

	// Parse the arguments
	std::string operation_type = argp->operation_type;
	std::string resource = argp->resource;
	std::string access_token_str = argp->access_token;

	log("Validating delegated action for user with access token " + access_token_str +
			" for operation " + operation_type + " on resource " + resource,
		2);

	// Check if the access token is empty
	if (access_token_str.empty())
	{
		log("Access token is empty. Returning PERMISSION_DENIED", 2);
		log_validation_result("DENY", operation_type, resource, access_token_str, 0, /*omit_token=*/true);
		result = strdup(ResponseCodes::getString(ResponseCodes::PERMISSION_DENIED).c_str());
		return &result;
	}

	// Find the access token
	Token access_token;
	std::string user_id;
	if (!find_access_token(access_token_str, access_token, user_id))
	{
		log("Access token not found", 2);
		log_validation_result("DENY", operation_type, resource, "", 0, /*omit_token=*/true);
		result = strdup(ResponseCodes::getString(ResponseCodes::PERMISSION_DENIED).c_str());
		return &result;
	}

	log("Access token with id " + access_token_str + " with lifetime " +
			std::to_string(access_token.get_lifetime()) + " found for user " + user_id,
		2);

	// Check if the token is expired
	if (access_token.get_lifetime() <= 0)
	{
		log("Access token expired", 2);
		log_validation_result("DENY", operation_type, resource, "", 0, /*omit_token=*/true);
		result = strdup(ResponseCodes::getString(ResponseCodes::TOKEN_EXPIRED).c_str());
		return &result;
	}

	// Decrease the lifetime of the token
	user_to_access_token[user_id].decrease_lifetime();
	access_token.decrease_lifetime();

	// Validate the operation type
	if (!is_valid_operation(operation_type))
	{
		log("Operation type not found. Returning OPERATION_NOT_PERMITTED", 2);
		log_validation_result("DENY", operation_type, resource, access_token_str, access_token.get_lifetime());
		result = strdup(ResponseCodes::getString(ResponseCodes::OPERATION_NOT_PERMITTED).c_str());
		return &result;
	}

	// Validate the resource
	if (!is_valid_resource(resource))
	{
		log("Resource not found", 2);
		log_validation_result("DENY", operation_type, resource, access_token_str, access_token.get_lifetime());
		result = strdup(ResponseCodes::getString(ResponseCodes::RESOURCE_NOT_FOUND).c_str());
		return &result;
	}

	// Check if the user has the required approval
	if (has_required_approval(access_token, resource, operation_type))
	{
		// All good, user has the permission, we can return PERMISSON_GRANTED
		log("User has the permission to perform the action", 2);
		log_validation_result("PERMIT", operation_type, resource, access_token_str, access_token.get_lifetime());
		result = strdup(ResponseCodes::getString(ResponseCodes::PERMISSION_GRANTED).c_str());
	}
	else
	{
		// User does not have the permission, return PERMISSION_DENIED
		log("User does not have the permission to perform the action", 2);
		log_validation_result("DENY", operation_type, resource, access_token_str, access_token.get_lifetime());
		result = strdup(ResponseCodes::getString(ResponseCodes::OPERATION_NOT_PERMITTED).c_str());
	}

	return &result;
}

// Find the token in the auth_token_list
Token *find_token(const std::string &token_received)
{
	// Find the token in the list
	for (Token &token : auth_token_list)
	{
		if (token.get_token() == token_received)
		{
			return &token;
		}
	}
	return nullptr;
}

// Check if approvals are available and valid for a user
bool has_valid_approvals(std::unordered_map<std::string, std::string> &approvals)
{
	return !approvals.empty() && !(approvals.find("*") != approvals.end() && approvals["*"] == "-");
}

// Add approvals to a token and sign it if unsigned
void add_approvals_and_sign(Token &token, const std::unordered_map<std::string, std::string> &approvals, const std::string &user_id_str)
{
	log("Adding approvals to token for user " + user_id_str, 2);
	token.add_approvals(approvals);

	// Sign the token if it is not already signed
	if (token.get_status() != Token::AUTH_SIGNED)
	{
		log("Signing the token and returning it", 2);
		token.sign();
	}
}

// Return the token as a result
char **return_token_result(const Token &token)
{
	static char *result;

	// Use strdup to avoid memory problems
	result = strdup(token.get_token().c_str());
	return &result;
}

// Endpoint to approve a request token
char **approve_request_token_1_svc(request_authorization_t *argp, struct svc_req *rqstp)
{
	// Log header at low severity server logs
	log("========= APPROVE REQUEST TOKEN =========", 2);

	static char *result;

	std::string token_received = argp->authentification_token;
	std::string user_id_str = argp->user_id;
	int refresh_token = argp->refresh_token;

	log("Request signature for token: " + token_received + " for user " + user_id_str +
			" with refresh token " + std::to_string(refresh_token),
		2);

	// Find the token in the list
	Token *token = find_token(token_received);
	if (token != nullptr)
	{
		// Token found, check if it is already signed
		if (token->get_status() == Token::AUTH_SIGNED)
		{
			log("Token already signed. Returning the token", 2);
			return return_token_result(*token);
		}
		else
		{
			log("Found token as not signed. Signing the token", 2);

			// Get the approvals in FIFO order
			std::unordered_map<std::string, std::string> approvals = user_to_approvals_list.back();
			user_to_approvals_list.pop_back();

			// Check if approvals are valid
			if (!has_valid_approvals(approvals))
			{
				log("No approvals found for user " + user_id_str, 2);
				result = strdup(token_received.c_str());
				return &result;
			}

			// Add approvals and sign the access token
			add_approvals_and_sign(*token, approvals, user_id_str);
			return return_token_result(*token);
		}
	}

	// Token not found, create a new one
	log("Token not found. Adding it to the list", 2);
	Token new_token(token_received, user_id_str, global_token_lifetime, Token::AUTH_UNSIGNED);

	// Get the approvals in FIFO order
	std::unordered_map<std::string, std::string> approvals = user_to_approvals_list[0];

	// Remove the first approval from the list
	user_to_approvals_list.erase(user_to_approvals_list.begin());

	// Check if approvals are valid
	if (!has_valid_approvals(approvals))
	{
		log("No approvals found for user " + user_id_str, 2);
		result = strdup(token_received.c_str());
		return &result;
	}

	// Add approvals and sign the access token
	add_approvals_and_sign(new_token, approvals, user_id_str);

	// Add the new token to the list and return it
	auth_token_list.push_back(new_token);
	return return_token_result(new_token);
}

// Find the user ID based on the access token
std::string find_user_id_by_token(const std::string &access_token_str)
{
	// Search for the access token in the user_to_access_token map
	for (const auto &user_access_token : user_to_access_token)
	{
		if (user_access_token.second.get_token() == access_token_str)
		{
			return user_access_token.first;
		}
	}
	return "";
}

// Generate a new access and refresh token
std::pair<std::string, std::string> generate_new_tokens(const std::string &refresh_token_str)
{
	// Use the refresh token to generate new access and refresh tokens
	std::string new_access_token = generate_access_token(strdup(refresh_token_str.c_str()));
	std::string new_refresh_token = generate_access_token(strdup(new_access_token.c_str()));
	return {new_access_token, new_refresh_token};
}

// Update the user access token in the map
void update_user_access_token(const std::string &user_id, const Token &current_token,
							  const std::string &new_access_token, const std::string &new_refresh_token)
{
	// Create a new access token and copy all the approvals
	Token new_access_token_i(new_access_token, new_refresh_token, current_token.get_user_id(),
							 global_token_lifetime, Token::ACCESS);
	new_access_token_i.copy_all_approvals(current_token);

	// Add the new access token to the user
	user_to_access_token[user_id] = new_access_token_i;
}

// Prepare and return the new access token in the result
access_token_t *prepare_access_token_result(access_token_t &result,
											const std::string &new_access_token,
											const std::string &new_refresh_token)
{
	// Use strdup to avoid memory problems
	result.access_token = strdup(new_access_token.c_str());
	result.refresh_token = strdup(new_refresh_token.c_str());

	// Set the expiration time as default for server
	result.expiration = global_token_lifetime;
	return &result;
}

// Handle the case where the access token is not found
access_token_t *handle_token_not_found(access_token_t &result)
{
	log("Access token not found. Could not refresh it", 2);

	// Fill with TOKEN_EXPIRED response
	result.access_token = strdup(ResponseCodes::getString(ResponseCodes::TOKEN_EXPIRED).c_str());
	result.refresh_token = strdup(ResponseCodes::getString(ResponseCodes::TOKEN_EXPIRED).c_str());
	result.expiration = -1;
	return &result;
}

// Endpoint to refresh the access token
access_token_t *refresh_access_1_svc(access_token_t *argp, struct svc_req *rqstp)
{
	// Log header at low severity server logs
	log("========= REFRESH ACCESS TOKEN =========", 2);

	static access_token_t result;

	// Parse the arguments
	std::string access_token_str = argp->access_token;
	std::string refresh_token_str = argp->refresh_token;

	// Find the user ID based on the access token
	std::string user_id = find_user_id_by_token(access_token_str);

	// Log the user authorization refresh attempt
	log("BEGIN " + user_id + " AUTHZ REFRESH", 0);
	log("Refresh access token with access token " + access_token_str +
			" and refresh token " + refresh_token_str,
		2);

	// If user ID is found, proceed to refresh the token
	if (!user_id.empty())
	{
		// Get the current access token
		Token &current_token = user_to_access_token[user_id];
		log("Access token found. Refreshing it", 2);

		// Generate new access and refresh tokens
		std::pair<std::string, std::string> tokens = generate_new_tokens(refresh_token_str);
		std::string new_access_token = tokens.first;
		std::string new_refresh_token = tokens.second;

		// Update the user access token map
		update_user_access_token(user_id, current_token, new_access_token, new_refresh_token);

		// Log and return the new access token result
		log("  AccessToken = " + new_access_token, 0);
		log("  RefreshToken = " + new_refresh_token, 0);
		return prepare_access_token_result(result, new_access_token, new_refresh_token);
	}

	// If access token not found, handle it with an appropriate response
	return handle_token_not_found(result);
}
