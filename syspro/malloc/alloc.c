/**
* Malloc Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const size_t WORD_SIZE = sizeof(size_t);

typedef struct front_tag {
    size_t size;
    size_t isalloc;
    struct front_tag *next_free;
    struct front_tag *prev_free;
} front_tag;

typedef struct back_tag {
    front_tag *my_front_tag;
} back_tag;

static front_tag *all_head;
static front_tag *free_head;
static void *heap_top;


/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
    // implement calloc!
    void *ptr = malloc(num * size);
    if (ptr == NULL) {
        return (void *)NULL;
    }

    front_tag *front = (front_tag *)(ptr - sizeof(front_tag));

    void *mem_reset = (void *)(ptr + 1);
    memset(mem_reset, 0, front->size);
    mem_reset = NULL;
    return ptr;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {
    // implement malloc!
    size_t rounded_total_size = (size + sizeof(front_tag) + sizeof(back_tag) + (WORD_SIZE - 1)) / WORD_SIZE * WORD_SIZE;
    //size_t rounded_total_size = (size + sizeof(front_tag) + sizeof(back_tag));
    size_t final_size = rounded_total_size - sizeof(front_tag) - sizeof(back_tag);

    // when free head has nothing
    if (free_head == NULL || (size < 24 && size != sizeof(int))) {
        front_tag *overhead = (front_tag *)sbrk(rounded_total_size);
        // when sbrk fails
        if (overhead == (void *)-1) {
            return (void *)NULL;
        }

        // point all_head to the front
        if (all_head == NULL) {
            all_head = overhead;
        }
        heap_top = (void *)overhead + rounded_total_size;

        overhead->size = final_size;
        overhead->isalloc = 1;
        overhead->prev_free = NULL;
        overhead->next_free = NULL;

        // get end tag...
        back_tag *end = (back_tag *)((void *)overhead + sizeof(front_tag) + final_size);
        end->my_front_tag = overhead;

        return (void *)(overhead + 1);
    }

    // when free head has something...
    front_tag *traverse = free_head;
    while (traverse != NULL) {
        size_t curr_size = traverse->size;
        // cannot fit in... go to the next free slot...
        if (curr_size < final_size) {
            traverse = traverse->next_free;
        } else {
            // otherwise...we fit that in and return...

            // if we cannot split... just use the slot, get rid of the free list!
            if (curr_size <= rounded_total_size) {
                traverse->size = curr_size;
                traverse->isalloc = 1;
                // when the traverse is at the top of the free list.
                if (free_head == traverse) {
                    free_head = traverse->next_free;
                    if (traverse->next_free != NULL) {
                        traverse->next_free->prev_free = NULL;
                    }
                } else {
                    if (traverse->next_free != NULL) {
                        traverse->next_free->prev_free = traverse->prev_free;
                    }
                    if (traverse->prev_free != NULL) {
                        traverse->prev_free->next_free = traverse->next_free;
                    }
                }
                back_tag *trav_end = (back_tag *)((void *)traverse + sizeof(front_tag) + traverse->size);
                trav_end->my_front_tag = traverse;
            } else {
                // yes we can split!!
                traverse->size = final_size;
                traverse->isalloc = 1;

                // when the traverse is at the top of the free list.
                if (free_head == traverse) {
                    free_head = traverse->next_free;
                    if (traverse->next_free != NULL) {
                        traverse->next_free->prev_free = NULL;
                    }
                } else {
                    if (traverse->next_free != NULL) {
                        traverse->next_free->prev_free = traverse->prev_free;
                    }
                    if (traverse->prev_free != NULL) {
                        traverse->prev_free->next_free = traverse->next_free;
                    }
                }

                back_tag *trav_end = (back_tag *)((void *)traverse + sizeof(front_tag) + traverse->size);
                trav_end->my_front_tag = traverse;

                // we have one more block!
                front_tag *next_block = (front_tag *)(trav_end + 1);
                next_block->size = curr_size - rounded_total_size;
                next_block->isalloc = 1;
                next_block->prev_free = NULL;
                next_block->next_free = NULL;

                back_tag *next_block_end = (back_tag *)((void *)next_block + sizeof(front_tag) + next_block->size);
                next_block_end->my_front_tag = next_block;

                free((void *)(next_block + 1));
            }
            return (void *)(traverse + 1);
        }
    }

    // if we get here, it means that no more available free slots, so we just sbrk...
    front_tag *overhead = (front_tag *)sbrk(rounded_total_size);
    // when sbrk fails
    if (overhead == (void *)-1) {
        return (void *)NULL;
    }
    heap_top = (void *)overhead + rounded_total_size;

    overhead->size = final_size;
    overhead->isalloc = 1;
    overhead->prev_free = NULL;
    overhead->next_free = NULL;

    // get end tag...
    back_tag *end = (back_tag *)((void *)overhead + sizeof(front_tag) + final_size);
    end->my_front_tag = overhead;

    return (void *)(overhead + 1);

}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {
    // implement free!
    if (ptr == NULL) {
        return;
    }

    // free the curr block first
    front_tag *curr_front = (front_tag *)ptr - 1;
    curr_front->isalloc = 0;
    curr_front->prev_free = NULL;
    // NOTE: go to the front of the list!
    if (free_head != NULL) {
        free_head->prev_free = curr_front;
    }
    curr_front->next_free = free_head;
    free_head = curr_front;

    back_tag *curr_end = (back_tag *)((void *)curr_front + sizeof(front_tag) + curr_front->size);
    curr_end->my_front_tag = curr_front;

    // check left hand side...
    // it is not in the frontmost...
    if (all_head != curr_front) {
        back_tag *left_end = ((back_tag *)curr_front) - 1;
        // we only do stuff when the left hand block is deallocated...
        if (left_end->my_front_tag->isalloc == 0) {

            front_tag *left_front = left_end->my_front_tag;
            left_front->size += (size_t)(sizeof(front_tag) + sizeof(back_tag) + curr_front->size);
            left_front->isalloc = 0;

            // curr_front->next_free cannot be null because in this case, there
            //   are at least one block in the free lise due to its left block
            //   is deallocated.
            if (curr_front->next_free != NULL) {
                curr_front->next_free->prev_free = NULL;
            }

            free_head = curr_front->next_free;
            curr_front->next_free = NULL;

            if (free_head != left_front) {
                if (left_front->prev_free != NULL) {
                    left_front->prev_free->next_free = left_front->next_free;
                }
                if (left_front->next_free != NULL) {
                    left_front->next_free->prev_free = left_front->prev_free;
                }

                free_head->prev_free = left_front;
                left_front->next_free = free_head;
                free_head = left_front;
            }

            curr_end->my_front_tag = left_front;

            // we can now treat left_front as curr_front, because we have merged
            //   them together.
            curr_front = left_front;

            left_end->my_front_tag = NULL;

        }
    }

    // check right hand size...
    // it is not the end.
    front_tag *right_front = (front_tag *)(curr_end + 1);
    if ((void *)right_front != heap_top) {
        if (right_front->isalloc == 0) {
            curr_front->size += (size_t)(sizeof(front_tag) + sizeof(back_tag) + right_front->size);
            // it is impossible that free_head is pointing to this block...
            //  and right_front->prev_free must not be null...
            if (right_front->prev_free != NULL) {
                right_front->prev_free->next_free = right_front->next_free;
            }
            if (right_front->next_free != NULL) {
                right_front->next_free->prev_free = right_front->prev_free;
            }
            right_front->prev_free = NULL;
            right_front->next_free = NULL;

            back_tag *right_end = (back_tag *)((void *)right_front + sizeof(front_tag) + right_front->size);
            right_end->my_front_tag = curr_front;
        }
    }
}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
    // implement realloc!

    // In the case that size is 0
    if (size == 0) {
        free(ptr);
        return (void *)NULL;
    }

    // In the case that pointer is NULL
    if (ptr == NULL) {
        return malloc(size);
    }

    size_t rounded_total_size = (size + sizeof(front_tag) + sizeof(back_tag) + (WORD_SIZE - 1)) / WORD_SIZE * WORD_SIZE;
    //size_t rounded_total_size = (size + sizeof(front_tag) + sizeof(back_tag));    
    size_t final_size = rounded_total_size - sizeof(front_tag) - sizeof(back_tag);

    front_tag *curr_front = (front_tag *)ptr - 1;
    size_t curr_size = curr_front->size;

    // When the size requested is the same as the slot, do nothing...
    if (curr_size == final_size) {
        return ptr;
    }

    // When we need to decrease the size...
    if (curr_size > final_size) {
        // There are two possiblities:
        // When we cannot split... do nothing...
        // TODO
        if (curr_size <= rounded_total_size * 3) {
            return ptr;
        }

        // when we should split...
        curr_front->size = final_size;

        back_tag *curr_end = (back_tag *)((void *)curr_front + sizeof(front_tag) + curr_front->size);
        curr_end->my_front_tag = curr_front;

        front_tag *next_front = (front_tag *)(curr_end + 1);
        next_front->size = curr_size - rounded_total_size;
        next_front->isalloc = 1;
        next_front->prev_free = NULL;
        next_front->next_free = NULL;

        back_tag *next_end = (back_tag *)((void *)next_front + sizeof(front_tag) + next_front->size);
        next_end->my_front_tag = next_front;
        free((void *)(next_front + 1));
        return ptr;
    }

    // The last condition: we need to increase the length of the block... OMG!!!

    // we can check if we can directly increase the block rightward...
    // TODO: this part of code is really buggy... need to be revised..
    // front_tag *right_front = (front_tag *)((void *)curr_front + curr_size + sizeof(front_tag) + sizeof(back_tag));
    // if (right_front != heap_top && right_front->isalloc == 0 && final_size <= sizeof(front_tag) + curr_size + sizeof(back_tag) + right_front->size) {
    //     curr_front->size += (sizeof(front_tag) + sizeof(back_tag) + right_front->size);
    //     if (free_head == right_front) {
    //         free_head = right_front->next_free;
    //     }
    //     if (right_front->next_free != NULL) {
    //         right_front->next_free->prev_free = right_front->prev_free;
    //     }
    //     if (right_front->prev_free != NULL) {
    //         right_front->prev_free->next_free = right_front->next_free;
    //     }
    //     back_tag *right_back = (back_tag *)((void *)right_front + sizeof(front_tag) + right_front->size);
    //     right_back->my_front_tag = curr_front;
    //
    //     return ptr;
    // }

    // if you get here...that means you have to free and memcpy and malloc...so sad...
    void *new_usable = malloc(final_size);
    if (new_usable == NULL) {
        return NULL;
    }

    memcpy(new_usable, ptr, curr_size);
    free(ptr);

    return new_usable;
}
