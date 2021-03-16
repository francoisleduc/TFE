/**
 * \file linkedlist.c
 * \brief A linked list implementation.
 * \version 0.1
 *
 * A linear collection of data elements.
 *
 */

#include <stddef.h>
#include <stdlib.h>
#include "linkedlist.h"

/* ========================================================================= *
 *                             STRUCTURES
 * ========================================================================= */

/**
 * \struct Element_t
 * \brief Holds content and neighbours pointer.
 *
 */
struct Element_t 
{
    void* value; /*!< The content. */
    Element* prev; /*!< Previous element. */
    Element* next; /*!< Next element. */
};

/**
 * \struct LinkedList_t
 * \brief Represents a sequence of elements.
 *
 */
struct LinkedList_t 
{
    size_t size; /*!< Size of the sequence. */
    Element* head; /*!< The first element. */
    Element* tail; /*!< The last element. */
};

/* ========================================================================= *
 *                             GET ACCESSORS
 * ========================================================================= */

void* get_element_value(Element* el)
{
    if (el == NULL)
    {
        log_error("Structure is NULL", __func__, __LINE__);
        return NULL;
    }

    return el->value;
}

Element* get_element_previous(Element* el)
{
    if (el == NULL)
    {
        log_error("Structure is NULL", __func__, __LINE__);
        return NULL;
    }

    return el->prev;
}

Element* get_element_next(Element* el)
{
    if (el == NULL)
    {
        log_error("Structure is NULL", __func__, __LINE__);
        return NULL;
    }

    return el->next;
}

size_t get_list_size(List* list)
{
    if (list == NULL)
    {
        log_error("Structure is NULL", __func__, __LINE__);
        return 0;
    }

    return list->size;
}

Element* get_list_head(List* list)
{
    if (list == NULL)
    {
        log_error("Structure is NULL", __func__, __LINE__);
        return NULL;
    }

    return list->head;
}

Element* get_list_tail(List* list)
{
    if (list == NULL)
    {
        log_error("Structure is NULL", __func__, __LINE__);
        return NULL;
    }
    
    return list->tail;
}

/* ========================================================================= *
 *                             SET ACCESSORS
 * ========================================================================= */

void set_element_value(Element* el, void* value)
{
    if (el == NULL)
    {
        log_error("Structure is NULL", __func__, __LINE__);
        return;
    }

    el->value = value;
}

void set_element_previous(Element* el, Element* prev)
{
    if (el == NULL || prev == NULL)
    {
        log_error("Structure is NULL", __func__, __LINE__);
        return;
    }

    el->prev = prev;
}

void set_element_next(Element* el, Element* next)
{
    if (el == NULL || next == NULL)
    {
        log_error("Structure is NULL", __func__, __LINE__);
        return;
    }

    el->next = next;
}


void set_head_list(List* list, Element* el)
{
    if(list == NULL)
    {
        log_error("Structure is NULL", __func__, __LINE__);
        return;
    }

    list->head = el;
}

/* ========================================================================= *
 *                             GENERAL FUNCTIONS
 * ========================================================================= */

Element* new_element(Element* prev, Element* next, void* value)
{
    Element *el = malloc(sizeof(Element));
    if (el == NULL)
    {
        log_fatal("Couldn't dynamically allocate", __FILE__, __func__, __LINE__);
        return NULL;   
    }

    el->prev = prev;
    el->next = next;
    el->value = value;

    return el;
}

void free_element(Element *el)
{
    if (el == NULL)
    {
        log_error("Structure is NULL", __func__, __LINE__);
        return;
    }

    free(el);
}

List* new_list(void)
{
    List* list = malloc(sizeof(List));
    if (list == NULL)
    {
        log_fatal("Couldn't dynamically allocate", __FILE__, __func__, __LINE__);
        return NULL;   
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    
    return list;
}

void free_list(List* list)
{
    if (list == NULL)
    {
        log_error("Structure is NULL", __func__, __LINE__);
        return;
    }

    Element* el = list->head;
    Element *prev = NULL;

    while(el != NULL)
    {
        prev = el;
        el = el->next;
        free_element(prev);
    }

    free(list);
}

void insert_in_list(List* list, void* value)
{
    if (list == NULL)
    {
        log_error("Structure is NULL", __func__, __LINE__);
        return;
    }
    
    Element* el = malloc(sizeof(Element));
    if (el == NULL)
    {
        log_fatal("Couldn't dynamically allocate", __FILE__, __func__, __LINE__);
        return;   
    }

    el->next = NULL;
    el->prev = NULL;
    el->value = value;

    if(list->tail == NULL)
    {
        /* First element in the list */
        list->tail = el;
        list->head = el;
    }
    else
    {
        /* At least one element in the list */
        el->prev = list->tail;
        list->tail->next = el;
        list->tail = el;
    }

    list->size++;
}

void delete_in_list(List *list, Element* el)
{
    if (el == NULL)
    {
        log_error("Structure is NULL", __func__, __LINE__);
        return;
    }

    if (el == list->head && el == list->tail)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    else if (el == list->head)
    {
        list->head = el->next;
        list->head->prev = NULL;
    }
    else if (el == list->tail)
    {
        list->tail = el->prev;
        list->tail->next = NULL;
    }
    else
    {
        /* Links previous and next element together before deletion */
        el->prev->next = el->next;
        el->next->prev = el->prev;
    }

    list->size--;
    free_element(el);
}
