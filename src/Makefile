CXX = g++
CXX_FLAGS = -Wall -lnsl -g -I./rpc -I./data/client_model
CLIENT = client
CLIENT_SRC = rpc/oauth_clnt.c rpc/oauth_xdr.c data/client_model/client_model.cpp data/client_data/client_instance.cpp
SERVER = server
SERVER_SRC = rpc/oauth_svc.c rpc/oauth_xdr.c data/server_model/server_model.cpp data/server_data/server_instance.cpp
RPCGEN = rpcgen
RPC_SRC = rpc/oauth.x
RPC_GEN_FILES = rpc/oauth.h rpc/oauth_clnt.c rpc/oauth_svc.c rpc/oauth_xdr.c


all: $(CLIENT) $(SERVER)

# Rule for generating RPC files
rpcgen_files: $(RPC_GEN_FILES)

rpc/oauth.h: $(RPC_SRC)
	$(RPCGEN) -h -o rpc/oauth.h $(RPC_SRC)

rpc/oauth_clnt.c: $(RPC_SRC)
	$(RPCGEN) -l -o rpc/oauth_clnt.c $(RPC_SRC)

rpc/oauth_svc.c: $(RPC_SRC)
	$(RPCGEN) -m -o rpc/oauth_svc.c $(RPC_SRC)

rpc/oauth_xdr.c: $(RPC_SRC)
	$(RPCGEN) -c -o rpc/oauth_xdr.c $(RPC_SRC)

# Rule for building client and server
build: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_SRC) $(RPC_GEN_FILES)
	$(CXX) $(CXX_FLAGS) -o $(CLIENT) $(CLIENT_SRC)

$(SERVER): $(SERVER_SRC) $(RPC_GEN_FILES)
	$(CXX) $(CXX_FLAGS) -o $(SERVER) $(SERVER_SRC)

clean:
	rm -f $(CLIENT) $(SERVER) $(RPC_GEN_FILES)
