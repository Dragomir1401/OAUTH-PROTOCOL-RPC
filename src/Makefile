CXX = g++
CXX_FLAGS = -lnsl -g -I./rpc -I./data/client_model
CLIENT = client
CLIENT_SRC = rpc/oauth_clnt.c rpc/oauth_xdr.c rpc/oauth_client.cpp client_driver/operations_processor/operations_processor.cpp client_driver/operations_parser/operations_parser.cpp client_driver/operations/operation.cpp utils/res_codes.cpp data/client_model/client_model.cpp
SERVER = server
SERVER_SRC = rpc/oauth_server.cpp rpc/oauth_svc.c rpc/oauth_xdr.c data/server_model/server_model.cpp utils/res_codes.cpp
RPC_GEN_FILES = rpc/Makefile.oauth rpc/oauth_client.cpp rpc/oauth_clnt.c rpc/oauth_server.cpp rpc/oauth_svc.c rpc/oauth_xdr.c rpc/oauth.h


all: $(CLIENT) $(SERVER)

client: $(CLIENT_SRC) $(RPC_GEN_FILES)
	$(CXX) $(CXX_FLAGS) -o $(CLIENT) $(CLIENT_SRC)

server: $(SERVER_SRC) $(RPC_GEN_FILES)
	$(CXX) $(CXX_FLAGS) -o $(SERVER) $(SERVER_SRC)

clean:
	rm -f $(CLIENT) $(SERVER) $(RPC_GEN_FILES)
