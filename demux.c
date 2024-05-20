#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>

#include "vector.h"
#include "vec.c"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345

void demux(char *line, Vector *reg0, Vector *reg1);

int main()
{
  Vector reg0;
  Vector reg1;

  FILE *fPtrO0 = NULL;
  FILE *fPtrO1 = NULL;

  const char filePathO0[] = "out0.txt";
  const char filepathO1[] = "out1.txt";

  vector_setup(&reg0, 64, sizeof(char));
  vector_setup(&reg1, 64, sizeof(char));
  

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

  fPtrO0 = fopen(filePathO0, "w");
  fPtrO1 = fopen(filepathO1, "w");
    /* fopen() return NULL if unable to open file in given mode. */
  if (fPtrO0 == NULL)
  {
      /* Unable to open file hence exit */
      printf("\nUnable to open '%s' file.\n", filePathO0);
      printf("Please check whether file exists and you have write privilege.\n");
      exit(EXIT_FAILURE);
  }

  if (fPtrO1 == NULL)
  {
      /* Unable to open file hence exit */
      printf("\nUnable to open '%s' file.\n", filepathO1);
      printf("Please check whether file exists and you have write privilege.\n");
      exit(EXIT_FAILURE);
  }

  // Loop for fetching data

  while (1)
  {
    const char *fetchCommand = "fetch";
    if (send(clientSocket, fetchCommand, strlen(fetchCommand), 0) == -1)
    {
      perror("Send failed");
      exit(EXIT_FAILURE);
    }

    char lineBuffer[64];
    ssize_t bytesRead = recv(clientSocket, lineBuffer, 64, 0);

    if (bytesRead == -1)
    {
      perror("Receive failed");
      exit(EXIT_FAILURE);
    }
    else if (bytesRead == 0)
    {
      printf("Done");
      break;
    }

    lineBuffer[bytesRead] = '\0';
    demux(lineBuffer, &reg0, &reg1);

    VECTOR_FOR_EACH(&reg0, i)
    {
      char s0 = ITERATOR_GET_AS(char, &i);
      printf("0%c", s0);
      fputc(s0, fPtrO0);
    }

    VECTOR_FOR_EACH(&reg1, d)
    {
      char s1 = ITERATOR_GET_AS(char, &d);
      printf("1%c", s1);
      fputc(s1, fPtrO1);
    }
    vector_clear(&reg0);
    vector_clear(&reg1);
  }

  fclose(fPtrO0);
  fclose(fPtrO1);
  close(clientSocket);

  return 0;
}

// packet: 64 bytes of muxed data
void demux(char *packet, Vector *reg0, Vector *reg1) {
  int i = 0;
  // For packets total length 64 bytes
  // loop and assign the data byte to the correct registry
  while(i <= 62) {
    // the header byte is the first byte every two bytes
    // [data][0|1]
    if (i % 2 == 0) {
      Vector *vp = *(packet[i] == '0' ? &reg0 : &reg1);
      vector_push_back(vp, &packet[i + 1]);
    }
    i+=1;
  }
}
