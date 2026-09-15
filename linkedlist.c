#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
  uint8_t value;
  struct Node *next;
} Node;

typedef struct {
  Node *head;
  int len;
} LinkedList;

LinkedList *newLinkedList() {
  LinkedList *ll = malloc(sizeof(LinkedList));
  if (!ll) {
    return NULL;
  }
  ll->head = NULL;
  ll->len = 0;
  return ll;
}

Node *newNode(uint8_t value) {
  Node *node = malloc(sizeof(Node));
  if (!node)
    return NULL;
  *node = (Node){.value = value, .next = NULL};
  return node;
}

void addToFront(uint8_t value, LinkedList *ll) {
  Node *node = newNode(value);
  if (!node) {
    printf("ERR::ALLOC::NEW_LINKED_NODE - Value: %d\n", value);
    return;
  }

  node->next = ll->head;
  ll->head = node;
  ll->len++;
  return;
}

int deleteFromFront(LinkedList *ll) {
  if (!ll->head) {
    return 0;
  }

  int deleted = ll->head->value;
  ll->head = ll->head->next;
  ll->len--;

  return deleted;
}

void printLinkedList(LinkedList *ll) {
  Node *current = ll->head;
  for (int i = 0; i < ll->len; i++) {
    printf("%d -> ", current->value);
    current = current->next;
  }

  printf("NULL\n");
}

/*
 This function and design is made for learning purposes only.
 The better way would be to make a reversed linked list, that has a tail, and
 addToBack(int value) instead of addToFront - which is unintuitive both visually
 and logically. From tail you can print the actual changes start at the node 1,
 and last change in the tail but I wanted to learn how to efficently reverse a
 linked list
*/
// x >> y = x/2^y
void reverse(LinkedList *ll) {
  if (ll->len == 0 || ll->len == 1) {
    return;
  }

  Node *prev = NULL;
  Node *current = ll->head;
  Node *next = NULL;

  for (int i = 0; i < ll->len; i++) {
    next = current->next;
    current->next = prev;
    prev = current;
    current = next;
  }

  ll->head = prev;
  return;
}
