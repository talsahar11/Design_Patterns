Design_Patters task - Operating systems:

Submitters -
    Tal Sahar - 206955452
    Asaf Galili - 315785006

Instructions -

    1. Run the command make all to compile all the necessary files
    2. Include the current directory in the LD_PATH by running the following: export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
    3. Run the server executable using the command: ./react_server
    4. Run the wanted client executables that will connect on port 9034 (The client side is not included in our files)
    5. All is up, each data that will be received by the server, will be printed to the terminal

Our testing -
    We have tested the system with up to 100 clients, and the system should support unlimited number of clients.

System explanation -
    1. The system establish a listening socket on the local-host ip on port 9034, and listening to new connections
        in a infinite loop.
    2. A reactor structure is being created, memory is being allocated for the pollfds and for the map.
    3. When calling the reactor start method, a thread is being created and starting, the thread is managing a poll of
        all the accepted fd's.
        For each fd that been accepted, the reactor saves a dedicated function, that will be called if the fd is hot.
    4. The poll have default size of 2, and if needed the pfds array and the hash map are being dynamically extended.
    5. When a fd is "hot", if the connection has been closed, the fd being removed from both the map and the pfds array,
        if data has been received, it is being printed to the terminal.



