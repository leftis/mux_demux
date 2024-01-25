#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ncurses.h>
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

  vector_setup(&reg0, 64, sizeof(char));
  vector_setup(&reg1, 64, sizeof(char));

  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  char o0[4096] = {'\0'};
  int o0pos=0;
  char o1[4096] = {'\0'};
  int o1pos=0;

  initscr();
  cbreak();
  noecho();

  init_pair(1, COLOR_WHITE, COLOR_BLACK);

  WINDOW *win1 = newwin(LINES / 2 - 2, COLS - 2, 1, 1);
  WINDOW *win2 = newwin(LINES / 2 - 1, COLS - 2, LINES / 2, 1);

  wrefresh(win1);
  wrefresh(win2);

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
    const char *fetchCommand = "fetch";
    if (send(clientSocket, fetchCommand, strlen(fetchCommand), 0) == -1)
    {
      perror("Send failed");
      exit(EXIT_FAILURE);
    }

    char lineBuffer[64];
    ssize_t bytesRead = recv(clientSocket, lineBuffer, 65, 0);

    if (bytesRead == -1)
    {
      perror("Receive failed");
      exit(EXIT_FAILURE);
    }
    else if (bytesRead == 0)
    {
      perror("here");
      break;
    }

    lineBuffer[bytesRead] = '\0';

    demux(lineBuffer, &reg0, &reg1);

    VECTOR_FOR_EACH(&reg0, i)
    {
      char s = ITERATOR_GET_AS(char, &i);
      o0[o0pos] = s;
      o0pos+=1;
    }

    mvwprintw(win1, 0, 0, "O0:%s", o0);

    VECTOR_FOR_EACH(&reg1, d)
    {
      char s = ITERATOR_GET_AS(char, &d);
      o1[o1pos] = s;
      o1pos += 1;
    }

    mvwprintw(win2, 0, 0, "O1:%s", o1);

    printf("\n");
    vector_clear(&reg0);
    vector_clear(&reg1);
    wrefresh(win1);
    wrefresh(win2);
    usleep(10000);
  }

  close(clientSocket);
  endwin();
  return 0;
}

// packet: 64 bytes of muxed data
void demux(char *packet, Vector *reg0, Vector *reg1) {
  int i = 0;
  while(i <= 62) {
    if (i % 2 == 0) {
      Vector *vp = *(packet[i] == '0' ? &reg0 : &reg1);
      vector_push_back(vp, &packet[i + 1]);
    }
    i+=1;
  }
}