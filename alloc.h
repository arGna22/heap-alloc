#define HEAP_SIZE 1024
#define BLOCK_SIZE 16 

typedef struct block_metadata { // need to add is_allocated field.
    void *block_start; 
    size_t size;
    int alloc_id;
    struct block_metadata* next;
    struct block_metadata *prev; 
} block_metadata;

typedef struct heap_metadata {
    void *heap_start;
    size_t heap_size; 
    block_metadata *blocks;
} heap_metadata;
