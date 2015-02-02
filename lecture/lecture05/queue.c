#include <stdlib.h> /* for malloc(), free(), definition of NULL */
#include <stdio.h> 

typedef struct queue_node {
  struct queue_node *next;
  int value;
} queue_node;

typedef struct queue {
  queue_node *head, *tail;
  int size;
} queue;

void queue_add(queue *q, int val)
{
  queue_node *qn;

  /* sizeof operator takes a type name or a variable */
  /* sizeof (queue_node) */
  if (!(qn = malloc(sizeof (*qn)))) {
    /* malloc() can fail (more on this later).  You need to check it. *
     * In this case, perror() will print a message from the system,   *
     * tersely explaining why it failed, and exit() will terminate    *
     * the program.                                                   */
    perror("malloc");
    exit(1);
  }
  qn->next = NULL;
  qn->value = val;

  if (q->size == 0) {
    q->head = q->tail = qn;
  } else {
    q->tail->next = qn;
    q->tail = qn;
  }
    q->size++;  
}

int queue_remove(queue *q, int *value)
{
  queue_node *tmp;

  if (q->size == 0) {
    return -1;
  }

  q->size--;
  *value = q->head->value;
  tmp = q->head;
  q->head = q->head->next;
  free(tmp);

  if (!q->size) {
    q->head = q->tail = NULL;
  }

  return 0;
}

void queue_init(queue *q)
{
  /* Arrow operator is used to reach struct fields from pointers to structs */
  q->size = 0;
  q->head = q->tail = NULL;
}

int main(int argc, char *argv[])
{
  int val;
  queue q;

  queue_init(&q);
  queue_add(&q, 1);
  queue_add(&q, 2);
  queue_add(&q, 3);
  queue_add(&q, 4);
  queue_add(&q, 5);

  printf("The queue holds %d elements.\n", q.size);
  while(!queue_remove(&q, &val)) {
    printf("%d\n", val);
  }
  printf("The queue holds %d elements.\n", q.size);

  return 0;
}