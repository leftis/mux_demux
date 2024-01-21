#include <stdio.h>

#define MAXSIZE 64

typedef struct {
  int front;
  int rear;
  int size;
  char data[MAXSIZE];
} queue;

int isempty(queue *q)
{
  return q->size < 0;
}

int isfull(queue *q)
{
  return q->size == MAXSIZE;
}

void enqueue(queue *q, char value)
{
  if (q->size < MAXSIZE)
  {
    if (q->size < 0)
    {
      q->data[0] = value;
      q->front = q->rear = 0;
      q->size = 1;
    }
    else if (q->rear == MAXSIZE - 1)
    {
      q->data[0] = value;
      q->rear = 0;
      q->size++;
    }
    else
    {
      q->data[q->rear + 1] = value;
      q->rear++;
      q->size++;
    }
  }
  else
  {
    printf("Queue is full\n");
  }
}

char dequeue(queue *q)
{
  if (q->size < 0)
  {
    printf("Queue is empty\n");
  }
  else
  {
    q->size--;
    q->front++;
  }
}

char front(queue *q)
{
  return q->data[q->front];
}

void display(queue *q)
{
  int i;
  if (q->rear >= q->front)
  {
    for (i = q->front; i <= q->rear; i++)
    {
      printf("%c\n", q->data[i]);
    }
  }
  else
  {
    for (i = q->front; i < MAXSIZE; i++)
    {
      printf("%c\n", q->data[i]);
    }
    for (i = 0; i <= q->rear; i++)
    {
      printf("%c\n", q->data[i]);
    }
  }
}

void reset(queue *q) {
  q->size = -1;
  q->front = -1;
  q->rear = -1;

  memset(&q->data[0], 0, sizeof(q->data));
}
