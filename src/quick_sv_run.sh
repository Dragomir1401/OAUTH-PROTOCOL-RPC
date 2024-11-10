make server
# empty content of server_global_logging_file.txt
echo "" > server_global_logging_file.txt
echo "./server ./testing/test3/userIDs.db ./testing/test3/resources.db ./testing/test3/approvals.db 2"
./server ./testing/test3/userIDs.db ./testing/test3/resources.db ./testing/test3/approvals.db 2