/* 
 * File:   main.c
 * Author: andre
 *
 * Created on July 25, 2017, 10:30 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
 * 
 */
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
  //struct node **child;
  int child[2];
  /*Bit flag, true means there is exactly one child that is zero*/
  bool flag;
  /*Zero counter, if this counter is 1, set flag to true*/
  int counter;
};



int main(int argc, char** argv) {
  FILE *ac_file;
  char lineToRead[5000]; 
  struct node **circuit;
  struct node* n;
  int index = 0;
  
    
  /*Try to open the AC file*/
  if (argc < 2) {
    /*No file has been passed - error*/
    fprintf(stderr, "Must pass AC file\n");
    return(EXIT_FAILURE);
  }
    
  ac_file = fopen(argv[1], "r");
    
  if (!ac_file) {
    /* File does not exist*/
    fprintf(stderr, "Unable to read file %s\n", argv[1]);
    return(EXIT_FAILURE);
  }
    
  /*File was successfully read*/
  /*Allocate memory for the circuit*/
  circuit = (struct node**)malloc(sizeof(struct node*) * 50);
  while (fgets(lineToRead, 5000, ac_file) != NULL) {
    printf("%s", lineToRead);
    
        
    if (*lineToRead == '(') {
      /*Do nothing*/
    }
    else{
      if (*lineToRead == 'n') {
	/*Leaf node (Constant)*/
	/*Insert node into circuit*/
	n = (struct node*)malloc(sizeof(struct node));
	sscanf(lineToRead, "%s %lf", &(n->nodeType), &(n->vr));
	n->flag = false;
	//printf("in: %c, val: %lf \n", n->nodeType, n->vr);
      }
      else if (*lineToRead == 'v') {
	/*Leaf node (Variable)*/
	n = (struct node*)malloc(sizeof(struct node));
	sscanf(lineToRead, "%s %d %lf", &(n->nodeType), &(n->index), &(n->vr));
	n->flag = false;
	//printf("in: %c %d, val: %lf \n", n->nodeType, n->index, n->vr);
      }
      else if (*lineToRead == '+') {
	/*Non-leaf (Operation)*/
	n = (struct node*)malloc(sizeof(struct node));
	/*"n->child" stores the index of the children nodes in the circuit*/
	sscanf(lineToRead, "%s %d %d", &(n->nodeType), &(n->child[0]), &(n->child[1]));
	n->flag = false;
	n->vr = 0;
	
	/*Only add values if the flag is down*/
	if (!circuit[n->child[0]]->flag) {
	  n->vr += circuit[n->child[0]]->vr;
	}
	if (!circuit[n->child[1]]->flag) {
	  n->vr += circuit[n->child[1]]->vr;
	}
	//n->vr = circuit[n->child[0]]->vr + circuit[n->child[1]]->vr;
	//printf("in: %c %d, val: %lf \n", n->nodeType, n->index, n->vr);
      }
      else if (*lineToRead == '*') {
	/*Non-leaf (Operation)*/
	n = (struct node*)malloc(sizeof(struct node));
	sscanf(lineToRead, "%s %d %d", &(n->nodeType), &(n->child[0]), &(n->child[1]));
        n->vr = 1;

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
	
	//printf("in: %c %d, val: %lf \n", n->nodeType, n->index, n->vr);
      }
      printf("node type: %c, vr: %lf, index: %d, flag %d\n", n->nodeType, n->vr, index, n->flag);
      //printf("node added %d\n", index);
      n->dr = 0;
      circuit[index] = n;
      index++;
    }
    
    if (*lineToRead == 'E') {
      printf("... finished reading AC file ... \n");
      index--;
    }
  }

  /*Print out circuit output*/
  printf("output %lf\n", circuit[index]->vr);

  /*Bit-encoded backpropagation*/
  
  
  /*Free all nodes and circuit*/
  for (int i = 0; i < index; i++) {
    //printf("%d\n", i);	    
    free(circuit[i]);
  }
    
  free(circuit);

  /*Close file*/
  fclose(ac_file);
    
  return (EXIT_SUCCESS);
}

