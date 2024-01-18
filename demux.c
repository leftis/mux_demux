#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1" // Replace with your server's IP address
#define SERVER_PORT 12345     // Replace with your server's port

#define MAX_BUFFER 64

int main()
{
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1)
  {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(SERVER_PORT);
  inet_pton(AF_INET, SERVER_IP, &(serverAddress.sin_addr));

  if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
  {
    perror("Connection failed");
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    // Send "fetch" command
    const char *fetchCommand = "fetch";
    if (send(clientSocket, fetchCommand, strlen(fetchCommand), 0) == -1)
    {
      perror("Send failed");
      exit(EXIT_FAILURE);
    }

    // Receive and print the response
    char responseBuffer[MAX_BUFFER];
    ssize_t bytesRead = recv(clientSocket, responseBuffer, sizeof(responseBuffer), 0);

    if (bytesRead == -1)
    {
      perror("Receive failed");
      exit(EXIT_FAILURE);
    }
    else if (bytesRead == 0)
    {
      // Server has no more data, break from the loop
      break;
    }

    responseBuffer[bytesRead] = '\0';
    printf("Received data: %s\n", responseBuffer);

    // Add a short delay (adjust as needed)
    usleep(500000); // 500 milliseconds
  }

  // Close the socket
  close(clientSocket);

  return 0;
}
