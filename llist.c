#include <stdio.h>
#include <stdlib.h>
#include "alloc.h"
#include "block_list.h"

void append_block(block_metadata** free_list, void* block_start) 
{
    block_metadata *block = block_start;
    block->block_start = block_start + HEAP_SIZE / BLOCK_SIZE * sizeof(block_metadata);
    block->size = BLOCK_SIZE;
    block->alloc_id = 0;
    block->next = NULL;

    if (*free_list == NULL) {
        block->prev = NULL;
        *free_list = block; 
    } else {
        block_metadata *current = *free_list;
        while (current->next != NULL) current = current->next; 
        current->next = block;
        block->prev = current;
    }
}

void display_list(block_metadata* free_list)
{
    block_metadata* current = free_list;

    int count = 1;
    while (current != NULL) {
        printf("count = %d\n", count);
        count++;
        printf("actual addresss in memory: %p\n", current);
        printf("heap start address: %p\n", current->block_start);
        printf("size: %zd\n", current->size);
        printf("is_allocated: %d\n", current->alloc_id);
        printf("next address: %p\n", current->next);
	printf("prev address: %p\n", current->prev);
        printf("\n");

        current = current->next;
    }
}
