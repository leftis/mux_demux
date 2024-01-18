// multiplexer.c
// Simple multiplexer 2 to 1
// - Control bit gets controlled by weights that are allocated on sources
// - Implementation uses static sources, static packages
// - You can test the implementation by doing a telnet 127.0.0.1 12345
// -

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <time.h>

#define PORT 12345
#define MAX_BUFFER 64
#define BIG_PACKET_SIZE 640
#define PACKET_SIZE 64
#define SOURCE_0_PACKAGES 4
#define SOURCE_1_PACKAGES 2

typedef struct
{
  char id;
  char *data;
  int consumed_packets;
  double ratio;
} source;

typedef struct {
  char id;
  char data[PACKET_SIZE-1];
} small_packet;

double randomDouble() {
  return (double)rand() / RAND_MAX;
}

int bitToggle(double ratioA, double ratioB) {
  double randomValue = randomDouble();

  if (randomValue < ratioA / (ratioA + ratioB))
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

void initDataPackages(source *s, int big_packets_numbers) {
  s->data = malloc(big_packets_numbers * sizeof(BIG_PACKET_SIZE));

  for (int i = 0; i < BIG_PACKET_SIZE * big_packets_numbers; i++) {
    s->data[i] = 'a';
  }
}

void getSlice(const char *sourceArray, int startIndex, int length, char *destinationArray) {
  for (int i = 0; i < length; ++i) {
    destinationArray[i] = sourceArray[startIndex + i];
  }
}

void multiplexer(source *source_0, source *source_1, char *outputBuffer) {
  int bit = bitToggle(source_0->ratio, source_1->ratio);

  source *s = bit == 0 ? source_0 : source_1;
  small_packet sp = { .id = s->id };

  getSlice(s->data, (s->consumed_packets * (PACKET_SIZE - 1)), PACKET_SIZE - 1, sp.data);
  s->consumed_packets += 1;

  snprintf(outputBuffer, MAX_BUFFER, "%d%.*s", sp.id, PACKET_SIZE, sp.data);
}

void trim(char *str) {
  int start = 0, end = strlen(str) - 1;

  while (isspace((unsigned char)str[start])) {
    start++;
  }

  while ((end > start) && isspace((unsigned char)str[end])) {
    end--;
  }

  str[end + 1] = '\0';
}

int main()
{
  srand(time(NULL));

  source source_0 = { .id = 0, .consumed_packets = 0, .ratio = 2.0 };
  source source_1 = { .id = 1, .consumed_packets = 0, .ratio = 4.0 };

  initDataPackages(&source_0, SOURCE_0_PACKAGES);
  initDataPackages(&source_1, SOURCE_1_PACKAGES);

  char outputBuffer[MAX_BUFFER];

  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  int enable = 1;
  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(PORT);

  if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(serverSocket, 3) < 0) {
    perror("Listen failed");
    exit(EXIT_FAILURE);
  }

  printf("Waiting for connections...\n");

  while (1)
  {
    int clientSocket = accept(serverSocket, NULL, NULL);

    if (clientSocket < 0) {
      perror("Accept failed");
      exit(EXIT_FAILURE);
    }

    char command[MAX_BUFFER];
    ssize_t bytesRead;

    while ((bytesRead = recv(clientSocket, command, sizeof(command), 0)) > 0) {
      command[bytesRead] = '\0';
      trim(command);
      if (strcasecmp(command, "fetch") == 0) {
        multiplexer(&source_0, &source_1, outputBuffer);
        send(clientSocket, outputBuffer, strlen(outputBuffer), 0);
      } else if (strcasecmp(command, "quit") == 0) {
        send(clientSocket, "Bye\n", strlen("Bye\n"), 0);
        close(clientSocket);
      }
      else if (strcasecmp(command, "help") == 0) {
        strcpy(outputBuffer, "Commands: fetch | quit | help\n");
        send(clientSocket, outputBuffer, strlen(outputBuffer), 0);
      } else {
        send(clientSocket, "Invalid command\n", strlen("Invalid command\n"), 0);
      }
    }
  }

  free(source_0.data);
  free(source_1.data);
  close(serverSocket);

  return 0;
}