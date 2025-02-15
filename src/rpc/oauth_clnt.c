/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "oauth.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = {25, 0};

char **
request_authorization_1(char **argp, CLIENT *clnt)
{
	static char *clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, request_authorization,
				  (xdrproc_t)xdr_wrapstring, (caddr_t)argp,
				  (xdrproc_t)xdr_wrapstring, (caddr_t)&clnt_res,
				  TIMEOUT) != RPC_SUCCESS)
	{
		return (NULL);
	}
	return (&clnt_res);
}

access_token_t *
request_access_token_1(access_token_request_t *argp, CLIENT *clnt)
{
	static access_token_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, request_access_token,
				  (xdrproc_t)xdr_access_token_request_t, (caddr_t)argp,
				  (xdrproc_t)xdr_access_token_t, (caddr_t)&clnt_res,
				  TIMEOUT) != RPC_SUCCESS)
	{
		return (NULL);
	}
	return (&clnt_res);
}

char **
validate_delegated_action_1(delegated_action_request_t *argp, CLIENT *clnt)
{
	static char *clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, validate_delegated_action,
				  (xdrproc_t)xdr_delegated_action_request_t, (caddr_t)argp,
				  (xdrproc_t)xdr_wrapstring, (caddr_t)&clnt_res,
				  TIMEOUT) != RPC_SUCCESS)
	{
		return (NULL);
	}
	return (&clnt_res);
}

char **
approve_request_token_1(request_authorization_t *argp, CLIENT *clnt)
{
	static char *clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, approve_request_token,
				  (xdrproc_t)xdr_request_authorization_t, (caddr_t)argp,
				  (xdrproc_t)xdr_wrapstring, (caddr_t)&clnt_res,
				  TIMEOUT) != RPC_SUCCESS)
	{
		return (NULL);
	}
	return (&clnt_res);
}

access_token_t *
refresh_access_1(access_token_t *argp, CLIENT *clnt)
{
	static access_token_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call(clnt, refresh_access,
				  (xdrproc_t)xdr_access_token_t, (caddr_t)argp,
				  (xdrproc_t)xdr_access_token_t, (caddr_t)&clnt_res,
				  TIMEOUT) != RPC_SUCCESS)
	{
		return (NULL);
	}
	return (&clnt_res);
}
