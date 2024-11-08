struct access_token_t {
    string access_token<>;
    string refresh_token<>;
    int expiration;
};

struct request_authorization_t {
    string user_id<>;
    string authentification_token<>;
    bool refresh_token;
};

struct access_token_request_t {
    string user_id<>;
    string authentification_token<>;
    int auto_refresh;
};

struct delegated_action_request_t {
    string operation_type<>;
    string resource<>;
    string access_token<>;
};

program OAUTH_PROTOCOL {
    version OAUTH_VERSION {
        string request_authorization(string) = 1;
        access_token_t request_access_token(access_token_request_t) = 2;
        string validate_delegated_action(delegated_action_request_t) = 3;
        string approve_request_token(string) = 4;
        access_token_t refresh_access(access_token_t) = 5;
    } = 1;
} = 0x31234567;