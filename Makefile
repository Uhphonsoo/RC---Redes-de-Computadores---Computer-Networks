all: client DS

client: user.o functions.o user_functions.o
	gcc user.o functions.o user_functions.o -o user 

DS: DS.o functions.o DS_functions.o
	gcc DS.o functions.o DS_functions.o -o DS

user.o: user.c
	gcc -c user.c

DS.o: DS.c
	gcc -c DS.c

functions.o: functions.c
	gcc -c functions.c

user_functions.o: user_functions.c
	gcc -c user_functions.c

clean:
	rm *.o user
