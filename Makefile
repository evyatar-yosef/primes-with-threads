.PHONY: all clean run-primeCounter run-myprime

all: generator primeCounter myprime

generator: generator.c
	gcc -o randomGenerator generator.c

primesCounter: primesCounter.c
	gcc -o primeCounter primeCounter.c

myprime: myprime.c
	gcc -o myprime myprime.c -lpthread -lm

clean:
	rm -f randomGenerator primeCounter myprime

run-primesCounter:
	time ./randomGenerator 10 10000000 | ./primeCounter

run-myprime:
	time ./randomGenerator 10 10000000 | ./myprime
