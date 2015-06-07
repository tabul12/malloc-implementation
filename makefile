my_lib: main.c my_lib.c 
	gcc -o my_lib main.c my_lib.c
clean: 
	rm -f my_lib
