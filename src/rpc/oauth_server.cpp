/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "oauth.h"
#include "../../data/server_model/server_model.h"
#include "../utils/token.h"
#include "../utils/res_codes.hpp"
#include <iostream>
#include <algorithm>

void log(std::string message, int level = 0)
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

char **
request_authorization_1_svc(char **argp, struct svc_req *rqstp)
{
	static char *result;
	log("========= REQUEST AUTHORIZATION =========", 2);

	char *user_id = argp[0];

	for (std::string user : user_list)
	{
		if (user == user_id)
		{
			log("User found", 2);
			result = generate_access_token(user_id);
			log("Token generated: " + std::string(result), 2);
			return &result;
		}
	}

	log("User not found", 2);
	strcpy(result, ResponseCodes::getString(ResponseCodes::USER_NOT_FOUND).c_str());
	return &result;
}

access_token_t *
request_access_token_1_svc(access_token_request_t *argp, struct svc_req *rqstp)
{
	static access_token_t result;

	// parse the arguments
	std::string user_id = argp->user_id;
	std::string token_received = argp->authentification_token;
	int auto_refresh = argp->auto_refresh;

	log("BEGIN " + user_id + " AUTHZ", 0);

	// if user id is not in the user_list return a USER_NOT_FOUND error
	if (std::find(user_list.begin(), user_list.end(), user_id) == user_list.end())
	{
		log("User not found. Returning USER_NOT_FOUND", 2);
		result.access_token = strdup(ResponseCodes::getString(ResponseCodes::USER_NOT_FOUND).c_str());
		result.refresh_token = strdup(ResponseCodes::getString(ResponseCodes::USER_NOT_FOUND).c_str());
		result.expiration = -1;
		return &result;
	}

	log("  RequestToken = " + token_received, 0);

	log("========= REQUEST ACCESS TOKEN =========", 2);
	log("Request access token for user " + user_id + " with auth token " + token_received + " and auto refresh " + std::to_string(auto_refresh), 2);

	// search for the token in the token list
	for (Token token : auth_token_list)
	{
		if (token.get_token() == token_received)
		{
			if (token.get_status() == Token::AUTH_SIGNED)
			{
				log("Token found and signed. Generating an access token", 2);

				// convert the token to char*
				char *token_received_c = strdup(token.get_token().c_str());

				// generate access token
				std::string access_token_str = generate_access_token(token_received_c);

				log("Access token generated: " + access_token_str, 2);

				// generate a token instance with this access token
				Token access_token = Token(access_token_str, user_id, global_token_lifetime, Token::ACCESS);

				// add the approvals from the token to the access token
				std::unordered_map<std::string, std::string> approvals = token.get_approvals();
				access_token.add_approvals(approvals);

				std::string refresh_token = "";
				// if auto refresh is enabled then also generate a refresh token based on the access token
				if (auto_refresh)
				{
					log("Auto refresh enabled. Generating a refresh token", 2);

					// Convert the access token to char*
					char *access_token_c = strdup(access_token_str.c_str());
					// generate the refresh token
					refresh_token = generate_access_token(access_token_c);
					// add the refresh token to the token
					access_token.add_refresh_token(refresh_token);
				}

				// Add the access token to the user to access token map
				log("Adding access token " + access_token_str + " to the user " + user_id + " in the user_to_access_token map", 2);
				user_to_access_token[user_id] = access_token;

				// log the permissions that this access token has
				std::unordered_map<std::string, std::string> access_token_approvals = access_token.get_approvals();
				for (auto const &approval : access_token_approvals)
				{
					log("Approval for this access token: " + approval.first + " " + approval.second, 2);
				}

				// if no approvals or approval is - for * then return a REQUEST_DENIED error
				if (access_token_approvals.empty() || (access_token_approvals.find("*") != access_token_approvals.end() && access_token_approvals["*"] == "-"))
				{
					log("No approvals found for the user. Returning REQUEST_DENIED", 2);
					result.access_token = strdup(ResponseCodes::getString(ResponseCodes::REQUEST_DENIED).c_str());
					result.refresh_token = strdup(ResponseCodes::getString(ResponseCodes::REQUEST_DENIED).c_str());
					result.expiration = -1;
					return &result;
				}

				// return the access token
				result.access_token = strdup(access_token_str.c_str());
				result.refresh_token = strdup(refresh_token.c_str());
				result.expiration = global_token_lifetime;

				log("  AccessToken = " + access_token_str, 0);

				return &result;
			}
			else
			{
				// if the token is not signed return an REQUEST_DENIED error
				log("Token found but not signed. Returning REQUEST_DENIED", 2);
				result.access_token = strdup(ResponseCodes::getString(ResponseCodes::REQUEST_DENIED).c_str());
				result.refresh_token = strdup(ResponseCodes::getString(ResponseCodes::REQUEST_DENIED).c_str());
				result.expiration = -1;
				return &result;
			}
		}
	}

	// if the token is not found return an REQUEST_DENIED error
	log("Token not found. Returning REQUEST_DENIED", 2);
	result.access_token = strdup(ResponseCodes::getString(ResponseCodes::REQUEST_DENIED).c_str());
	result.refresh_token = strdup(ResponseCodes::getString(ResponseCodes::REQUEST_DENIED).c_str());
	result.expiration = -1;

	return &result;
}

char **
validate_delegated_action_1_svc(delegated_action_request_t *argp, struct svc_req *rqstp)
{
	static char *result;

	// parse the arguments
	std::string operation_type = argp->operation_type;
	std::string resource = argp->resource;
	std::string access_token_str = argp->access_token;

	log("========= VALIDATE DELEGATED ACTION =========", 2);
	log("Validating delegated action for user with access token " + access_token_str + " for operation " + operation_type + " on resource " + resource, 2);

	// if access_token_str is "" print a DENY (Operation,Resource,access_tokien,valability) and return a PERMISSION_DENIED error
	if (access_token_str == "")
	{
		log("Access token is empty. Returning PERMISSION_DENIED", 2);
		log("DENY (" + operation_type + "," + resource + "," + access_token_str + ",0)", 0);
		result = strdup(ResponseCodes::getString(ResponseCodes::PERMISSION_DENIED).c_str());
		return &result;
	}

	// check if the access token is in the user_to_access_token map
	for (auto const &user_access_token : user_to_access_token)
	{
		Token user_access_token_i = user_access_token.second;
		if (user_access_token_i.get_token() == access_token_str)
		{
			// get the access token based on the user id
			Token access_token = user_to_access_token[user_access_token.first];

			// based on the token, find the token in the
			log("Access token with id " + access_token_str + " with lifetime " + std::to_string(access_token.get_lifetime()) + " found for user " + user_access_token.first, 2);

			// check if the token is expired
			if (access_token.get_lifetime() <= 0)
			{
				log("Access token expired", 2);
				// log to stream 0 DENY (Operaion,Resource,access_token,validity)
				log("DENY (" + operation_type + "," + resource + "," + ",0)", 0);
				result = strdup(ResponseCodes::getString(ResponseCodes::TOKEN_EXPIRED).c_str());
				return &result;
			}

			// decrease the lifetime of the token
			user_to_access_token[user_access_token.first].decrease_lifetime();
			access_token.decrease_lifetime();

			// check if the resource exists in the resource_list
			if (std::find(resource_list.begin(), resource_list.end(), resource) == resource_list.end())
			{
				log("Resource not found", 2);
				// log to stream 0 DENY (Operaion,Resource,access_token,validity)
				log("DENY (" + operation_type + "," + resource + "," + access_token_str + "," + std::to_string(access_token.get_lifetime()) + ")", 0);
				result = strdup(ResponseCodes::getString(ResponseCodes::RESOURCE_NOT_FOUND).c_str());
				return &result;
			}

			// check if the user has the permission to perform the action
			std::unordered_map<std::string, std::string> approvals = access_token.get_approvals();

			// log if the approvals are empty
			if (approvals.empty())
			{
				log("No approvals found for the user", 2);
			}

			// print the approvals
			for (auto const &approval : approvals)
			{
				log("Approval: " + approval.first + " " + approval.second, 2);
			}

			log("Searching for operation type " + operation_type + " in the approvals", 2);

			// if the resource appears in approvals and the operation type lets say "D" for delete appears in the approvals string of that resource
			if (approvals.find(resource) != approvals.end() && approvals[resource].find(access_token.get_operation_to_code()[operation_type]) != std::string::npos)
			{
				log("User has the permission to perform the action", 2);
				// log to stream 0 PERMIT (Operaion,Resource,access_token,validity)
				log("PERMIT (" + operation_type + "," + resource + "," + access_token_str + "," + std::to_string(access_token.get_lifetime()) + ")", 0);
				result = strdup(ResponseCodes::getString(ResponseCodes::PERMISSION_GRANTED).c_str());
				return &result;
			}
			else
			{
				log("User does not have the permission to perform the action", 2);
				// log to stream 0 DENY (Operaion,Resource,access_token,validity)
				log("DENY (" + operation_type + "," + resource + "," + access_token_str + "," + std::to_string(access_token.get_lifetime()) + ")", 0);
				result = strdup(ResponseCodes::getString(ResponseCodes::OPERATION_NOT_PERMITTED).c_str());
				return &result;
			}
		}
		else
		{
			log("Access token not found", 2);
			// log to stream 0 DENY (Operaion,Resource,access_token,validity)
			log("DENY (" + operation_type + "," + resource + "," + access_token_str + ", 0)", 0);
			result = strdup(ResponseCodes::getString(ResponseCodes::PERMISSION_DENIED).c_str());
			return &result;
		}
	}

	// if the access token is not found return an error
	log("Access token not found", 2);
	// log to stream 0 DENY (Operaion,Resource,access_token,validity)
	log("DENY (" + operation_type + "," + resource + "," + access_token_str + ", 0)", 0);
	result = strdup(ResponseCodes::getString(ResponseCodes::PERMISSION_DENIED).c_str());
	return &result;
}

char **
approve_request_token_1_svc(request_authorization_t *argp, struct svc_req *rqstp)
{
	static char *result;

	std::string token_received = argp->authentification_token;
	std::string user_id_str = argp->user_id;
	int refresh_token = argp->refresh_token;

	log("========= APPROVE REQUEST TOKEN =========", 2);
	log("Request signature for token: " + token_received + " for user " + user_id_str + " with refresh token " + std::to_string(refresh_token), 2);

	// if we find the token in the list
	for (Token token : auth_token_list)
	{
		if (token.get_token() == token_received)
		{
			// if the token is signed
			if (token.get_status() == Token::AUTH_SIGNED)
			{
				log("Token already signed. Returning the token", 2);
				// return the token
				strcpy(result, token.get_token().c_str());
				return &result;
			}
			else
			{
				log("Found token as not signed. Signing the token", 2);
				// based on the approvals the user has in user_to_approvals_list, add them to the token
				// make user_id from argp string
				std::unordered_map<std::string, std::string> approvals = user_to_approvals_list.back();
				user_to_approvals_list.pop_back();

				// if there are no approvals return token as unsigned
				if (approvals.empty() || (approvals.find("*") != approvals.end() && approvals["*"] == "-"))
				{
					log("No approvals found for user " + user_id_str, 2);
					strcpy(result, token_received.c_str());
					return &result;
				}

				log("Adding approvals to token for user " + user_id_str, 2);
				token.add_approvals(approvals);

				// if the token is unsigned, sign it
				log("Signing the token and returning it", 2);
				token.sign();

				// return the token
				strcpy(result, token.get_token().c_str());
				return &result;
			}
		}
	}

	// return the token unchanged if no approvals are to be added
	log("Token not found. Adding it to the list", 2);
	Token token = Token(argp->authentification_token, user_id_str, global_token_lifetime, Token::AUTH_UNSIGNED);

	// based on the approvals the user has in user_to_approvals_list, add them to the token
	std::unordered_map<std::string, std::string> approvals = user_to_approvals_list[0];
	user_to_approvals_list.erase(user_to_approvals_list.begin());

	// if there are no approvals return token as unsigned
	if (approvals.empty() || (approvals.find("*") != approvals.end() && approvals["*"] == "-"))
	{
		log("No approvals found for user " + user_id_str, 2);
		strcpy(result, token_received.c_str());
		return &result;
	}

	log("Adding approvals to token for user " + user_id_str, 2);
	token.add_approvals(approvals);

	// if the token is unsigned, sign it
	log("Signing the token and returning it", 2);
	token.sign();

	// add the token to the list
	auth_token_list.push_back(token);

	std::string result_str = token.get_token();
	result = strdup(result_str.c_str());

	return &result;
}

access_token_t *
refresh_access_1_svc(access_token_t *argp, struct svc_req *rqstp)
{
	static access_token_t result;

	// parse the arguments
	std::string access_token_str = argp->access_token;
	std::string refresh_token_str = argp->refresh_token;

	log("========= REFRESH ACCESS TOKEN =========", 2);
	log("Refresh access token with access token " + access_token_str + " and refresh token " + refresh_token_str, 2);

	// find the access token in the user_to_access_token map
	for (auto const &user_access_token : user_to_access_token)
	{
		Token user_access_token_i = user_access_token.second;
		if (user_access_token_i.get_token() == access_token_str)
		{
			log("Access token found. Refreshing it", 2);

			// generate a new access token
			std::string new_access_token = generate_access_token(strdup(refresh_token_str.c_str()));

			// generate a new refresh token
			std::string new_refresh_token = generate_access_token(strdup(new_access_token.c_str()));

			// generate a new access token instance
			Token new_access_token_i = Token(new_access_token, new_refresh_token, user_access_token_i.get_user_id(), global_token_lifetime, Token::ACCESS);

			// update the access token in the user_to_access_token map
			user_to_access_token[user_access_token.first] = new_access_token_i;

			// return the new access token
			result.access_token = strdup(new_access_token.c_str());
			result.refresh_token = strdup(new_refresh_token.c_str());
			result.expiration = global_token_lifetime;

			return &result;
		}
	}

	// if the access token is not found return an error
	log("Access token not found. Could not refresh it", 2);
	result.access_token = strdup(ResponseCodes::getString(ResponseCodes::TOKEN_EXPIRED).c_str());
	result.refresh_token = strdup(ResponseCodes::getString(ResponseCodes::TOKEN_EXPIRED).c_str());
	result.expiration = -1;

	return &result;
}
