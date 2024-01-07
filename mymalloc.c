#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\The mechanism that i suppose to follow is as follows\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
Heap can dynamically grow in size. programbreak is a pointer marked at the end of the heap. Heap can grow untill it comes to the end of memory. In case it grows beyond the memory, the program should break.

mechanism that i wish to implement in MyMalloc().
When a call to MyMalloc() is happen, it first scans the freelist(a list maintained to to keep the released memory blocks), in order to find a memory block which fits in to the required size or larger than that. The scan is named as bestFit(), which returns a memory address if it find one or returns null otherwise. In case the found memory block is larger than the required size, it should be split so that it fits perfectly. split() will do that for us.

Wnen we deallocate memory of a block, the program break would not reduce. Instead that deallocated block of memory is added to the freelist. Freelist is a doubly linked list. Each block in the freelist has the following three fields, the size of the block, pointer to the previous block, pointer to the next block, pointer to the starting memory address.

Each block returned by MyMalloc() should consist of the size of the block, because it is essential when freeing how much to free.

The memory allocator i've designed doesn't know how to manage potential errors, and does not optimize the heap storage by merging adjacent free blocks.
****************I suppose to implement those in near future.************
****Thank You******
****sheane mario silva*******
****Index No : 22001905******
*/

/* At the start of a process, program break, which is the end of the heap is initialized such that it points to first address in array memory .*/
char memory[MEMORY_SIZE];
char *programBreak = memory;

struct freeBlock *freeList = (void *)memory;

void *MyMalloc(size_t ALLOC_SIZE)
{
    if (freeList != NULL)
    {
        char *startingAddr = bestFit(ALLOC_SIZE);
        /*bestfit checks whether there is any block in the freelist which can be fit into our requirenmet,*/
        if (startingAddr == NULL) /* When there is no any block that fits */
        {
            /* Unable to find a memory block that fits into ALLOC_SIZE or greater from the free list. So we must call sbrk() to increase the program break and allocate more memory to the heap. */

            return sbrk(ALLOC_SIZE);
        }
        /* Return the starting address of the memory block. */
        return startingAddr;
    }
}
/* When a void pointer is passed this function will deallocate the memory and add that to the freelist, but does not decrease the program break */
void MyFree(void *pMemoryBlock)
{
    if (pMemoryBlock == NULL)
    {
        fprintf(stderr, "Cannot free NULL pointer\n");
        return;
    }

    struct freeBlock *newBlock = (struct freeBlock *)pMemoryBlock;

    // Update the size of the block to mark it as free
    newBlock->size = sizeof(struct freeBlock);

    // Add this block back to the free list
    if (freeList == NULL)
    {
        /* The case when the fist memory block is added to the free list */
        freeList = newBlock;
        freeList->next = NULL;
        freeList->prev = NULL;
    }
    else
    {
        /* Add that memory block to the start of the freelist. */
        newBlock->next = freeList;
        freeList->prev = newBlock;
        newBlock->prev = NULL;
        freeList = newBlock;
    }
}

/*
The brk() function call sets the program break to the location specified by
end_data_segment.
*/
int brk(void *end_data_segment)
{
    if ((char *)end_data_segment < &memory[0])
    {
        fprintf(stderr, "SIGSEGV: %s\n", "Segmentation Violation, Attempting to set program break below it's initial value.");
        exit(EXIT_FAILURE);
    }
    if ((char *)end_data_segment > &memory[MEMORY_SIZE])
    {
        fprintf(stderr, "SIGSEGV: %s\n", "Segmentation Violation, Attempting to set program break beyond the max heap size.");
        exit(EXIT_FAILURE);
    }
    programBreak = end_data_segment;
    return 0; // Indicating success.
}

/* This function returns the previous address of the program break on sucess. if we have increased the program
break, then the return value is a pointer to the start of the newly allocated block of
memory.*/
char *sbrk(size_t increment)
{
    char *prevProgramBreak = programBreak;
    programBreak += increment;
    // printf("%p:: %p\n", prevProgramBreak, programBreak);
    if (programBreak > &memory[MEMORY_SIZE])
    {
        fprintf(stderr, "SIGSEGV: %s\n", "Segmentation Violation, Heap size overflow.");
        exit(EXIT_FAILURE);
    }
    return prevProgramBreak;
}

/*
This function performs a scan through the free list and check whether it can find a memory block larger than or equal to the
size specified by the ALLOC_SIZE.
*/
char *bestFit(size_t ALLOC_SIZE)
{
    struct freeBlock *temp = freeList;
    while (temp != NULL)
    {
        if (temp->size == ALLOC_SIZE)
        {
            return temp->startingAddr;
        }
        if (temp->size > ALLOC_SIZE)
        {
            /* Handle the case when we have to split the memory block in the freelist. split the block and add the remaining part back into the freelist and return the required memory block's address */
            char *startingAddrOfBlock = split(temp, ALLOC_SIZE);
            return startingAddrOfBlock;
        }
        temp = temp->next;
    }
    return NULL;
}

/*
Split a block of memory to the specified size and place the remaining block in the free list.
*/
char *split(struct freeBlock *block, size_t ALLOC_SIZE)
{
    size_t remainingSize = block->size - ALLOC_SIZE;
    block->size = ALLOC_SIZE;

    struct freeBlock *newBlock = (struct freeBlock *)(block->startingAddr + ALLOC_SIZE);
    newBlock->size = remainingSize;
    newBlock->startingAddr = block->startingAddr + ALLOC_SIZE;
    if (block->next != NULL)
    {
        newBlock->next = block->next;
    }
    else
    {
        newBlock->next = NULL;
    }
    if (block->prev != NULL)
    {
        newBlock->prev = block->prev;
    }
    else
    {
        newBlock->prev = NULL;
    }

    if (block->next != NULL)
    {
        block->next->prev = newBlock;
    }
    if (block->prev != NULL)
    {
        block->prev->next = newBlock;
    }

    return block->startingAddr;
}