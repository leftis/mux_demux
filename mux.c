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
#include <stdbool.h>

#include "vec.c"
#include "vector.h"
#include "data.h"

#define PORT 12345
#define MAX_BUFFER 64
#define BIG_PACKET_SIZE 640
#define SOURCE_0_PACKAGES 4
#define SOURCE_1_PACKAGES 2

typedef struct
{
  char identifier;
  char *data;
  int consumed_packets;
  int remaning_packets;
  double ratio;
} source;

double randomDouble() {
  return (double)rand() / RAND_MAX;
}

source* bitToggle(source *s1, source *s2) {
  double ratio1 = s1->ratio;
  double ratio2 = s2->ratio;
  int bit = 0;

  if (s1->remaning_packets > 0 && s2->remaning_packets > 0) {
    double randomValue = randomDouble();
    if (randomValue > ratio1 / (ratio1 + ratio2)) {
      bit = 1;
    }
  } else {
    if (s2->remaning_packets > 0) {
      bit = 1;
    }
  }

  return bit == 0 ? s1 : s2;
}

void initDataPackages(source *s, int big_packets_numbers) {
  s->data = malloc(big_packets_numbers * BIG_PACKET_SIZE);
  s->remaning_packets = BIG_PACKET_SIZE * big_packets_numbers;

  for (int i = 0; i < big_packets_numbers; i++) {
    memcpy(s->data + (i * BIG_PACKET_SIZE), byteArray, BIG_PACKET_SIZE);
  }
}

void multiplexer(source *s1, source *s2, Vector *line, int space) {
  int s1_count = s1->remaning_packets;
  int s2_count = s2->remaning_packets;

  bool packets = (s1_count > 0 || s2_count > 0);

  if (s1_count == 0 && s2_count == 0) {
    return;
  }
  int i=0;
  while (i < space && packets)
  {
    source *s = bitToggle(s1, s2);
    vector_push_back(line, &s->identifier);
    vector_push_back(line, &s->data[s->consumed_packets]);

    s->consumed_packets += 1;
    s->remaning_packets -= 1;
    i++;
  }
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
  Vector line;
  vector_setup(&line, 64, sizeof(char));

  source source_0 = {
      .identifier = '0',
      .consumed_packets = 0,
      .ratio = 2.0};

  source source_1 = {
      .identifier = '1',
      .consumed_packets = 0,
      .ratio = 1.0};

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
        multiplexer(&source_0, &source_1, &line, 32);

        if (!vector_is_empty(&line)) {
          // send the packet
          send(clientSocket, line.data, strlen(line.data), 0);

          VECTOR_FOR_EACH(&line, i)
          {
            vector_pop_back(&line);
          }
        } else {
          const char *data = "";
          size_t dataSize = strlen(data);
          send(clientSocket, data, dataSize, 0);
        }
      } else if (strcasecmp(command, "quit") == 0) {
        send(clientSocket, "Bye\n", strlen("Bye\n"), 0);
        close(clientSocket);
      } else if (strcasecmp(command, "help") == 0) {
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
