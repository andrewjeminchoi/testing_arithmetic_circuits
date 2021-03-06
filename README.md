# Arithmetic Circuits in C

#### Description
An implementation of a feed-forward arithmetic circuit to find marginal probabilities. Cache-propagation (default) produces feed-forward circuits, bit-encoded does not. 

The default is cache-propagation, but the code also supports bit-encoded (as described in Darwiche 2003, JACM). The bit-encoding propagation always assumes that the AC file is alternating between addition and multiplication nodes. 


#### Running the program for movie.ac on Linux

gcc -o ac main.c -lm

./ac movie.ac 30000

The second argument is the name of the ac file. 
The program also accepts a third argument, which specifies the size of the circuit. There is a 'default' size if the size is not specified (50000).

