# Client-Server Application

This is a simple client-server application written in C++ that demonstrates socket programming and basic command execution on the server side. The server listens for incoming connections on a specified port, and the client can send commands to the server, which are then executed on the server's terminal.

## Features

- Server listens for incoming connections on a user-specified port
- Client can connect to the server and send commands
- Server executes commands enclosed within double hyphens (`--command--`)
- Support for executing basic terminal commands (e.g., `--dir--`, `--cls--`)
- Multithreaded server to handle multiple clients concurrently (not implemented in this version)

## Prerequisites

- C++ compiler with C++11 or later support
- Windows operating system

## Running

From the client terminal, send commands to the server by enclosing them within double hyphens (`--command--`). For example:

```
--dir--
--cls--
```

The server will execute the received commands and display the output on its terminal.


## License

This project is licensed under the [MIT License](LICENSE).