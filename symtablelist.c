#include <assert.h>
#include <string.h>
#include <malloc.h>
#include "symtable.h"

struct STNode {
   const char *key;
   const void *value;
   struct STNode *nextNode;
};

struct SymTable {
   struct STNode *firstNode;
   int length;
};

SymTable_T SymTable_new(void) {
   SymTable_T symTable;

   symTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (symTable == NULL) {
      return NULL;
   }

   (*symTable).firstNode = NULL;
   (*symTable).length = 0;
   return symTable;
}

void SymTable_free(SymTable_T symTable) {
   struct STNode *currentNode;

   assert(symTable != NULL);

   for (currentNode = (*symTable).firstNode;
        currentNode != NULL;
        currentNode = (*currentNode).nextNode) {

      free((char *)(*currentNode).key);
      free(currentNode);
   }

   free(symTable);
}

size_t SymTable_getLength(SymTable_T symTable) {
   assert(symTable != NULL);

   return (*symTable).length;
}

int SymTable_put(SymTable_T symTable, const char *key,
                 const void *value) {

   struct STNode *newNode;
   struct STNode *currentNode;

   assert(symTable != NULL);
   assert(key != NULL);

   /* Traverse ST for input key, if found, return 0. */
   for (currentNode = (*symTable).firstNode;
        currentNode != NULL;
        currentNode = (*currentNode).nextNode) {
      if (strcmp((*currentNode).key, key) == 0) {
         return 0;
      }
   }

   /* Asign memory for new node, if not enough space, return 0 and
      leave symbolTable unchanged. */
    newNode = (struct STNode*)malloc(sizeof(struct STNode));

    if (newNode == NULL) {
       return 0;
    }

    /* Allocating memory to create a defensive copy of the key. */
    (*newNode).key = (const char*)malloc(strlen(key) + 1);

    /* If there is insufficient memory for key, return 0. */
    if ((*newNode).key == NULL) {
       free(newNode);
       return 0;
    }

    /* Assigning input key and value to new node. */
    strcpy((char *)(*newNode).key, key);
    (*newNode).value = value;

    /* Add new node to the beginning of the list and increment size
       of list. */
    (*newNode).nextNode = (*symTable).firstNode;
    (*symTable).firstNode = newNode;
    (*symTable).length++;
    return 1;
}

void *SymTable_replace(SymTable_T symTable, const char *key,
                       const void *value) {
   struct STNode *currentNode;
   const void *oldValue;

   assert(symTable != NULL);
   assert(key != NULL);

   /* Traverse ST for input key, if found, return value. */
   for (currentNode = (*symTable).firstNode;
        currentNode != NULL;
        currentNode = (*currentNode).nextNode) {

      if (strcmp((*currentNode).key, key) == 0) {

         oldValue = (*currentNode).value;
         (*currentNode).value = value;
         return (void *)oldValue;

         }
      }

   return NULL;
}

int SymTable_contains(SymTable_T symTable, const char *key) {
   struct STNode *currentNode;

   for (currentNode = (*symTable).firstNode;
        currentNode != NULL;
        currentNode = (*currentNode).nextNode) {

      if (strcmp((*currentNode).key, key) == 0) {
         return 1;
      }

   }

   return 0;
}

void *SymTable_get(SymTable_T symTable, const char *key) {
   struct STNode *currentNode;

   assert(symTable != NULL);
   assert(key != NULL);

   for (currentNode = (*symTable).firstNode;
        currentNode != NULL;
        currentNode = (*currentNode).nextNode) {

      if (strcmp((*currentNode).key, key) == 0) {
         return (void *)(*currentNode).value;
      }
   }

   return NULL;
}

void *SymTable_remove(SymTable_T symTable, const char *key) {

   struct STNode *currentNode = (*symTable).firstNode;
   struct STNode *previousNode;
   struct STNode *temporaryNode;
   void *value;

   assert(symTable != NULL);
   assert(key != NULL);

   /* If symTable is empty. */
   if (currentNode == NULL) {
      return NULL;
   }

   /* If firstNode contains key, removing the node, returning the
      value, and setting the first node parameter of the symTable
      to the next node. */
   if (strcmp((*currentNode).key, key) == 0) {
      temporaryNode = currentNode;
      value = (void *)(*temporaryNode).value;
      (*symTable).firstNode = (*temporaryNode).nextNode;
   }
   /* If symTable is not empty, and the firstNode does not contain
      the key, traversing the list while key is not found and end
      of the symTable is not reached; */

   else {
      while (currentNode != NULL &&
            (strcmp((*currentNode).key, key) != 0)) {
         previousNode = currentNode;
         currentNode = (*currentNode).nextNode;
      }

      if (currentNode == NULL) {
         return NULL;
      }

      temporaryNode = currentNode;
      value = (void *)(*temporaryNode).value;
      (*previousNode).nextNode = (*temporaryNode).nextNode;

   }

      free((char *)((*temporaryNode).key));
      free(temporaryNode);

      (*symTable).length--;
      return value;
}

void SymTable_map(SymTable_T symTable, void (*functionApply)
                  (const char *key, void *value, void *extra),
                  const void *extra) {

   struct STNode *currentNode;

   assert(symTable != NULL);
   assert(functionApply != NULL);

   for (currentNode = (*symTable).firstNode;
        currentNode != NULL;
        currentNode = (*currentNode).nextNode) {

      (*functionApply)((const char*)(*currentNode).key,
                       (void*)(*currentNode).value,
                       (void*)extra);
   }
}
