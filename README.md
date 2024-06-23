# primes-with-threads
## Description

This project focuses on developing efficient data processing skills using threading and data structures.
The goal is to handle an endless stream of random numbers, simulating scenarios such as real-time security camera data where intrusion detection involves identifying prime numbers. 
The challenge includes optimizing performance to utilize multiple CPU cores effectively while maintaining memory usage below 2MB.
we asked to enhance the isPrime function and utilize threading techniques to achieve significant performance improvements. Results will be evaluated based on execution speed and memory efficiency.

### wanted result are:

 4 times better without improving the isPrime function.
 
 10 times better with improving the isPrime function. 

# submitted:
a) solution code.

b) printScreen with 10M numbers checked by provided primeCounter (used for reference) + time.

c) printScreen with 10M numbers checked by your solution + time.

d) proof of ram usage below 2MB.

# how to run:
open the folder where the project is.

To run this task you should run - make.

for run the original code;

run: time ./randomGenerator 10 10000000 | ./primesCounter

for run the improve code - myprime.c

run: time ./randomGenerator 10 10000000 | ./myprime































