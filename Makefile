build : main.cpp
	g++ -Wall main.cpp -o calc

run : calc
	./calc

clean :
	rm -rf *.o *.out calc
