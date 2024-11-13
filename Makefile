CXX = g++
CXX_FLAGS = -lnsl -g -Wall -std=c++11 -I src/rpc
CLIENT = client
CLIENT_SRC = src/rpc/oauth_clnt.c src/rpc/oauth_xdr.c src/rpc/oauth_client.cpp src/client_driver/operations_processor/operations_processor.cpp src/client_driver/operations_parser/operations_parser.cpp src/client_driver/operations/operation.cpp src/utils/res_codes.cpp src/data/client_model/client_model.cpp
SERVER = server
SERVER_SRC = src/rpc/oauth_server.cpp src/rpc/oauth_svc.c src/rpc/oauth_xdr.c src/data/server_model/server_model.cpp src/utils/res_codes.cpp
RPC_GEN_FILES = src/rpc/Makefile.oauth src/rpc/oauth_client.cpp src/rpc/oauth_clnt.c src/rpc/oauth_server.cpp src/rpc/oauth_svc.c src/rpc/oauth_xdr.c src/rpc/oauth.h


all: $(CLIENT) $(SERVER)

client: $(CLIENT_SRC) $(RPC_GEN_FILES)
	$(CXX) $(CXX_FLAGS) -o $(CLIENT) $(CLIENT_SRC)

server: $(SERVER_SRC) $(RPC_GEN_FILES)
	$(CXX) $(CXX_FLAGS) -o $(SERVER) $(SERVER_SRC)

clean:
	rm -f $(CLIENT) $(SERVER) $(RPC_GEN_FILES)
