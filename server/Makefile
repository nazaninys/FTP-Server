CC = g++
FLAGS = -std=c++11 
all : server

server: main.o server.o database.o requestHandler.o user.o
	$(CC) $(FLAGS) main.o server.o database.o requestHandler.o user.o -ljsoncpp -o server

main.o: main.cpp server.hpp 
	$(CC) $(FLAGS) -c main.cpp -ljsoncpp

server.o: server.cpp server.hpp database.hpp user.hpp
	$(CC) $(FLAGS) -c server.cpp -ljsoncpp

database.o : database.cpp database.hpp
	$(CC) $(FLAGS)  -c database.cpp -ljsoncpp

requestHandler.o : requestHandler.cpp requestHandler.hpp 
	$(CC) $(FLAGS)  -c requestHandler.cpp
user.o : user.cpp user.hpp
	$(CC) $(FLAGS)  -c user.cpp

clean:
	rm *.o server


