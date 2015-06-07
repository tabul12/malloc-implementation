#ifndef MY_LIB
#define MY_LIB

struct mem_control_block
{
	int is_available;
	int size;
	void * previous_block_pointer;
};

void *my_malloc(int);
void my_free(void *);
void meminfo();

#endif 