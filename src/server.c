#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <winsock2.h>
#include <Ws2tcpip.h>

#define PORT 3000 // the port on which the server will listen
#define INPUT_BUFFER_SIZE 64 //  input buffer size
#define OUTPUT_BUFFER_SIZE 74 // output buffer size

// utility function to check if the result of an action is successful otherwise exit the program
void check_result(int result, char *message)
{
  if (result < 0) {
    printf(message);
    exit(-1);
  }
}

int main()
{
  printf("Server started\n");

  struct sockaddr_in server_configuration; // we declare a struct for the server configuration
  int socket_bind_configuration_size = sizeof(server_configuration); // size of the server configuration struct
  int server_alive = 1; // flag for server alive

  WSADATA wsaData;
  int start_up = WSAStartup(0x202, &wsaData); // windows requires initialization
  check_result(start_up,"Socket initialization\n");

  // Docs on socket: https://www.geeksforgeeks.org/socket-programming-cc/
  // AF_INET is the domain which is IPV4 (different from AF_INET6 which is IPV6)
  // SOCK_STREAM is the type of socket (TCP) (different from SOCK_DGRAM which is UDP)
  // 0 is the protocol which is IP
  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  check_result(socketfd,"Socket creation failed\n"); // we check if the creation of the socket was successful

  server_configuration.sin_family = AF_INET; // we now set the domain to IPV4 
  server_configuration.sin_addr.s_addr = INADDR_ANY; // we set the IP address to any (we can use INADDR_ANY to listen on all interfaces)
  server_configuration.sin_port = htons(PORT); // we now set the port to 3000 (where the server is listening) (htons is a function that is used to convert the port number from host to network byte order (big endian))

  // we bind the socket to the server configuration
  // first argument is the socket we want to bind which is the one we created in the beginning
  // second argument is the server configuration (struct server_configuration)
  // third argument is the size of the server configuration struct (in this case it is sizeof(server_configuration))
  int bind_result = bind(socketfd, (struct sockaddr*) &server_configuration, sizeof(server_configuration));

  check_result(bind_result,"Socket bind failed\n"); // we check if the bind was successful

  int listen_result = listen(socketfd, 1); // we listen on the socket with a backlog of 1 (the backlog is the number of connections that can be queued up before the system starts rejecting new connections)

  check_result(listen_result,"Socket listen failed\n"); // we check if the listen was successful

  // we accept the new connection from the client
  // first argument is the socket on which we want to accept the connection
  // second argument is the server configuration (struct server_configuration)
  // third argument is the size of the server configuration struct (in this case it is sizeof(server_configuration))
  int new_connection = accept(socketfd, (struct sockaddr*) &server_configuration, (socklen_t *) &socket_bind_configuration_size);
  check_result(new_connection,"Socket accept failed\n"); // we check if the accept was successful

  while (server_alive) {

    char input[INPUT_BUFFER_SIZE];  // we prepare a buffer for user input of the size of INPUT_BUFFER_SIZE 
    char output[OUTPUT_BUFFER_SIZE]; // we prepare a buffer for server response of the size of OUTPUT_BUFFER_SIZE
    
    int read_result = recv(new_connection, input, INPUT_BUFFER_SIZE,0);    // we read the input from the client
    check_result(read_result,"Socket read failed\n"); // we check if the read was successful
    input[read_result] = '\0';  // we terminate the string
    
    printf("the input from client is %s\n",input);

    if(strcmp("exit",input)  == 0){  // check if the user input is exit so we quit the program clean
        printf("exit command received");
        exit(0);
    }

    strcpy(output, "Hello "); // we prepare the first part of the output message
    strcat(output, input); // we append the input to the output message
    strcat(output, "!\n"); // we append the ending to the ouput message

    printf("the output from server is %s\n",output);
    send(new_connection, output, strlen(output), 0);   // we send the output to the client
  }
  return 0;
}