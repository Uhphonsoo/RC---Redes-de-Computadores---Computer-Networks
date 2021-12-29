
output: user.o functions.o
	gcc user.o functions.o -o user 

user.o: user.c
	gcc -c user.c

functions.o: functions.c
	gcc -c functions.c

clean:
	rm *.o user
