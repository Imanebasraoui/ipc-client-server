#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <winsock2.h>

#define PORT 3000 // the port on which we will connect to the server
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

int main(void) 
{
  printf("Client started...\n");
 
  struct sockaddr_in server_adress; // struct for server connection configuration

  int client_alive = 1; // flag for client alive

  // this part is used to initialize the socket on windows otherwise the socket will fail
  // it must be called before initializing the socket
  // docs: https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup
  WSADATA wsaData;  
  int start_up = WSAStartup(0x202, &wsaData);
  check_result(start_up,"Socket initialization\n");

  // Docs on socket: https://www.geeksforgeeks.org/socket-programming-cc/
  // AF_INET is the domain which is IPV4 (different from AF_INET6 which is IPV6)
  // SOCK_STREAM is the type of socket (TCP) (different from SOCK_DGRAM which is UDP)
  // 0 is the protocol which is IP
  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  check_result(socketfd,"Socket creation failed\n");

  server_adress.sin_family = AF_INET; // we now set the domain to IPV4 
  server_adress.sin_addr.s_addr = inet_addr("127.0.0.1"); // we now set the IP address to localhost (where the server is listening) (inet_addr is a function that converts a string to a 32 bit integer)
  server_adress.sin_port = htons(PORT); // we now set the port to 3000 (where the server is listening) (htons is a function that is used to convert the port number from host to network byte order (big endian))

  // we connect to the server
  // fist argument is the socket we want to connect
  // second argument is the server connection configuration
  // third argument is the size of the server connection configuration struct (in this case it is sizeof(server_adress))
  int connection = connect(socketfd, (struct sockaddr*) &server_adress, sizeof(server_adress));
  check_result(connection,"server connection failed\n");

  // we are connected to the server, now we can start listening for user input and sending it to the server
  while (client_alive) {
    char user_input[INPUT_BUFFER_SIZE]; // buffer for user input
    char server_response[OUTPUT_BUFFER_SIZE]; // buffer for server response

    printf("Hi what's your name ?\n");
    scanf("%63s",user_input,INPUT_BUFFER_SIZE); // scanf we put the user input into the user_input buffer (%64s) means the user can input max 64 characters
    user_input[INPUT_BUFFER_SIZE - 1] = '\0'; // add string termination to user input
    send(socketfd, user_input, strlen(user_input), 0); // send the user input to the server

    if(strcmp("exit",user_input) == 0){ // we check if we receive exit command from the user input to we exit the process clean
        printf("exit command received");
        exit(0);
    }

    int read_value = recv(socketfd, server_response, OUTPUT_BUFFER_SIZE,0); // read the server response into the server_response buffer
    server_response[read_value] = '\0'; // we terminate the string
    printf("%s\n", server_response); // print the server response
  }

  return 0;
}