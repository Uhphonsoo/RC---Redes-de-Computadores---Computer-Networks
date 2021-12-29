all: client server

client: user.o functions.o user_functions.o
	gcc user.o functions.o user_functions.o -o user 

server: server.o functions.o server_functions.o
	gcc server.o functions.o server_functions.o -o server

user.o: user.c
	gcc -c user.c

server.o: server.c
	gcc -c server.c

functions.o: functions.c
	gcc -c functions.c

user_functions.o: user_functions.c
	gcc -c user_functions.c

clean:
	rm *.o user
