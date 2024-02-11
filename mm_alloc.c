/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines. Remove this comment and provide
 * a summary of your allocator's design here.
 */

#include "mm_alloc.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/resource.h>
#define MAXIMUM_SEGMENT_SIZE 1000
/* Your final implementation should comment out this macro. */
//#define MM_USE_STUBS

void* heap_start;

struct s_block* list;
int list_size = 0;
void make_one(s_block_ptr b){
	 struct s_block* b1 = b->prev;
         struct s_block* b2 = b;
         struct s_block* b3 = b->next;
	if(b->next != NULL && b->prev != NULL){
		if((b->next)->free == 1 && (b->prev)->free == 1){//triple fusion
			b1 -> next = b3->next;
			if((b3 -> next) != NULL)
				(b3 -> next) -> prev = b1;
			b1 -> size = b1->size + b2->size + b3->size + 2 * sizeof(struct s_block);
			list_size = list_size - 2;
			return;

		}
		if((b->next) -> free == 1 && (b->prev) -> free == 0){//fusion with next block
			b2 -> next = b3 -> next;
			if((b3 -> next) != NULL)
				(b3 -> next) -> prev = b2;
			b2 -> size = b2->size + b3->size + 1 * sizeof(struct s_block);
			list_size = list_size - 1;
			return;
		}
		if((b->next) -> free == 0 && (b->prev) -> free == 1){//fusion with previous block
			b1 -> next = b2 -> next;
			if((b2 -> next) != NULL)
				(b2 -> next) -> prev = b1;
			b1 -> size = b1->size + b2->size + 1 * sizeof(struct s_block);	
			list_size = list_size - 1;
			return;
		}
		return;
	}
	if(b->next == NULL && b->prev != NULL && (b->prev) -> free == 1){//fusion with previous block
		b1 -> next = b2 -> next;
		b1 -> size = b1->size + b2->size + 1 * sizeof(struct s_block);
		list_size = list_size - 1;
		return;
	}
	if(b->next != NULL && b->prev == NULL && (b->next) -> free == 1){//fusion with next block
		b2 -> next = b3 -> next;
		if((b3 -> next) != NULL)
			(b3 -> next) -> prev = b2;
		b2 -> size = b2->size + b3->size + 1 * sizeof(struct s_block);
		list_size = list_size - 1;
		return;
	}
}
void split_block (s_block_ptr b, size_t s){
	struct s_block* new_block = b -> ptr + sizeof(struct s_block) + s;
	
	size_t size0 = b -> size;
    	struct s_block* next0 = b -> next;
    	struct s_block* prev0 = b-> prev;
    	
	b -> size = s;
	b -> next = new_block;
	b -> free = 0;
	
	new_block -> size = size0 - s - sizeof(struct s_block);
	new_block -> free = 1;
	new_block -> next = next0;
	if(next0 != NULL)
		next0 -> prev = new_block;
	new_block -> prev = b;
	new_block -> ptr = b -> ptr + sizeof(struct s_block) + s;
	
	list_size++;
}
void* move_breakline(size_t size){	//moves the brk and create a new block based on the size
	struct s_block* last_block = list;
	for(int i = 0; i < list_size - 1; i++){
		if(last_block -> next == NULL)
			perror("error in move_breakline");
		last_block = (last_block -> next);
	}
	if(last_block -> free == 1){
/*
		if(sbrk(0) + size - last_block->size > getrlimit()){//must move rlimit
                        //TODO
                }
*/
		sbrk(size - last_block->size);
		last_block -> size = size;
		last_block -> free = 0;
		return last_block->ptr + sizeof(struct s_block);
	}
/*
	if(sbrk(0) + size + sizeof(s_block) > getrlimit()){//must move rlimit
		//TODO
	}
*/
	sbrk(size + sizeof(struct s_block));	
	
	struct s_block* new_block = last_block->ptr + sizeof(struct s_block) + last_block->size;
	new_block -> ptr = last_block->ptr + sizeof(struct s_block) + last_block->size;
	new_block -> size = size;
	new_block -> next = NULL;
	new_block -> free = 0;

	new_block -> prev = last_block;
	last_block -> next = new_block;
	list_size++;
	return new_block->ptr + sizeof(struct s_block);
}
void* mm_malloc(size_t size)
{
/*
#ifdef MM_USE_STUBS
    return calloc(1, size);
#else
#error Not implemented.
#endif
*/
	if(size == 0)
		return NULL;
	if(size > MAXIMUM_SEGMENT_SIZE){
		perror("SIZE_LIMIT_ERROR)");
		return NULL;
	}
	if(list_size == 0){//first time in the function
		heap_start = sbrk(0);
	}
	if(sbrk(0) - heap_start <= 0){	//break is under heap_start (usually for the first time using the function)
		void* sbrk_backup = sbrk(0);
		sbrk(sbrk(0) - heap_start + 1 * (sizeof(struct s_block) + size));//moved break up
		
		struct s_block* first_block = sbrk_backup;
		struct s_block* second_block = sbrk_backup + sizeof(struct s_block) + size;
		
		first_block->size = size;
		first_block->next = NULL; // this one is new
		first_block->prev = NULL;
		first_block->ptr = sbrk_backup;//heap_start
		first_block->free = 0;
		list = first_block;
		list_size += 1;
			
		memset(first_block->ptr + sizeof(struct s_block), 0, size);
		return first_block->ptr + sizeof(struct s_block);
		
	}
	if(sbrk(0) - heap_start >= size + sizeof(struct s_block)){//allocate memory
		struct s_block* block = list;
		for(int i = 0; i < list_size; i++){
			if(block -> free == 1 && block -> size >= size){
				if(block -> size >= size + sizeof(struct s_block)){//split the block
					split_block(block,size);
					memset(block->ptr + sizeof(struct s_block), 0, block->size);
					return block->ptr + sizeof(struct s_block);
				}
				else{
					block->free = 0;
					memset(block->ptr + sizeof(struct s_block), 0, block->size);
					return block->ptr + sizeof(struct s_block);
				}
			}
			block = block -> next;
		}
	//There is no free block --> we must move the brk line
		void* ptr = move_breakline(size);
		memset(ptr + sizeof(struct s_block), 0, size);
		return ptr;
	}
	else if(sbrk(0) - heap_start < size + sizeof(struct s_block)){//move brk line
		void* ptr = move_breakline(size);
                memset(ptr + sizeof(struct s_block), 0, size);
                return ptr;
	}
	
	return NULL;
}

void* mm_realloc(void* ptr, size_t size)
{
/*
#ifdef MM_USE_STUBS
    return realloc(ptr, size);
#else
#error Not implemented.
#endif
*/
	if(size == 0)
		return NULL;
	if(ptr == NULL)
		return mm_malloc(size);
	
	
	struct s_block* block = list;
	for(int i = 0; i < list_size; i++){
		if(block -> ptr + sizeof(struct s_block) == ptr){
			if(block->size == size)
				return block->ptr + sizeof(struct s_block);
			if(block->size > size){
				int block_size = block->size;
                                char c[block ->size];
                                for(int i = 0; i < block->size; i++){
                                        c[i] = *(block->data + i);
                                }
                                mm_free(ptr);
                                void* new_ptr = mm_malloc(size);
                                memcpy(new_ptr, c , size);
                                return new_ptr;
			
			}
			if(block->size < size){
				int block_size = block->size;
				char c[block ->size];
				for(int i = 0; i < block->size; i++){
                                        c[i] = *(block->data + i);
                                }
				mm_free(ptr);
				void* new_ptr = mm_malloc(size);
				memcpy(new_ptr, c , block_size);
				return new_ptr;
			}
		}
		block = block -> next;
	}
	perror("something is not right!");
	return NULL;
}

void mm_free(void* ptr)
{
/*
#ifdef MM_USE_STUBS
    free(ptr);
#else
#error Not implemented.
#endif
*/
	if(ptr == NULL)
		perror("NULL_POINTER_ERROR");
	else{
		struct s_block* block = list;
		for(int i = 0; i < list_size; i++){
			if(block->ptr + sizeof(struct s_block) == ptr){
				block->free = 1;
				make_one(block);
				return;
			}
			block = block->next;
		}
		perror("Wrong pointer in mm_free!");
		return;
	}
}
