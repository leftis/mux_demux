#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ncurses.h>
#include <sys/queue.h>

#define SERVER_IP "127.0.0.1" // Replace with your server's IP address
#define SERVER_PORT 12345     // Replace with your server's port

#define PACKET_SIZE 2
#define B16 16


int main()
{
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  //
  char source_0_reg[8][8];
  char source_1_reg[8][8];

  int s1_full = 0;
  int s2_full = 0;

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

  // Initialize ncurses
  // initscr();
  // cbreak();
  // noecho();
  // keypad(stdscr, TRUE);

  // Create two windows for displaying data with dynamic buffers
  // WINDOW *win_s1 = newwin(30, 60, 1, 1);
  // WINDOW *win_s2 = newwin(30, 60, 1, 62);
  // wrefresh(win_s1);
  // wrefresh(win_s2);

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
    char responseBuffer[PACKET_SIZE];
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
    // demux(responseBuffer, source_0_reg, source_1_reg);
    // now the content is assigned registries
    // we need to start removing data out of it
    // printf()

    // if (s1_full)
    // {
    //   // we have a full registry from the multiplexer in which case we need to push those byte into our queue and empty the registry.
    //   wclear(win_s1);
    //   box(win_s1, 0, 0);
    //   mvwprintw(win_s1, 1, 1, "Source 0 Buffer:");
    //   mvwprintw(win_s1, 2, 2, "%s", s1_);
    //   wrefresh(win_s1);
    // }

    // if (s2_full)
    // {
    //   // Print and clear source_1_reg
    //   wclear(win_s2);
    //   box(win_s2, 0, 0);
    //   mvwprintw(win_s2, 1, 1, "Source 1 Buffer:");
    //   mvwprintw(win_s2, 2, 2, "%s", o2out);
    //   wrefresh(win_s2);
    // }

    usleep(100000); // 100 milliseconds
  }

  // Free dynamically allocated memory

  // Close the socket
  close(clientSocket);

  // Clean up ncurses
  // endwin();

  return 0;
}

// void demux(char *packet, char s1_reg[8][8], char *s2_reg[8][8])
// {
  // char *reg =
  //   (packet[0] == '1') ? s2_reg : (packet[0] == '0') ? s1_reg : NULL;

  // memcpy(reg, packet, PACKET_SIZE);

  // if (*reg == *s1_reg) {
  //   *s1_full = 1;
  // } else {
  //   *s2_full = 1;
  // }
// }