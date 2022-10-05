all: exe

exe: main.o thumb.o
	gcc main.o thumb.o -o exe

main.o: main.c
	gcc -c main.c -Iinc -o main.o

thumb.o: thumb.c
	gcc -c thumb.c -Iinc -o thumb.o

clean:
	rm *.o exe 
