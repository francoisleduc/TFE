/* ========================================================================= *
 * LinkedList interface:
 * Note that the linked list structure
 * is not opaque so that you can manipulate it directly.
 * ========================================================================= */

#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <stddef.h>
#include <stdbool.h>


typedef struct llnode_t {
    const void* value;
    struct llnode_t* next;

} LLNode;

typedef struct linkedlist_t {
    size_t size;
    LLNode* head;
    LLNode* last;
}LinkedList;




/**
 * \fn LinkedList* newLinkedList(void)
 * \brief Creates a new single linked list. Must later be deleted by calling freeLinkedList().
 *
 *
 *
 * \return A pointer to the new linkedlist or NULL in case of error.
 *
 */
LinkedList* newLinkedList(void);






/**
 * \fn void freeLinkedList(LinkedList* ll, bool freeContent)
 * \brief Frees the allocated memory of the given LinkedList.
 *
 * \param ll A valid pointer to a LinkedList object
 * \param freeContent Whether to free the content as well.
 *
 * \return Void.
 *
 */
void freeLinkedList(LinkedList* ll, bool freeContent);



/**
 * \fn size_t sizeOfLinkedList(const LinkedList* ll)
 * \brief Counts the number of elements stored in the given LinkedList.
 *
 * \param ll A valid pointer to a LinkedList object
 *
 * \return The amount of elements stored in linked list.
 *
 */
size_t sizeOfLinkedList(const LinkedList* ll);






/**
 * \fn bool insertInLinkedList(LinkedList* ll, const void* value)
 * \brief Inserts a new element in the linked list.
 *
 * \param ll A valid pointer to a LinkedList object
 * \param value The value to store
 *
 * \return A boolean equal to true if the new element was successfully
 *              inserted, false otherwise (error).
 *
 */
bool insertInLinkedList(LinkedList* ll, const void* value);




/**
 * \fn LinkedList* filterLinkedList(LinkedList* ll,  bool keepIt_fn_t(const void*))
 * \brief Return a new linked list containing (the pointer to) the element of the
 *         original linked list for which the keepIt predicate is true.
 *
 * \param ll A valid pointer to a LinkedList object
 * \param keepIt A predicate operating on the values of the linked list
 *
 * \return A (possibly empty) linked list or NULL in case of error.
 *
 */

LinkedList* filterLinkedList(LinkedList* ll,  bool keepIt_fn_t(const void*));

/*
 * USAGE (example for strings)
 * bool startByH(const void* string)
 * {
 *     const char* string_ = string;
 *     return string_[0] == 'h';
 * }
 * ...
 * LinkedList* filtered = filterLinkedList(ll, &startByH);
*/

#endif // !_LINKED_LIST_H_