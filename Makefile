
output: user.o
	gcc user.o -o user 

user.o: user.c
	gcc -c user.c

clean:
	rm *.o user
