## ❗❗❗ This repository is currently in development.

## Advanced Summary of This IRC Server

This IRC server is implemented in C++ following the standard IRC protocol. It is designed to handle multiple client connections over IPv6 using non-blocking sockets and the `poll()` system call, ensuring efficient management of numerous simultaneous connections. The server’s architecture is highly modular, with each command (such as PASS, NICK, USER, JOIN, MODE, TOPIC, etc.) encapsulated in its own class derived from a common base `Command` class, which simplifies extension and maintenance.

---

### Server Workflow

1. **Startup and Initialization**  
   - **Main Function:**  
     The program starts in the `main()` function, where command-line arguments are validated. The server requires two arguments: a port number (as a numeric value) and a password. Once validated, an instance of the `Server` class is created, and its `listen()` method is invoked.
   - **Socket Setup:**  
     The server sets up a master socket configured for IPv6 TCP connections, enabling non-blocking I/O through `fcntl()`. It binds to the specified port and then listens for incoming connections.
   - **Signal Handling:**  
     Signal handlers (for SIGINT to shutdown and SIGQUIT to toggle debug mode) are registered. These handlers set global flags so that the server can shut down gracefully or toggle debug output.
   - **Polling for Activity:**  
     The `poll()` system call is used to monitor the master socket and all connected client sockets. When an event is detected, the server either accepts a new connection or reads incoming data from an existing client.

2. **Connection Management**  
   - **Accepting Clients:**  
     When a new connection is detected on the master socket, the server accepts it. The client’s IP address is processed (e.g., for IPv6-mapped IPv4 addresses), and a new `Client` object is created.
   - **Non-Blocking I/O and Dynamic Polling:**  
     Each client's socket is added to a dynamically maintained array of file descriptors for polling, ensuring that all sockets are monitored for incoming data.
   - **Graceful Shutdown:**  
     When shutdown signals are received, the server stops accepting new connections and disconnects clients gracefully.

3. **Data Reception and Command Handling**  
   - **Receiving Data:**  
     The server reads data from client sockets in a non-blocking manner. Data is buffered until a complete command (terminated by a newline) is received; incomplete messages are stored until additional data arrives.
   - **Command Parsing and Execution:**  
     Incoming messages are parsed into individual commands. The `CommandHandler` class maintains a mapping between command names (e.g., PASS, NICK, USER, JOIN, PART, MODE, TOPIC, KICK, PRIVMSG, NOTICE, WHO, LIST) and their corresponding command objects. It then validates the parameters, checks registration and permissions when necessary, and finally executes the command using the appropriate `execute()` method.

---

### Supported Commands and Their Functions

- **PASS:**  
  Authenticates a client by comparing the provided password with the server’s password. If incorrect, an error is returned.

- **NICK:**  
  Sets or changes the client's nickname. It checks for duplicates, and if the nickname is valid and available, the client's nickname is updated. Once both NICK and USER commands are successfully executed, the welcome sequence is initiated.

- **USER:**  
  Provides the username and real name necessary for client registration. This command, in conjunction with NICK, finalizes the client registration process.

- **QUIT:**  
  Disconnects a client from the server, optionally with a quit message. It sends a quit reply and removes the client from the server's client list.

- **PING / PONG:**  
  These commands help maintain the connection. A PING command expects a response in the form of a PONG, ensuring that both the server and client remain active.

- **JOIN:**  
  Allows clients to join a channel. If the channel does not exist, it is created. The command verifies channel conditions such as invite-only status, maximum user limits, and password requirements.

- **PART:**  
  Lets a client leave a channel, optionally providing a departure reason. The command ensures that the client is a member of the channel before processing the part request.

- **MODE:**  
  Changes channel settings or user privileges. Supported modes include:
  - **i:** Toggle invite-only mode.
  - **l:** Set or remove a user limit.
  - **k:** Set or remove a channel password.
  - **o:** Grant or revoke operator privileges.
  - **t:** Set or remove topic restriction. When topic restriction (mode +t) is active, only the channel admin or operators can change the channel topic.
  
  Mode changes are broadcast to all channel members so that everyone is informed of the updated channel configuration.

- **TOPIC:**  
  Allows querying or setting the topic of a channel. When issued with just the channel name, it returns the current topic (or indicates that no topic is set). When a new topic is provided (prefixed with a colon), the command verifies, if necessary, that the client has the privileges to change the topic (if the channel is topic-restricted), updates the topic, and broadcasts the change.

- **KICK:**  
  Removes a user from a channel. This command checks that the requester has the necessary privileges (admin or operator) before kicking a user, and it allows specifying a reason for the kick.

- **INVITE:**  
  Invites a user to a channel. The command checks that the inviter is a channel member and, for invite-only channels, that they have sufficient privileges. The invited user receives an invitation and may join the channel.

- **PRIVMSG:**  
  Sends private messages to a specific user or to an entire channel. If the target is a channel, the message is broadcast to all channel members (excluding the sender); if the target is a user, the message is sent directly.

- **NOTICE:**  
  Similar to PRIVMSG but designed not to generate automatic replies. It is used for sending informational messages.

- **WHO:**  
  Provides a list of users on the server or in a specific channel, including details like username, hostname, and real name.

- **LIST:**  
  Displays a list of channels on the server along with details such as the number of clients and the channel topic. It supports optional filtering by channel names.

---

### Modes and Channel Management

This IRC server supports several channel modes to provide flexible control over channel behavior:

- **Invite-Only Mode (`i`):**  
  Restricts channel access to invited users only.
  
- **User Limit Mode (`l`):**  
  Sets a maximum limit on the number of users who can join the channel.
  
- **Password Mode (`k`):**  
  Requires a password for joining the channel.
  
- **Operator Mode (`o`):**  
  Designates certain users as channel operators, granting them the ability to manage channel settings and user privileges.
  
- **Topic Restriction Mode (`t`):**  
  When active, only the channel admin or operators can change the channel topic. This is enforced by the TOPIC command, which checks the topic restriction flag before allowing a topic change.

Mode changes are processed via the MODE command and immediately broadcast to all members of the channel.

---

### How It All Comes Together

1. **Client Connection and Registration:**  
   Clients connect to the server and must first authenticate using the PASS command. They then register by setting their nickname with NICK and providing user details with USER. Once registered, a series of welcome messages (including server information and MOTD) are sent to the client.

2. **Command Processing:**  
   Each command received from a client is parsed by the `CommandHandler`, which then delegates execution to the appropriate command object. This modular approach allows for easy extension and customization of the server’s functionality.

3. **Channel Interaction:**  
   Clients can join channels, part from them, change channel modes (including topic restrictions), and interact with other users via commands such as PRIVMSG and NOTICE. Channel membership and modes are maintained by the server, and updates are broadcast to all channel members.

4. **Graceful Shutdown and Maintenance:**  
   Signal handling ensures that the server can shut down gracefully, notifying clients appropriately. Debug mode can be toggled to assist with troubleshooting.

---

### Usage Guide

- **Starting the Server:**  
  Compile and run the server with the port number and server password as arguments. For example:  
  `./ircserv 6667 mypassword`

- **Client Registration:**  
  To register, a client must send the following commands in order:
  1. **PASS <password>** – Authenticate with the server.
  2. **NICK <nickname>** – Set a unique nickname.
  3. **USER <username> 0 * :<realname>** – Provide user details.

- **Channel Operations:**  
  - **JOIN <channel> [<password>]** – Join or create a channel.
  - **PART <channel> [<reason>]** – Leave a channel.
  - **MODE <channel> <modes> [<parameters>]** – Change channel modes. For example:
    - **+i** or **-i**: Enable or disable invite-only mode.
    - **+l <limit>** or **-l**: Set or remove the user limit.
    - **+k <key>** or **-k**: Set or remove the channel password.
    - **+o <nick>** or **-o <nick>**: Grant or revoke operator privileges.
    - **+t** or **-t**: Enable or disable topic restriction (only operators/admin can change the topic when enabled).
  - **KICK <channel> <user> [<reason>]** – Remove a user from a channel.
  - **INVITE <user> <channel>** – Invite a user to a channel.

- **Messaging:**  
  - **PRIVMSG <target> :<message>** – Send a private message to a user or channel.
  - **NOTICE <target> :<message>** – Send a notice to a user or channel without expecting replies.

- **Server Queries:**  
  - **WHO [<channel>]** – List users on the server or in a specific channel.
  - **LIST [<channel1,channel2,...>]** – List available channels along with details.

- **Topic Management:**  
  - **TOPIC <channel>** – Query the current topic of a channel.
  - **TOPIC <channel> :<new topic>** – Set a new topic for a channel. If the channel is topic-restricted (mode +t), only the channel admin or operators can change the topic.

- **Keep-Alive and Disconnection:**  
  - **PING/PONG** commands maintain the connection.
  - **QUIT [<reason>]** disconnects the client from the server.

---

### Conclusion

This IRC server is a comprehensive implementation that adheres to the IRC protocol. Its modular design—built on a robust command handler system and non-blocking I/O—ensures efficient connection management, command processing, and channel interactions. The inclusion of the TOPIC command and topic restriction mode (+t / -t) further enhances channel management by controlling who can change a channel's topic. Extensive documentation (using Doxygen style) throughout the codebase makes it easier to understand, maintain, and extend the server's functionality. This summary provides a complete guide to understanding the overall workflow, supported commands, channel modes, and practical usage of the server, even for someone encountering the code for the first time.

(Tested with Irssi v1.2.3 client.)

