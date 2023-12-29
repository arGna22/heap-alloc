#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "alloc.h"
#include "llist.h"

static int is_initialized = 0;
static heap_metadata heap_data; 

static heap_metadata initialize_heap() 
{
	size_t memory_size = HEAP_SIZE / BLOCK_SIZE * sizeof(block_metadata) + 1024; 
	void *free_list_start = mmap(NULL, memory_size, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	void *heap_start = free_list_start + HEAP_SIZE / BLOCK_SIZE * sizeof(block_metadata);
	heap_metadata data = {.heap_start = heap_start, .heap_size = HEAP_SIZE}; 
	
	block_metadata *free_list = NULL;
	void *current_block_addr = free_list_start;

	for (int i = 0; i < HEAP_SIZE / BLOCK_SIZE; i++) {
		append_block(&free_list, current_block_addr);
		current_block_addr += sizeof(block_metadata); 
	} 

	data.blocks = free_list;

	return data;
}

void* heap_alloc(size_t bytes)
{
	if (!is_initialized) {
		heap_data = initialize_heap();
		is_initialized = 1;
	}

	static int alloc_id = 1;
	
	if (bytes <= 0) 
		return NULL;

	int blocks_to_allocate = 1 + bytes / BLOCK_SIZE; 
	int blocks_in_a_row = 0; 

	block_metadata *current = heap_data.blocks;

	while (current->next != NULL && blocks_in_a_row != blocks_to_allocate) {
		if (current->alloc_id == 0) blocks_in_a_row++;
		else blocks_in_a_row = 0;

		if (blocks_in_a_row == blocks_to_allocate) {
			block_metadata* block_to_allocate = current;

			for (int i = blocks_in_a_row; i > 0; i--) {
				block_to_allocate->alloc_id = alloc_id;
				if (i > 1) block_to_allocate = block_to_allocate->prev; 
			}

			return block_to_allocate->block_start; 
		}

		current = current->next;
		if (current == NULL) { 
			fprintf(stderr, "Not enough heap space.");
			return NULL;
		}
	}

	alloc_id++;
}

void heap_free(void* ptr) 
{
	block_metadata* current = heap_data.blocks;

	while (current->block_start != ptr && current != NULL) current = current->next;
	if (current == NULL) {
		fprintf(stderr, "Error: invalid memory address passed in.");
		return;
	}

	block_metadata* block_to_free = current;
	int target_id = block_to_free->alloc_id;

	while (block_to_free != NULL && block_to_free->alloc_id == target_id) {
		block_to_free->alloc_id == 0;
		block_to_free = block_to_free->next;
	}
}
