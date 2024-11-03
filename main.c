#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "memory_manager.h"

typedef struct MemoryBlock {

    size_t M_size;
    bool is_free;
    struct MemoryBlock* memory_address;
    struct MemoryBlock* next;
} MemoryBlock;


static void* memory_pool = NULL;
static MemoryBlock* free_lists = NULL;

static void* memory_pool2 = NULL;
static MemoryBlock* free_lists2 = NULL;

static void* memory_pool3 = NULL;
static MemoryBlock* free_lists3 = NULL;


void mem_init(size_t size) {

    memory_pool = malloc(size);
    memory_pool2 = malloc(size*sizeof(MemoryBlock));


    if (!memory_pool) {
        printf("It has failed to locate");
    }

    else {

    free_lists = (MemoryBlock*)memory_pool;

    free_lists2 = (MemoryBlock*)memory_pool2;
    free_lists2 -> M_size = size;
    free_lists2->is_free = true;
    free_lists2 -> next = NULL;
    free_lists2 -> memory_address = free_lists;

    free_lists3 = (MemoryBlock*)memory_pool3;

    }
}

void* mem_alloc(size_t size) {
    MemoryBlock* current2 = free_lists2;
    MemoryBlock* current = current2 -> memory_address; // free_lists

    //current -> size = current -> size + sizeof(MemoryBlock);
    if (size == 0) {
        return (void*)((char*)current2 + 1);
    }
    // Traverse the free list to find a suitable block
    while (current2 != NULL) {
        size_t currentSize = current2->M_size;
        bool currentIsfree = current2->is_free;



        if (currentIsfree && currentSize >= size) {
            if (currentSize >= size) {
                MemoryBlock* new_block = (MemoryBlock*)((char*)current + size);
                MemoryBlock* new_block2 = (MemoryBlock*)((char*)current2 + sizeof(MemoryBlock));

                new_block2 -> memory_address = new_block;

                new_block2 -> M_size = current2-> M_size - size;


                new_block2 -> is_free = true;
                new_block2 -> next = current2-> next;

                current2-> next = new_block2;
                current2 -> M_size = size;
                current2 -> is_free = false;

            } 
            
            return (void*)((char*)current2 + 1);
        }
        current2 = current2-> next;
    }
    return NULL; // No suitable block found
}



void mem_free(void* block) {
    if (block == NULL) {
        return;
    }

    // Get the memory block metadata
    MemoryBlock* header = (MemoryBlock*)((char*)block - 1);
    header ->is_free = true;

    // Coalesce adjacent free blocks
    MemoryBlock* current2 = free_lists2;
    while (current2 != NULL) {
        // Check if the current block and the next block are free
        if (current2->is_free && current2->next != NULL && current2 -> next ->is_free) {
            // Coalesce the current block with the next free block
            current2 ->M_size +=sizeof(MemoryBlock) + current2 -> next ->M_size;
            // Update the block pointer to skip over the merged block
            current2 ->next = current2 -> next ->next;
            return;

        } else {
            // Move to the next block
            current2 = current2 ->next;
        }

        // Safety check to prevent infinite loops
        if (current2 == NULL || current2->next == NULL) {
            break;
        }
    }
    
}



void* mem_resize(void* block, size_t size) {
    if (block == NULL) {
        return mem_alloc(size);
    }

    MemoryBlock* header = (MemoryBlock*)((char*)block);

    if (header -> M_size >= size) {
        return block; // No need to resize, current block is large enough
    }

    // Try to allocate a new block
    void* new_block = mem_alloc(size);
    if (new_block) {
        // Copy the old content to the new block
        memcpy(new_block, block, header -> M_size);
        mem_free(block);
    }

    return new_block;
}



void mem_deinit() {
    free(memory_pool); // Free the entire memory pool
    free(memory_pool2);

    memory_pool2 = NULL;
    memory_pool = NULL;
    free_lists2 = NULL;
    free_lists = NULL;
}