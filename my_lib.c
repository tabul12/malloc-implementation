#include "my_lib.h"
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdio.h>

#define PAGE_SIZE 4096
#define KK 4
static int has_initialized = 0;
static void *managed_memory_start;
static void *last_valid_address;
static void *last_valid_block_address=NULL;
static int M_TRIM_THRESHOLD=128*1024;
static int bla=0;
/*
	roca sul pirvelad izaxeba malloc
*/
void my_malloc_init()
{
	last_valid_address = sbrk(0);
	managed_memory_start = last_valid_address;
 	has_initialized = 1;
 	char* val=getenv("M_TRIM_THRESHOLD");
	if(val!=NULL)
		M_TRIM_THRESHOLD=atoi(val);
}
/*
	malloc
*/
void * my_malloc(int numbytes)
{
	void *current_location;
	struct mem_control_block *current_location_mcb;
	void *memory_location; //amas davabrunebt
	
	if(! has_initialized) //tu saertod pirvelad vizaxebt mallocs
		my_malloc_init();
	numbytes = numbytes + sizeof(struct mem_control_block); //realurad gvinda gamovyot motxovnils damatebuli metainfods zoma

	memory_location = NULL; //defaultad NULL iyos
	current_location = managed_memory_start;//ziebas viwyebt startidan
	// gadavuyvebit blockebs vnaxulobt tua tavisufali mere tu jdeba zomashi da yvelaze dids varchevt asetebshi
	//radgan worst fit algoritmia
	void * bigest_free_block_pointer;
	int bigest_free_block_size=0;

	while(current_location != last_valid_address)
	{
		current_location_mcb =(struct mem_control_block *)current_location;
		if(current_location_mcb->is_available)
			if(bigest_free_block_size<current_location_mcb->size)
			{
				bigest_free_block_size=current_location_mcb->size;
				bigest_free_block_pointer=current_location;
			}
		current_location = (uint8_t*)current_location + current_location_mcb->size;
	}
	struct mem_control_block * bigest_as_struct=(struct mem_control_block *)bigest_free_block_pointer;
	if(bigest_as_struct->size >= numbytes)
	{
		bigest_as_struct->is_available = 0;
		memory_location = bigest_free_block_pointer;
		if(bigest_as_struct->size-numbytes>sizeof(struct mem_control_block))
		{
			int rest=bigest_as_struct->size-numbytes;
			bigest_as_struct->size=numbytes;
			struct mem_control_block * new_block=(struct mem_control_block *)((uint8_t*)bigest_free_block_pointer+numbytes);
			new_block->size=rest;
			new_block->is_available=1;
			void* next_block=(uint8_t*)new_block+rest;
			
			if(next_block!=last_valid_address)
			{
				struct mem_control_block * next_block_mcb=next_block;
				next_block_mcb->previous_block_pointer=new_block;
			}
		}
	}
	//tu ver vnaxe taviosufali bloki
	if(memory_location==NULL)
	{
		int new_break=numbytes+KK*PAGE_SIZE;
		sbrk(new_break);
		memory_location = last_valid_address;

		 //gadavwiot last valid adresi
		last_valid_address = last_valid_address + new_break;
		//gavaketot inicializacia mem control blokis
		current_location_mcb = memory_location;
		current_location_mcb->is_available = 0;
		current_location_mcb->size = numbytes;
		current_location_mcb->previous_block_pointer=last_valid_block_address;
		last_valid_block_address=(void*)current_location_mcb;
		
		void * previous=current_location;

		current_location = (uint8_t*)current_location + current_location_mcb->size;
		current_location_mcb =(struct mem_control_block *)current_location;

		current_location_mcb->is_available = 1;
		current_location_mcb->size = KK*PAGE_SIZE;
		current_location_mcb->previous_block_pointer=previous;
		last_valid_block_address=(void*)current_location_mcb;

	}
	//davabrunot pasuxi
	memory_location = (uint8_t*)memory_location + sizeof(struct mem_control_block);
	return memory_location;
}
/*
	free function
*/
void my_free(void* ptr)
{
	struct mem_control_block *block_addr=(struct mem_control_block *)((uint8_t*)ptr-sizeof(struct mem_control_block));
	block_addr->is_available=1;
	struct mem_control_block * previous=(struct mem_control_block *)block_addr->previous_block_pointer;
	struct mem_control_block * next=(struct mem_control_block *)((uint8_t*)block_addr+block_addr->size);
	if(previous!=NULL && previous->is_available==1)
	{
		previous->size=previous->size+block_addr->size;
	}
	else
		previous=block_addr;
	if(next!=last_valid_address && next->is_available==1)
	{
		previous->size=(previous->size)+(next->size);
		void* next_next=(uint8_t*)previous+previous->size;
		if(next_next!=last_valid_address)
		{
			((struct mem_control_block *)next_next)->previous_block_pointer=previous;
		}

	}
	
	if(((uint8_t*)previous+previous->size)==last_valid_address)
	{
		if(previous->size>=M_TRIM_THRESHOLD)
		{
			last_valid_address=previous;
			last_valid_block_address=previous->previous_block_pointer;
			brk((void*)previous);
		}
		else
			last_valid_block_address=previous;
	}
}
void meminfo()
{
	void *current_location;
	struct mem_control_block *current_location_mcb;
	current_location = managed_memory_start;
	int i = 1;
	while(current_location != last_valid_address)
	{
		current_location_mcb =(struct mem_control_block *)current_location;
		printf("Block number : %d\n",i);
		printf("Block size : %d\n",current_location_mcb->size);
		if(current_location_mcb->is_available)
			printf("Block is is_available :%s\n","Yes");
		else
			printf("Block is is_available :%s\n","No");
		printf("\n");
		current_location = (uint8_t*)current_location + current_location_mcb->size;
		i++;
	}
}