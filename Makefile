all: dict1 dict2

dict1: dict1.o 
	gcc -g -O0 -o dict1 dict1.o 

dict2: dict2.o 
	gcc -g -O0 -o dict2 dict2.o 
 
dict1.o: dict1.c 
	gcc -g -O0 -c dict1.c

dict2.o: dict2.c 
	gcc -g -O0 -c dict2.c

clean:
	rm task5 *.o
