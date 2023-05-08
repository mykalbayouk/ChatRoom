* About
This is a Chat Room application built with C. It utilizes socket programming to allow
multiple users to connect to a server and chat with each other. The server is able to handle
multiple clients at once using p_threads. Up to 10 clients can connect to the server at once. 

* How to run
1. Compile the server and client files using the makefile
2. Run the server using the command `./server <port number>`
3. Run the client using the command `./client <port number>`
4. Enter a username and begin chatting

* Commands
    ```
    make - compiles the server and client files
    ```
    ```
    make clean - removes the server and client executables
    ```
    ```
    "exit" - exits the chat room
    ```

