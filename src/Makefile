CXX = g++
CXX_FLAGS = -Wall -lnsl -g -I./rpc -I./data/client_model
CLIENT = client
CLIENT_SRC = rpc/oauth_clnt.c rpc/oauth_xdr.c data/client_model/client_model.cpp data/client_data/client_instance.cpp client.cpp
SERVER = server
SERVER_SRC = rpc/oauth_svc.c rpc/oauth_xdr.c data/server_model/server_model.cpp data/server_data/server_instance.cpp
RPCGEN = rpcgen
RPC_SRC = /rpc/oauth.x
RPC_GEN_FILES = rpc/oauth.h rpc/oauth_clnt.c rpc/oauth_svc.c rpc/oauth_xdr.c


all: $(CLIENT) $(SERVER)

client: $(CLIENT_SRC) $(RPC_GEN_FILES)
	$(CXX) $(CXX_FLAGS) -o $(CLIENT) $(CLIENT_SRC)

server: $(SERVER_SRC) $(RPC_GEN_FILES)
	$(CXX) $(CXX_FLAGS) -o $(SERVER) $(SERVER_SRC)

clean:
	rm -f $(CLIENT) $(SERVER) $(RPC_GEN_FILES)
