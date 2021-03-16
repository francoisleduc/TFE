#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "log.h"
#include <stddef.h>
typedef struct Element_t Element;
typedef struct LinkedList_t List;

/**
 * \fn void* get_element_value(Element* el)
 * \brief Gets value accessor.
 *
 * \param el The element structure.
 *
 * \return The content.
 *
 */
void* get_element_value(Element* el);

/**
 * \fn void* get_element_previous(Element* el)
 * \brief Gets previous accessor.
 *
 * \param el The element structure.
 *
 * \return The previous element.
 *
 */
Element* get_element_previous(Element* el);

/**
 * \fn void* get_element_next(Element* el)
 * \brief Gets next accessor.
 *
 * \param el The element structure.
 *
 * \return The next element.
 *
 */
Element* get_element_next(Element* el);

/**
 * \fn size_t get_list_size(List* list)
 * \brief Gets size accessor.
 *
 * \param list The list structure.
 *
 * \return Size of the list.
 *
 */
size_t get_list_size(List* list);


/**
 * \fn size_t get_list_head(List* list)
 * \brief Gets first element accessor.
 *
 * \param list The list structure.
 *
 * \return The first element.
 *
 */
Element* get_list_head(List* list);

/**
 * \fn size_t get_list_tail(List* list)
 * \brief Gets last element accessor.
 *
 * \param list The list structure.
 *
 * \return The last element.
 *
 */
Element* get_list_tail(List* list);

/**
 * \fn void set_element_value(Element* el, void* value)
 * \brief Sets value accessor.
 *
 * \param el The element structure.
 * \param value The element content.
 *
 * \return Void.
 *
 */
void set_element_value(Element* el, void* value);

/**
 * \fn void set_element_previous(Element* el, Element* prev)
 * \brief Sets previous accessor.
 *
 * \param el The element structure.
 * \param prev The previous element structure.
 *
 * \return Void.
 *
 */
void set_element_previous(Element* el, Element* prev);

/**
 * \fn void set_element_next(Element* el, Element* next)
 * \brief Sets next element accessor.
 *
 * \param el The element structure.
 * \param next The next element structure.
 *
 * \return Void.
 *
 */
void set_element_next(Element* el, Element* next);

/**
 * \fn void set_head_list(List* list, Element* el)
 * \brief Assigns new head element pointer.
 *
 * \param list The list structure.
 * \param el The element structure to be assigned to head field.
 *
 * \return Void.
 *
 */
void set_head_list(List* list, Element* el);

/**
 * \fn Element* new_element(Element* prev, Element* next, void* value)
 * \brief Instantiates an element structure.
 *
 * \param prev The previous element structure.
 * \param next The next element structure.
 * \param value The element content.
 *
 * \return The element structure, or NULL if error.
 *
 */
Element* new_element(Element* prev, Element* next, void* value);


/**
 * \fn void free_element(Element *el)
 * \brief Frees an element structure.
 *
 * \param el The element structure.
 *
 * \return Void.
 *
 */
void free_element(Element *el);

/**
 * \fn List* new_list(void)
 * \brief Instantiates a list structure.
 *
 * \return The list structure, or NULL if error.
 *
 */
List* new_list(void);


/**
 * \fn void free_list(List* list)
 * \brief Frees a list structure.
 *
 * \param list The list structure.
 *
 * \return Void.
 *
 */
void free_list(List* list);

/**
void insert_in_list(List* list, void* value);
 * \brief Inserts a value in the list.
 *
 * \param list The list structure.
 * \param value The content.
 *
 * \return Void.
 *
 */
void insert_in_list(List* list, void* value);

/**
 * \fn void delete_in_list(List *list, Element* el)
 * \brief Deletes the element in the list.
 *
 * \param list The list structure.
 * \param el The element structure.
 *
 * \return Void.
 *
 */
void delete_in_list(List *list, Element* el);

#endif
