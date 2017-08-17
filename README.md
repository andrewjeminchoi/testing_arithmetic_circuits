# Arithmetic Circuits in C

#### Running the program for movie.ac on Linux

gcc -o ac main.c -lm

./ac movie.ac 30000


The program also accepts a third argument, which specifies the size of the circuit. 

The default is cache-propagation, but the code currently supports bit-encoded (as described in Darwiche 2003, JACM) and cache propagation. The bit-encoding propagation always assumes that the AC file is alternating between addition and multiplication nodes. 
