#include "my_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	int i;
	void*p,*p1;
	int x=10;
	//for(i = 1; i<10; i=i+1)
	//{
	  // p=my_malloc(i*x);
	   //x=i*x;
	   //my_free(p);
	//}
	p=my_malloc(2500);
	my_free(p);
	//p1=my_malloc(2500);
	///my_free(p1);

	meminfo();
	return 0;
}