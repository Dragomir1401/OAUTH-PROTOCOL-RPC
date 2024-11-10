make server
# empty content of server_global_logging_file.txt
echo "" > server_global_logging_file.txt
echo "./server ./testing/test7/userIDs.db ./testing/test7/resources.db ./testing/test7/approvals.db 3"
./server ./testing/test7/userIDs.db ./testing/test7/resources.db ./testing/test7/approvals.db 3