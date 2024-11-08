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

enum response_codes_to_attempt_auth {
    NOT_FOUND = 0
};


program OAUTH_PROTOCOL {
    version OAUTH_VERSION {
        string attemp_auth(string) = 1;
        access_token_t request_access_token(request_authorization_t) = 2;
    } = 1;
} = 0x31234567;