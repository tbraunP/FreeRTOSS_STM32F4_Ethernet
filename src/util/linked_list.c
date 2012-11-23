/*
 * linked_list.c
 *
 *  Created on: 07.04.2012
 *      Author: pyro
 */
#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>

NODE_T* list_create(void *data) {
	NODE_T* node;
	if (!(node = (NODE_T*) malloc(sizeof(NODE_T))))
		return NULL;
	node->data = data;
	node->next = NULL;
	return node;
}

NODE_T* list_append(NODE_T volatile *node, void *data) {
	NODE_T volatile* firstNode = node;
	NODE_T volatile* newnode = list_create(data);
	while (firstNode->next != NULL) {
		firstNode = firstNode->next;
	}
	firstNode->next = newnode;
	return (NODE_T*) node;
}

int list_countElements(NODE_T volatile *node){
	int i=0;
	NODE_T volatile* firstNode = node;
	while (firstNode->next != NULL) {
		firstNode = firstNode->next;
		i++;
	}
	return i;
}
