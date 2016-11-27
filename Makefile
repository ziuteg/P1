PROGRAM_NAME = main
FLAGS = -pthread -std=c++11 -Wall -Werror
CPP = g++

main: main.o barbershop.o customer.o barber.o
	$(CPP) $(FLAGS) -o main main.o barbershop.o customer.o barber.o

main.o: main.cpp barbershop.h
	$(CPP) -c $(FLAGS) main.cpp -o  main.o

barbershop.o: barbershop.cpp barbershop.h customer.h barber.h
	$(CPP) -c $(FLAGS) barbershop.cpp -o barbershop.o

customer.o: customer.cpp customer.h barber.h
	$(CPP) -c $(FLAGS) customer.cpp -o customer.o
	
barber.o: barber.cpp barber.h customer.h
	$(CPP) -c $(FLAGS) barber.cpp -o barber.o

run:
	./$(PROGRAM_NAME) 3 30
