make server
# empty content of server_global_logging_file.txt
echo "" > server_global_logging_file.txt
echo "./server ./testing/test2/userIDs.db ./testing/test2/resources.db ./testing/test2/approvals.db 2"
./server ./testing/test2/userIDs.db ./testing/test2/resources.db ./testing/test2/approvals.db 2