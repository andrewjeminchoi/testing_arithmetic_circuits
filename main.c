/* 
 * File:   main.c
 * Author: andrewchoi
 *
 * Created on July 25, 2017, 10:30 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

/* 
 * Reads an .ac file by argument and calculates the circuit output and 
 * partial derivatives for every node.
 * The circuit is stored in an adjacency list.
 * Each non-leaf node storess its children in a linked list.
 */

/* 
 * GLOBAL VARIABLES 
 */
struct node **circuit; //Arithmetic Circuit Structure

/* 
 * CONSTANTS
 */
#define MAX_NODE_NUMBER 50000 //Program assumes max AC size of 50000 (if not specified)
#define MAX_LINE_NUMBER 20000
#define NODE_SAFETY_MARGIN 20 //Adds 20 to the AC size that user specified

/*
 * STRUCTURES
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
  /*Bit flag, true means there is exactly one child that is zero*/
  bool flag;
  /*Linked list of child nodes*/
  struct childList *childHead;
  /*Product registers*/
  double *prL;
  double *prR;
};

/* List of child nodes of a non-leaf node */
struct childList {
  int position; //Child node position, relative to tree. Left-most is position 0. 
  int childIndex;
  struct childList *next;
};


/* 
 * FUNCTIONS
 */

struct node* allocate_constant_node(char* line, struct node* n) {
  n = (struct node*)malloc(sizeof(struct node));
  sscanf(line, "%s %lf", &(n->nodeType), &(n->vr));
  n->dr = 0;
  n->flag = false;
  n->childHead = NULL;
  return n;
}

struct node* allocate_variable_node(char* line, struct node* n) {
  n = (struct node*)malloc(sizeof(struct node));
  sscanf(line, "%s %d %lf", &(n->nodeType), &(n->index), &(n->vr));
  n->dr = 0;
  n->flag = false;
  n->childHead = NULL;
  return n;
}

/*Function to perform bit-encoded backpropagation*/
void bit_backpropagation(int index){
  struct node *parent;
  struct childList *tempPtr;
  for (int i = index; i >= 0; i--) {
    parent = circuit[i];

    /*Assign dr values depending on parent node*/
    if (parent->nodeType == '+') {
      tempPtr = parent->childHead;
      while (tempPtr != NULL) {
	int cIndex = tempPtr->childIndex;
	circuit[cIndex]->dr += parent->dr;
	tempPtr = tempPtr->next;
      }
    }
    else if (parent->nodeType == '*') {
      if (parent->vr != 0) {
	if (!parent->flag) {
	  tempPtr = parent->childHead;
	  while (tempPtr != NULL) {
	    int cIndex = tempPtr->childIndex;
	    circuit[cIndex]->dr += (parent->dr)*(parent->vr)/(circuit[cIndex]->vr);
	    tempPtr = tempPtr->next;
	  }
	}
	else {
	  //printf("parent %d flag 1\n", i);
	  tempPtr = parent->childHead;
	  while (tempPtr != NULL) {
	    int cIndex = tempPtr->childIndex;
	    //printf("child %d \n", cIndex);
	    if (circuit[cIndex]->vr == 0) {
	      circuit[cIndex]->dr += (parent->dr) * (parent->vr);
	    }
	    tempPtr = tempPtr->next;
	  }
	}
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
      /* Print out the values and partial derivatives for each node*/
      printf("n%d t: %c, dr: log(%lf) vr: %lf, flag: %d\n",
	     i, circuit[i]->nodeType, log10(circuit[i]->dr), circuit[i]->vr, circuit[i]->flag);

      /* Deallocate the list of child nodes (if it's a non-leaf node) */
      if (circuit[i]->childHead != NULL) {
	struct childList *childPtr = circuit[i]->childHead;
	struct childList *next = childPtr; 
	while (childPtr != NULL) {
	  next = childPtr->next;
	  free(childPtr);
	  childPtr = next;
	}
	circuit[i]->childHead = NULL;
      }
      
      /* Deallocate the node */
      free(circuit[i]);
    }
  }
  return (EXIT_SUCCESS);
}

int main(int argc, char** argv) {
  FILE *ac_file;
  char lineToRead[MAX_LINE_NUMBER]; 
  //struct node **circuit;
  struct node *n; //temporary storage for nodes
  int index = 0;
  int size = 0;
  
  /*Try to open the AC file*/
  if (argc < 2) {
    /*No file has been passed - error*/
    fprintf(stderr, "Must pass AC file\n");
    return(EXIT_FAILURE);
  }

  /*If the size of AC is specified, allocate fixed amount of memory*/
  if (argc > 2) {
    size = atoi(argv[2]);
  }
    
  ac_file = fopen(argv[1], "r");
    
  if (!ac_file) {
    /* File does not exist*/
    fprintf(stderr, "Unable to read file %s\n", argv[1]);
    return(EXIT_FAILURE);
  }
    
  /*File was successfully read*/
  while (fgets(lineToRead, MAX_LINE_NUMBER, ac_file) != NULL) {
    //printf("index: %d, %s", index, lineToRead);
        
    if (*lineToRead == '(') {
      printf("\t... reading file ...\n");

      /*Allocate memory for the circuit*/
      if (size > 0) {
	size += NODE_SAFETY_MARGIN;
	circuit = (struct node**)malloc(sizeof(struct node*) * size);
      }
      else {
	circuit = (struct node**)malloc(sizeof(struct node*) * MAX_NODE_NUMBER);
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
	n = allocate_constant_node(lineToRead, n);
      }
      
      else if (*lineToRead == 'v') {
	/*Leaf node (Variable)*/
	n = allocate_variable_node(lineToRead, n);
      }
      
      else if (*lineToRead == '+') {
	/*Non-leaf (Operation)*/
	n = (struct node*)malloc(sizeof(struct node));
	/*"n->child" stores the index of the children nodes in the circuit*/
	sscanf(lineToRead, "%s", &(n->nodeType));
	n->flag = false;
	n->vr = 0;
	n->dr = 0;
	n->childHead = NULL;

	/*Read the sequence of child nodes*/
	char *nodeList = lineToRead;
	int childIndex;
	int offset;
	struct childList *children;
	struct childList *linking;
	nodeList += 2; /*Ignore the operator (first two characters) */
	
	while (sscanf(nodeList, " %d%n", &childIndex, &offset) == 1) {
	  children = (struct childList*)malloc(sizeof(struct childList));
	  children->childIndex = childIndex;
	 
	  if (n->childHead == NULL) {
	    n->childHead = children;
	    linking = n->childHead;
	  }
	  else {
	    linking->next = children;
	    linking = linking->next;
	  }
	  nodeList += offset;
	  
	}
	linking->next = NULL;

	/* Add the child node value only if the flag is down */
	struct childList *tempPtr = n->childHead;

	while (tempPtr != NULL) {
	  int cIndex = tempPtr->childIndex;
	  if (!circuit[cIndex]->flag) {
	    n->vr += circuit[cIndex]->vr;
	  }
	  tempPtr = tempPtr->next;
	}

	/*Incorrect output when using bit flags*/
	//n->vr = circuit[n->child[0]]->vr + circuit[n->child[1]]->vr;
      }
      
      else if (*lineToRead == '*') {
	/*Non-leaf (Operation)*/
	n = (struct node*)malloc(sizeof(struct node));
	n->nodeType = '*';
	n->vr = 1;
	n->dr = 0;
	n->flag = false;
	n->childHead = NULL;
	
	/*Read the sequence of child nodes*/
	char *nodeList = lineToRead;
	int childIndex;
	int offset;
	int zeroCount = 0;
	struct childList *children;
	struct childList *linking;
	nodeList += 2; /*Ignore the operator (first two characters) */
	
	while (sscanf(nodeList, " %d%n", &childIndex, &offset) == 1) {
	  children = (struct childList*)malloc(sizeof(struct childList));
	  children->childIndex = childIndex;

	  /*Insert child node*/
	  if (n->childHead == NULL) {
	    n->childHead = children;
	    linking = n->childHead;
	  }
	  else {
	    linking->next = children;
	    linking = linking->next;
	  }
	  nodeList += offset;

	  /*Check if child node is zero*/
	  if (circuit[childIndex]->vr == 0) {
	    zeroCount++;
	  }
	}
	linking->next = NULL;
	if (zeroCount == 1) {
	  n->flag = true;

	  /* Multiply child if flag is down and is not zero */
	  struct childList *tempPtr = n->childHead;

	  while (tempPtr != NULL) {
	    int cIndex = tempPtr->childIndex;
	    if (circuit[cIndex]->flag) {
	      n->vr = 0;
	      break;
	    }
	    else if (!circuit[cIndex]->flag && circuit[cIndex]->vr != 0) {
	      n->vr *= circuit[cIndex]->vr;
	    }
	    tempPtr = tempPtr->next;
	  }
	}
	else {
	  struct childList *tempPtr = n->childHead;

	  while (tempPtr != NULL) {
	    int cIndex = tempPtr->childIndex;
	    if (circuit[cIndex]->flag) {
	      n->vr = 0;
	      break;
	    }
	    else {
	      n->vr *= circuit[cIndex]->vr;
	    }
	    tempPtr = tempPtr->next;
	  }
	}
      }
      //printf("node type: %c, vr: %lf, index: %d, flag %d\n", n->nodeType, n->vr, index, n->flag);
      circuit[index] = n;
      index++;   
    }
  }

  /*Print out circuit output*/
  printf("output %lf for %d nodes\n", circuit[index]->vr, index);

  printf("log: %lf\n", log10(circuit[index]->vr));
  
  if (circuit[index]->vr == 0) {
    assert(0);
  }

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

  printf("\t... done ... \n");
  
  return (EXIT_SUCCESS);
}
  
