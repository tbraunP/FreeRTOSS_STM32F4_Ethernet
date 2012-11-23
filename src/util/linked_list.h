/*
 * linked_list.h
 *
 *  Created on: 07.04.2012
 *      Author: pyro
 */

#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * list element
 */
typedef struct NODE_T {
	void *data;
	struct NODE_T volatile* volatile next;
} NODE_T;

/**
 * Create a new linked list
 * \param data - data to be added
 */
NODE_T* list_create(void *data);

/**
 * Append an element at the end of the list
 * \param node - list of nodes
 * \param data - data to be added
 */
NODE_T* list_append(NODE_T volatile* node, void *data);

/**
 * Count the numbers of elements
 * \param node - list of nodes
 * \return number of elements
 */
int list_countElements(NODE_T volatile* node);

#ifdef __cplusplus
}
#endif
#endif /* LINKED_LIST_H_ */
