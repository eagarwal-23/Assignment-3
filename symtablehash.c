/* *******************************************************************
 *  Name:    Eesha Agarwal
 *  NetID:   eagarwal
 *  Precept: P08
 *  Filename: symtablehash.c
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

/* Declaring an enum to hold the maximum number of buckets. */
enum{MAX_BUCKETS = 65521};

/* Declaring a global variable to hold the different bucket counts. */
static size_t bucketCounts[] = {509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};

/* Each item is stored in an STNode. STNodes are linked to form a list.  */
struct STNode {
   /* The String key (array of chars). */
   const char *key;

   /* The address of the value. */
   const void *value;

   /* The address of the next STNode. */
   struct STNode *next;
};

/* A SymTable structure is a 'manager' structure that contains
   an array of linked lists, the total number of bindings, the
   number of buckets presently in the array, as well as a
   bucket count index used when rehashing the symbol table.  */
struct SymTable {
   /* The addresses of the first nodes of each linked lists
      in the array. */
   struct STNode **bucketsArray;

   /* The number of bindings (key-value pairs) presently in
      the symbol table. */
   int numBindings;

   /* The number of buckets (array elements) in the array of
      linked lists presently. */
   size_t numBuckets;

   /* The bucket count index, used to keep track of where in
      the array of bucket counts the symbol table presently is. */
   int bucketCountIndex;
};

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount) {
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

/* Dynamically increases the number of buckets and repositions
   all bindings for symTable. */
static void SymTable_rehash(SymTable_T symTable) {
   struct STNode **oldBucketsArray, **newBucketsArray;
   struct STNode *currentNode, *newNode, *temporaryNode;
   int i, oldNumBuckets, newNumBuckets, newBucketCountIndex;
   size_t hashCode;

   assert(symTable != NULL);
   oldNumBuckets = (int)symTable->numBuckets;
   oldBucketsArray = symTable->bucketsArray;
   newBucketCountIndex = symTable->bucketCountIndex + 1;
   newNumBuckets = (int)bucketCounts[newBucketCountIndex];

   /* Dynamically allocating memory for the new, longer array of
      linked lists. */
   newBucketsArray = (struct STNode **)calloc((size_t)newNumBuckets,
                                              sizeof(struct STNode *));
   /* Terminate the function if there isn't enough space for
      the longer array of linked lists. */
   if (newBucketsArray == NULL) {
      return ;
   }

   /* Running a loop over all the nodes present in the symbol
      table. */
   for (i = 0; i < oldNumBuckets; i++) {
      currentNode = oldBucketsArray[i];
      while (currentNode != NULL) {
         /* Dynamically allocating memory for the new node. */
         newNode = (struct STNode*)malloc(sizeof(struct STNode));
         if (newNode == NULL) {
            return ;
         }
         /* Dynamically allocating memory for the defensive
            copy of the key of the new node. */
         newNode->key = (char *)malloc(strlen(currentNode->key) + 1);
         if (newNode->key == NULL) {
            return ;
         }

         /* Calculating the hashcode for the current key basis
            the new number of buckets. */
         hashCode = SymTable_hash(currentNode->key,
                                  ((size_t)newNumBuckets));

         /* Creating a defensive copy of the key by copying it
            into the key for the newNode. */
         strcpy((char*)newNode->key, currentNode->key);
         newNode->value = currentNode->value;

         /* Adding the new node to the beginning of the linked
            list for the determined bucket in the array of linked
            lists. */
         newNode->next = newBucketsArray[hashCode];
         newBucketsArray[hashCode] = newNode;
         temporaryNode = currentNode->next;
         currentNode = temporaryNode;
      }
   }

   /* If there is sufficient space for all the new nodes and
      they are all successfully repositioned, then freeing
      the old array of linked lists. */
   free(symTable->bucketsArray);

   /* Assigning the newly created bucket array to the symbol
      table, and correspondingly updating its number of buckets
      and bucket count index. */
   symTable->bucketsArray = newBucketsArray;
   symTable->numBuckets = (size_t)newNumBuckets;
   symTable->bucketCountIndex = newBucketCountIndex;
}

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

   /* Allocating memory for the array of linked lists, freeing
      the symTable structure and returning ULL if there isn't
      enough memory available. */
   symTable->bucketsArray = (struct STNode **)calloc(bucketCounts[0],
                                                     sizeof(struct STNode *));

   if (symTable->bucketsArray == NULL) {
      free(symTable);
      return NULL;
   }

   symTable->numBindings = 0;
   symTable->bucketCountIndex = 0;
   symTable->numBuckets = bucketCounts[0];

   return symTable;
}

/* Frees all memory occupied by symTable. */
void SymTable_free(SymTable_T symTable) {
   struct STNode *currentNode;
   struct STNode *temporaryNode;
   int hashCode;

   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);

   /* Iterating over all nodes in symTable and
      freeing memory occupied by defensive copies of
      keys and by STNodes in symTable. */
   for (hashCode = 0; hashCode < (int)symTable->numBuckets; hashCode++) {
      currentNode = symTable->bucketsArray[hashCode];
      while (currentNode != NULL) {
         if (currentNode->key != NULL) {
            free((char *)currentNode->key);
         }

         temporaryNode = currentNode;
         currentNode = currentNode->next;
         free(temporaryNode);
      }
   }

   /* Frees bucketsArray of symTable. */
   free(symTable->bucketsArray);

   /* Frees symTable structure. */
   free(symTable);
}

/* Returns number of bindings (key-value pairs) in symTable. */
size_t SymTable_getLength(SymTable_T symTable) {
   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);
   return (size_t)symTable->numBindings;
}

/* If key is already in symTable, leaves symTable unchanged
   and returns 0. If insufficient memory is available,
   returns 0. If symTable does not contain a binding
   with key, then adds a new binding to symTable consisting
   of key and value, and returns 1. */
int SymTable_put(SymTable_T symTable, const char* key,
                 const void *value) {
   struct STNode *currentNode;
   struct STNode *newNode;
   size_t hashCode;

   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);
   assert(key != NULL);

   /* Calculating the hashcode for the input key basis
      the number of buckets. */
   hashCode = SymTable_hash(key, symTable->numBuckets);

   /* Setting the currentNode equal to the first STNode
      at index hashCode of bucketsArray. */
   currentNode = symTable->bucketsArray[hashCode];

   /* Iterating over current linked list of STNodes
      to check if binding with key already exists. */
   while (currentNode != NULL) {
      if (strcmp(currentNode->key, key) == 0) {
         return 0;
      }
      currentNode = currentNode->next;
   }

   /* Dynamically allocating memory for the new node. */
   newNode = (struct STNode*)malloc(sizeof(struct STNode));
   if (newNode == NULL) {
      return 0;
   }

   /* Dynamically allocating memory for the defensive
      copy of the key of the new node. */
   newNode->key = (char *)malloc(strlen(key) + 1);
   if (newNode->key == NULL) {
      return 0;
   }

   /* Incrementing the number of bindings if the
      key is not in symTable and if there is
      sufficient memory. */
   symTable->numBindings++;

   /* Rehashing the symTable if the number of bindings
      exceeds the current number of buckets, up until
      the max number of buckets (=65521), and
      correspondingly updating the hashCode basis the
      new number of buckets. */
   if ((symTable->numBindings > (int)symTable->numBuckets) &&
       ((int)symTable->numBuckets < (int) MAX_BUCKETS)) {
      SymTable_rehash(symTable);
      hashCode = SymTable_hash(key, symTable->numBuckets);
   }

   /* Copying the input key into the key of the newNode
      (defensive copy). */
   strcpy((char*)newNode->key, key);
   newNode->value = value;

   /* Adding the newNode to the beginning of the
      linked list for that particular bucket. */
   newNode->next = symTable->bucketsArray[hashCode];
   symTable->bucketsArray[hashCode] = newNode;

   return 1;
}

/* If symTable contains a binding whose key is input parameter key, return
   its corresponding value and replace the value with input parameter value.
   Else if there is no binding whose key is input parameter key, leave
   symTable unchanged and return NULL. */
void *SymTable_replace(SymTable_T symTable, const char *key, const void *value) {
   struct STNode *currentNode;
   void *oldValue;
   size_t hashCode;

   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);
   assert(key != NULL);

   /* Calculating the hashcode for the input key basis
      the number of buckets. */
   hashCode = SymTable_hash(key, symTable->numBuckets);

   /* Setting the currentNode equal to the first STNode
      at index hashCode of bucketsArray. */
   currentNode = symTable->bucketsArray[hashCode];

   /* Iterating over linked list of STNodes to check if
      binding with input key exists. If so, return
      corresponding value, and replace it with
      input value. */
   while (currentNode != NULL) {
      if (strcmp(currentNode->key, key) == 0) {
         oldValue = (void *)currentNode->value;
         currentNode->value = value;
         return oldValue;
      }
      currentNode = currentNode->next;
   }

   /* Else, return NULL. */
   return NULL;
}

/* If symTable contains a binding whose key is input parameter key,
   return 1. Else if there is no binding whose key is input
   parameter key, return 0. symTable is unchanged. */
int SymTable_contains(SymTable_T symTable, const char *key) {
   struct STNode *currentNode;
   size_t hashCode;

   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);
   assert(key != NULL);

   /* Calculating the hashcode for the input key basis
      the number of buckets. */
   hashCode = SymTable_hash(key, symTable->numBuckets);

   /* Setting the currentNode equal to the first STNode
      at index hashCode of bucketsArray. */
   currentNode = symTable->bucketsArray[hashCode];

   /* Iterating over linked list of STNodes to check if
      binding with input key exists. If so, return 1. */
   while (currentNode != NULL) {
      if (strcmp(currentNode->key, key) == 0) {
         return 1;
      }
      currentNode = currentNode->next;
   }

   /* Else, return 0. */
   return 0;
}

/* If symTable contains a binding whose key is input parameter key,
   return corresponding value. Else, return NULL. symTable is unchanged. */
void *SymTable_get(SymTable_T symTable, const char *key) {
   struct STNode *currentNode;
   size_t hashCode;

   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);
   assert(key != NULL);

   /* Calculating the hashcode for the input key basis
      the number of buckets. */
   hashCode = SymTable_hash(key, symTable->numBuckets);

   /* Setting the currentNode equal to the first STNode
      at index hashCode of bucketsArray. */
   currentNode = symTable->bucketsArray[hashCode];

   /* Iterating over linked list of STNodes to check if
      binding with input key exists. If so, return
      corresponding value. */
   while (currentNode != NULL) {
       if (strcmp(currentNode->key, key) == 0) {
         return (void *)currentNode->value;
      }
      currentNode = currentNode->next;
   }

   /* Else, return NULL. */
   return NULL;
}

/* If symTable contains a binding with input key, remove that
   binding from symTable and return the binding's value. Else,
   leave symTable unchanged and return NULL. */
void *SymTable_remove(SymTable_T symTable, const char *key) {
   struct STNode *currentNode;
   struct STNode *previousNode;
   struct STNode *temporaryNode;
   void *value;
   size_t hashCode;

   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);
   assert(key != NULL);

   /* Calculating the hashcode for the input key basis
      the number of buckets. */
   hashCode = SymTable_hash(key, symTable->numBuckets);

   /* Setting the currentNode equal to the first STNode
      at index hashCode of bucketsArray. */
   currentNode = symTable->bucketsArray[hashCode];

   /* If symTable is empty, return NULL. */
   if (currentNode == NULL) {
      return NULL;
   }

   /* If the first node contains the key. */
   if (strcmp(currentNode->key, key) == 0) {
      temporaryNode = currentNode;
      value = (void *)temporaryNode->value;
      symTable->bucketsArray[hashCode] = temporaryNode->next;
   }
   /* If symTable is not empty, and the firstNode does not contain
      the key, traversing the list while key is not found and end
      of the symTable is not reached. */
  else {
      while (currentNode != NULL &&
             (strcmp(currentNode->key, key) != 0)) {
         previousNode = currentNode;
         currentNode = currentNode->next;
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
      previousNode->next = temporaryNode->next;
      free(temporaryNode);
   }

   /* Decrementing the number of bindings in symTable. */
   symTable->numBindings--;
   return value;
}

/* Applying the function functionApply to each binding in symTable,
   passing extra as an extra parameter, that is, calling
   (*functionApply)((const char*)currentNode->key,
                    (void*)currentNode->value, (void*)extra) for
                    each key-value binding in symTable. */
void SymTable_map(SymTable_T symTable,
                  void (*functionApply)(const char *key,  void *value,
                                        void *extra), const void *extra) {
   struct STNode *currentNode;
   int hashCode;

   /* Ensuring that the input parameters are not null. */
   assert(symTable != NULL);
   assert(functionApply != NULL);

   /* Iterating over all nodes in symTable and
      applying the function to each key-value
      pair. */
   for (hashCode = 0; hashCode < (int)symTable->numBuckets; hashCode++) {
      for (currentNode = symTable->bucketsArray[hashCode];
           currentNode != NULL;
           currentNode = currentNode->next) {
         (*functionApply)((const char*)currentNode->key,
                          (void*)currentNode->value, (void*)extra);
      }
   }
}
