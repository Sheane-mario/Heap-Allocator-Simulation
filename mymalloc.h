#include <stdio.h>
#define MEMORY_SIZE 25000
#pragma once

struct freeBlock
{
    size_t size;
    char *startingAddr;
    struct freeBlock *prev;
    struct freeBlock *next;
};

void *MyMalloc(size_t ALLOC_SIZE);
void MyFree(void *pMemoryBlock);
int brk(void *end_data_segment);
char *sbrk(size_t increment);
char *bestFit(size_t ALLOC_SIZE);
char *split(struct freeBlock *block, size_t ALLOC_SIZE);
