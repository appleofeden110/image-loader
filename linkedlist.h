#pragma once

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

LinkedList *newLinkedList();

Node *newNode(uint8_t value);

void printLinkedList(LinkedList *ll);

void addToFront(uint8_t value, LinkedList *ll);
int deleteFromFront(LinkedList *ll);
void reverse(LinkedList *ll);
