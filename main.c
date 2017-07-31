/* 
 * File:   main.c
 * Author: andrewchoi
 *
 * Created on July 25, 2017, 10:30 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Reads an .ac file by argument and calculates the circuit output and 
 * partial derivatives for every node.
 * The circuit is stored in an adjacency list.
 * Each non-leaf node storess its children in a separate array.
 */

/* Node Structure
   Children and flag only apply to non-leaf nodes */
struct node {
  /*Node type can be 'n' or 'v' for leaf nodes
    Node type can be '+' or '-' for non-leaf nodes */
  char nodeType; 
  /*Node index, e.g. "Third variable: n=2*/
  int index;
  /*Value of the node*/
  double vr; 
  /*Derivative of the node*/
  double dr; 
  /*Children nodes*/
  /*Currently assumes a binary AC (only two children per node)*/
  //struct node **child;
  int child[2];
  /*Bit flag, true means there is exactly one child that is zero*/
  bool flag;
  /*Zero counter, if this counter is 1, set flag to true*/
  /*Currently not used (assuming binary AC structure)*/
  //int counter;
};

struct node **circuit;

/* 
 * Function to perform bit-encoding backpropagation 
 */
void bit_backpropagation(int index){
  struct node* parent;
  for (int i = index; i >= 0; i--) {
    parent = circuit[i];

    /*Assign dr values depending on parent node*/
    if (parent->nodeType == '+') {
      circuit[parent->child[0]]->dr = parent->dr;
      circuit[parent->child[1]]->dr = parent->dr;
    }
    else if (parent->nodeType == '*') {
      /*if bit flag is down, and parent is non-zero, dr(c) = dr(p)*vr(p)/vr(c)*/
      if (parent->dr == 0) {
	/*Set all child nodes dr to zero*/
	circuit[parent->child[0]]->dr = 0;
	circuit[parent->child[1]]->dr = 0;
      }
      else if (parent->flag) {
	/*Check value of all child nodes*/
	/*if flag is up and child is zero, then dr(c) = dr(p) * vr(p)*/
	if (circuit[parent->child[0]]->vr == 0) {
	  circuit[parent->child[0]]->dr = parent->dr * parent->vr;
	  /*Set all other children dr to zero*/
	  circuit[parent->child[1]]->dr = 0;
	}
	else {
	  circuit[parent->child[1]]->dr = 0;
	  circuit[parent->child[0]]->dr = parent->dr *
	    (parent->vr / circuit[parent->child[0]]->vr);
	}
      }
      else {
	circuit[parent->child[1]]->dr = parent->dr *
	  (parent->vr / circuit[parent->child[1]]->vr);
	circuit[parent->child[0]]->dr = parent->dr *
	  (parent->vr / circuit[parent->child[0]]->vr);
      }
    }
  }
}
/*
 * Function to free all nodes and print the backpropagated values in AC
 */
int free_nodes(int index) {
  if (circuit == NULL) {
    printf("Circuit is empty!\n");
    return (EXIT_FAILURE);
  }
  for (int i = 0; i <= index; i++) {
    if (circuit[i] != NULL) {
      printf("n%d t: %c, dr: %lf vr: %lf\n", i, circuit[i]->nodeType, circuit[i]->dr, circuit[i]->vr);
      free(circuit[i]);
    }
  }
  return (EXIT_SUCCESS);
}

int main(int argc, char** argv) {
  FILE *ac_file;
  char lineToRead[5000]; 
  //struct node **circuit;
  struct node *n;
  int index = 0;
  int size = 0;
  
  /*Try to open the AC file*/
  if (argc < 2) {
    /*No file has been passed - error*/
    fprintf(stderr, "Must pass AC file\n");
    return(EXIT_FAILURE);
  }

  if (argc == 3) {
    size = atoi(argv[2]);
  }
    
  ac_file = fopen(argv[1], "r");
    
  if (!ac_file) {
    /* File does not exist*/
    fprintf(stderr, "Unable to read file %s\n", argv[1]);
    return(EXIT_FAILURE);
  }
    
  /*File was successfully read*/
  while (fgets(lineToRead, 20000, ac_file) != NULL) {
    //printf("index: %d, %s", index, lineToRead);
        
    if (*lineToRead == '(') {
      printf("\t... reading file ...\n");

      /*Allocate memory for the circuit*/
      if (size > 0) {
	size += 20;
	circuit = (struct node**)malloc(sizeof(struct node*) * size);
      }
      else {
	circuit = (struct node**)malloc(sizeof(struct node*) * 50000);
      }
    }
    else if (*lineToRead == 'E'){
      printf("\t... done reading file ... \n");
      index--;
      n->dr = 1;
      break;
    }
    else{
      if (*lineToRead == 'n') {
	/*Leaf node (Constant)*/
	/*Insert node into circuit*/
	n = (struct node*)malloc(sizeof(struct node));
	sscanf(lineToRead, "%s %lf", &(n->nodeType), &(n->vr));
	n->dr = 0;
	n->flag = false;
      }
      else if (*lineToRead == 'v') {
	/*Leaf node (Variable)*/
	n = (struct node*)malloc(sizeof(struct node));
	sscanf(lineToRead, "%s %d %lf", &(n->nodeType), &(n->index), &(n->vr));
	n->dr = 0;
	n->flag = false;
      }
      else if (*lineToRead == '+') {
	/*Non-leaf (Operation)*/
	n = (struct node*)malloc(sizeof(struct node));
	/*"n->child" stores the index of the children nodes in the circuit*/
	sscanf(lineToRead, "%s %d %d", &(n->nodeType), &(n->child[0]), &(n->child[1]));
	n->flag = false;
	n->vr = 0;
	n->dr = 0;
	
	/*Only add values if the flag is down*/
	if (!circuit[n->child[0]]->flag) {
	  n->vr += circuit[n->child[0]]->vr;
	}
	if (!circuit[n->child[1]]->flag) {
	  n->vr += circuit[n->child[1]]->vr;
	}
	/*Incorrect output when using bit flags*/
	//n->vr = circuit[n->child[0]]->vr + circuit[n->child[1]]->vr;
      }
      else if (*lineToRead == '*') {
	/*Non-leaf (Operation)*/
	n = (struct node*)malloc(sizeof(struct node));
	sscanf(lineToRead, "%s %d %d", &(n->nodeType), &(n->child[0]), &(n->child[1]));
        n->vr = 1;
	n->dr = 0;

	/*Raise bit flag if there is exactly one child with value equal to 0*/
	if (circuit[n->child[0]]->vr == 0 && circuit[n->child[1]]->vr != 0) {
	  n->flag = true;
	  /*Set value to product of all other non-zero child nodes*/
	  if (!circuit[n->child[1]]->flag) {
	    n->vr = circuit[n->child[1]]->vr;
	  }
	  else {
	    n->vr = 0;
	  }
	}
	else if (circuit[n->child[0]]->vr != 0 && circuit[n->child[1]]->vr == 0) {
	  n->flag = true;
	  /*Set value to product of all other non-zero child nodes*/
	  if (!circuit[n->child[0]]->flag) {
	    n->vr = circuit[n->child[0]]->vr;
	  }
	  else {
	    n->vr = 0;
	  }
	}
	else {
	  n->flag = false;
	  if (!circuit[n->child[0]]->flag) {
	    n->vr *= circuit[n->child[0]]->vr;
	  }
	  else {
	    n->vr = 0;
	  }
	  if (!circuit[n->child[1]]->flag) {
	    n->vr *= circuit[n->child[1]]->vr;
	  }
	  else {
	    n->vr = 0;
	  }
	}
      }
      //printf("node type: %c, vr: %lf, index: %d, flag %d\n", n->nodeType, n->vr, index, n->flag);
      circuit[index] = n;
      index++;   
    }
  }

  /*Print out circuit output*/
  printf("output %lf for %d nodes\n\n", circuit[index]->vr, index);

  /*Bit-encoded backpropagation*/
  printf("\t... starting backpropagation ...\n");
  bit_backpropagation(index);
  
  /*Free all nodes and circuit*/
  free_nodes(index);
  free(circuit);
  
  /*Close file*/
  if (ac_file != NULL) {
    fclose(ac_file);
  }
  
  return (EXIT_SUCCESS);
}
  
