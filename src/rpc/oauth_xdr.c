/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "oauth.h"

bool_t
xdr_access_token_t(XDR *xdrs, access_token_t *objp)
{
	register int32_t *buf;
	(void)buf;

	if (!xdr_string(xdrs, &objp->access_token, ~0))
		return FALSE;
	if (!xdr_string(xdrs, &objp->refresh_token, ~0))
		return FALSE;
	if (!xdr_int(xdrs, &objp->expiration))
		return FALSE;
	return TRUE;
}

bool_t
xdr_request_authorization_t(XDR *xdrs, request_authorization_t *objp)
{
	register int32_t *buf;
	(void)buf;

	if (!xdr_string(xdrs, &objp->user_id, ~0))
		return FALSE;
	if (!xdr_string(xdrs, &objp->authentification_token, ~0))
		return FALSE;
	if (!xdr_bool(xdrs, &objp->refresh_token))
		return FALSE;
	return TRUE;
}

bool_t
xdr_access_token_request_t(XDR *xdrs, access_token_request_t *objp)
{
	register int32_t *buf;
	(void)buf;

	if (!xdr_string(xdrs, &objp->user_id, ~0))
		return FALSE;
	if (!xdr_string(xdrs, &objp->authentification_token, ~0))
		return FALSE;
	if (!xdr_int(xdrs, &objp->auto_refresh))
		return FALSE;
	return TRUE;
}

bool_t
xdr_delegated_action_request_t(XDR *xdrs, delegated_action_request_t *objp)
{
	register int32_t *buf;
	(void)buf;

	if (!xdr_string(xdrs, &objp->operation_type, ~0))
		return FALSE;
	if (!xdr_string(xdrs, &objp->resource, ~0))
		return FALSE;
	if (!xdr_string(xdrs, &objp->access_token, ~0))
		return FALSE;
	return TRUE;
}
