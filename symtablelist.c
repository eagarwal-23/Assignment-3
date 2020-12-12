/* *******************************************************************
 *  Name:    Eesha Agarwal
 *  NetID:   eagarwal
 *  Precept: P08
 *  Filename: symtablelist.c
 *
 *  Description: Implements a SymTable data type (collection of key-
 *  -value bindings) with functions to allow its client to insert (put)
 *  new bindings, to retrieve (get) the values of bindings with
 *  specified keys, and to remove bindings with specified keys. This
 *  ADT additionally allows for a SymTable structure to be freed if
 *  it is no longer to be used, for a function to be applied to all
 *  key-value pairs, for the value for a specified key to be replaced,
 *  and for checking if a SymTable contains a given key.
 ******************************************************************* */

#include <assert.h>
#include <string.h>
#include <malloc.h>
#include "symtable.h"

/* Each item is stored in an STNode. STNodes are linked to form a list.  */
struct STNode {
   /* The String key (array of chars). */
   const char *key;

   /* The address of the value. */
   const void *value;

   /* The address of the next STNode. */
   struct STNode *nextNode;
};

/* A SymTable structure is a 'manager' structure that contains
   an array of linked lists, and the total number of bindings,
   i.e. the length of the symTable. */
struct SymTable {
   /* The address of the firstNode in symTable. */
   struct STNode *firstNode;

   /* The length of the linked list, i.e. the
      number of bindings in symTable. */
   int length;
};

/* Returns a new SymTable object that contains no bindings,
   or NULL if there is insufficient memory available. */
SymTable_T SymTable_new(void) {
   SymTable_T symTable;

   /* Allocating memory for the SymTable structure, returning
      NULL if there isn't enough memory available. */
   symTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (symTable == NULL) {
      return NULL;
   }

   symTable->firstNode = NULL;
   symTable->length = 0;
   return symTable;
}

/* Frees all memory occupied by symTable. */
void SymTable_free(SymTable_T symTable) {
   struct STNode *currentNode, *temporaryNode;

   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);

   /* Iterating over all nodes in symTable and
      freeing memory occupied by defensive copies of
      keys and by STNodes in symTable. */
   currentNode = symTable->firstNode;
   while (currentNode != NULL) {

      if (currentNode->key != NULL) {
         free((char *)currentNode->key);
      }

      temporaryNode = currentNode;
      currentNode = currentNode->nextNode;
      free(temporaryNode);
   }

   /* Frees symTable structure. */
   free(symTable);
}

/* Returns number of bindings (key-value pairs) in symTable. */
size_t SymTable_getLength(SymTable_T symTable) {
   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);
   return (size_t)symTable->length;
}

/* If key is already in symTable, leaves symTable unchanged
   and returns 0. If insufficient memory is available,
   returns 0. If symTable does not contain a binding
   with key, then adds a new binding to symTable consisting
   of key and value, and returns 1. */
int SymTable_put(SymTable_T symTable, const char *key,
                 const void *value) {

   struct STNode *newNode;
   struct STNode *currentNode;

   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);
   assert(key != NULL);

   /* Iterating over the linked list of STNodes
      to check if binding with key already exists. */
   for (currentNode = (*symTable).firstNode;
        currentNode != NULL;
        currentNode = (*currentNode).nextNode) {
      if (strcmp((*currentNode).key, key) == 0) {
         return 0;
      }
   }

   /* Dynamically allocating memory for the new node. */
   newNode = (struct STNode*)malloc(sizeof(struct STNode));

   if (newNode == NULL) {
      return 0;
   }

   /* Dynamically allocating memory for the defensive
      copy of the key of the new node. */
   newNode->key = (const char*)malloc(strlen(key) + 1);

   if (newNode->key == NULL) {
      free(newNode);
      return 0;
   }

   /* Copying the input key into the key of the newNode
      (defensive copy). */
   strcpy((char *)(*newNode).key, key);
   newNode->value = value;

   /* Adding the newNode to the beginning of the
      linked list for that particular bucket. */
   newNode->nextNode = symTable->firstNode;
   symTable->firstNode = newNode;

   /* Incrementing the number of bindings if the
       key is not in symTable and if there is
       sufficient memory. */
   symTable->length++;
   return 1;
}

/* If symTable contains a binding whose key is input parameter key, return
   its corresponding value and replace the value with input parameter value.
   Else if there is no binding whose key is input parameter key, leave
   symTable unchanged and return NULL. */
void *SymTable_replace(SymTable_T symTable, const char *key,
                       const void *value) {
   struct STNode *currentNode;
   const void *oldValue;

   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);
   assert(key != NULL);

   /* Iterating over linked list of STNodes to check if
      binding with input key exists. If so, return
      corresponding value, and replace it with
      input value. */
   for (currentNode = symTable->firstNode;
        currentNode != NULL;
        currentNode = currentNode->nextNode) {

      if (strcmp(currentNode->key, key) == 0) {

         oldValue = currentNode->value;
         currentNode->value = value;
         return (void *)oldValue;

      }
   }

   /* Else, return NULL. */
   return NULL;
}

/* If symTable contains a binding whose key is input parameter key,
   return 1. Else if there is no binding whose key is input
   parameter key, return 0. symTable is unchanged. */
int SymTable_contains(SymTable_T symTable, const char *key) {
   struct STNode *currentNode;

   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);
   assert(key != NULL);

   /* Iterating over linked list of STNodes to check if
      binding with input key exists. If so, return 1. */
   for (currentNode = symTable->firstNode;
        currentNode != NULL;
        currentNode = currentNode->nextNode) {

      if (strcmp(currentNode->key, key) == 0) {
         return 1;
      }

   }

   /* Else, return 0. */
   return 0;
}

/* If symTable contains a binding whose key is input parameter key,
   return corresponding value. Else, return NULL. symTable is unchanged. */
void *SymTable_get(SymTable_T symTable, const char *key) {
   struct STNode *currentNode;

   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);
   assert(key != NULL);

   /* Iterating over linked list of STNodes to check if
      binding with input key exists. If so, return
      corresponding value. */
   for (currentNode = symTable->firstNode;
        currentNode != NULL;
        currentNode = currentNode->nextNode) {

      if (strcmp(currentNode->key, key) == 0) {
         return (void *)currentNode->value;
      }
   }

   /* Else, return NULL. */
   return NULL;
}

/* If symTable contains a binding with input key, remove that
   binding from symTable and return the binding's value. Else,
   leave symTable unchanged and return NULL. */
void *SymTable_remove(SymTable_T symTable, const char *key) {

   struct STNode *currentNode = (*symTable).firstNode;
   struct STNode *previousNode;
   struct STNode *temporaryNode;
   void *value;

   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);
   assert(key != NULL);

   /* If symTable is empty, return NULL. */
   if (currentNode == NULL) {
      return NULL;
   }

   /* If firstNode contains key, removing the node, returning the
      value, and setting the first node parameter of the symTable
      to the next node. */
   if (strcmp(currentNode->key, key) == 0) {
      temporaryNode = currentNode;
      value = (void *)temporaryNode->value;
      symTable->firstNode = temporaryNode->nextNode;
   }

/* If symTable is not empty, and the firstNode does not contain
      the key, traversing the list while key is not found and end
      of the symTable is not reached. */
   else {
      while (currentNode != NULL &&
             (strcmp((*currentNode).key, key) != 0)) {
         previousNode = currentNode;
         currentNode = (*currentNode).nextNode;
      }

      /* If key is not present in symbol table. */
      if (currentNode == NULL) {
         return NULL;
      }

      /* Freeing the currentNode and its key. */
      temporaryNode = currentNode;

      /* Assigning the value to be returned to the
         value of the currentNode, and the next node for
         currentNode to be the next node for its
         previous node. */
      value = (void *)temporaryNode->value;
      free((char *)temporaryNode->key);
      previousNode->nextNode = temporaryNode->nextNode;
      free(temporaryNode);
   }

   /* Decrementing the number of bindings in symTable. */
   (*symTable).length--;
   return value;
}


/* Applying the function functionApply to each binding in symTable,
   passing extra as an extra parameter, that is, calling
   (*functionApply)((const char*)currentNode->key,
   (void*)currentNode->value, (void*)extra) for
   each key-value binding in symTable. */
void SymTable_map(SymTable_T symTable, void (*functionApply)
                  (const char *key, void *value, void *extra),
                  const void *extra) {

   struct STNode *currentNode;

   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);
   assert(functionApply != NULL);

   /* Iterating over all nodes in symTable and
      applying the function to each key-value
      pair. */
   for (currentNode = (*symTable).firstNode;
        currentNode != NULL;
        currentNode = (*currentNode).nextNode) {

      (*functionApply)((const char*)(*currentNode).key,
                       (void*)(*currentNode).value,
                       (void*)extra);
   }
}
