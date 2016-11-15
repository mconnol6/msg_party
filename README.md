Mary Connolly mconnol6
John Rocha  jrocha2
Paul Dowling pdowling

Included Files:

    client:
        Makefile
            Typing "make" in the command line will compile the client code, creating executable "myfrm"
            executing "make clean" will remove the executable and object files if they exist
        client.cpp
            This file contains more C++ libraries and the class functions for Client.
        client.h
            This file conatins the included C++ libraries and the class "Client".
        hi.txt
        main.cpp
            This file checks the user's input for the exectuable and constructs Client c. If fails to connect to the server, then it exits. If it does connect, it awaits users input.

    server:
        Makefile
            Typing "make" in the command line will compile the server code, creating executable "myfrmd"
            executing "make clean" will remove the executable and object files if they exist
        main.cpp
            This file checks the user input for starting the server. If the conditions are correct, it will wait for a connection and will accept it if requirements are met. It will contine to listen for connections until it is told to shutdown.
        server.cpp
            This file contains the functions for the class "Server" and more C++ libraries
        server.h
            This file contains the included C++ libraries, the structs "Message" and "Board", and the class "Server"



How to Operate:
    1) run the executable in the server folder by using the following:
        ./myfrmd [PORT] [Password]
        The password sets the admin password to execute SHUTDOWN on the client side.
    2) run a client on a different machine and after making the exectuable, run the program with the following arguments:
        ./myfrm [Server_Name] [PORT]
        If the Server_Name or PORT do not match the server's then an error is returned. If the port is in use then it will wait until the port is available.
    3) The Client will be able to accept input and perform operations. The following are valid commands:
        CRT:
            Creates a message board. The user is then prompted to name the board. No Boards can have the same name.
        MSG:
            Add a message to an existing board. The user is prompted for the name of the board to post to and then prompted for the message. If the board does not exist, an error will occur.
        DLT:
            Deletes a message on a board. User is prompted for a board and then the index of the message. If the user is not the original poster, then an error will occur.
        EDT:
            Edits a message on a board. The user is prompted for the board and then the index of the message. If the user is not the original poster, then an error will occur.
        LIS:
            Shows all of the current boards.
        RDB:
            Shows the messages of the board. If a file is appended to the board it will say show the filename that is appended. If the board does not exist, it errors.
        APN:
            Appends a file to an existing board. User supplies the name of the board and the file to be appended. 
        DWN:
            Downloads a file that was appended to a board. The user supplies the name of the board and the file to be downloaded.
        DST:
            Destroys an existing board. All the messages and files appended to the board are destroyed in the process. Only the creator of the board can destroy the board.
        XIT:
            The user closes the client socket and exits the client. The server is still running and awaiting a user to connect.
        SHT:
            Shuts down the server. All boards are destroyed and all appended files are removed. This action requires the Admin password (one of the arguments to run the server).


