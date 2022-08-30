all : Client

Client: Main.o Client.o 
	g++ Main.o Client.o -ljsoncpp -o client

Main.o: Main.cpp Client.hpp
	g++ -c Main.cpp -ljsoncpp

Client.o: Client.cpp Client.hpp
	g++ -c Client.cpp -ljsoncpp



clean:
	rm *.o client


