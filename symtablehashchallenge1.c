#include <assert.h>
#include <string.h>
#include <malloc.h>
#include "symtable.h"

enum{BUCKET_COUNT = 509};
enum{MAX_BUCKETS = 65521};
static size_t bucketCounts[] = {509, 1021, 2039, 4093, 8191, 16381,
                         32749, 65521};

struct STNode {
   const char* key;
   const void* value;
   struct STNode *next;
};

struct SymTable {
   struct STNode **bucketsArray;
   int numBindings;
   int numBuckets;
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

static void SymTable_rehash(SymTable_T symTable) {
   int oldBucketCount = symTable->numBuckets;
   int newBucketCount = bucketCounts[symTable->bucketCountIndex + 1];

   SymTable_T newSymTable;
   SymTable_T oldSymTable = symTable;

   int i, hashCode;

   struct STNode *currentNode;
   struct STNode *newNode;

   const char* key;
   const void* value;

   newSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (newSymTable != NULL) {
      
   }

   newSymTable->bucketsArray = (struct STNode **)calloc(newBucketCount, sizeof(struct STNode *));

   if (newSymTable->bucketsArray == NULL) {
      free(newSymTable);
      return NULL;
   }

   newSymTable->numBindings = 0;
   newSymTable->bucketCountIndex = oldSymTable->bucketCountIndex + 1;
   newSymTable->numBuckets = newBucketCount;

   for (i = 0; i < oldBucketCount; i++) {
      for (currentNode = oldSymTable->bucketsArray[i];
           currentNode != NULL;
           currentNode = currentNode->next) {
         key = currentNode -> key;
         value = currentNode -> value;

         newNode = (struct STNode*)malloc(sizeof(struct STNode));
         if (newNode == NULL) {
            return NULL;
         }

         newNode->key = (char *)malloc(strlen(key) + 1);
         if (newNode->key == NULL) {
            return NULL;
         }

         hashCode = SymTable_hash(key, newBucketCount);

         strcpy((char*)newNode->key, key);
         newNode->value = value;

         newNode->next = newSymTable->bucketsArray[hashCode];
         newSymTable->bucketsArray[hashCode] = newNode;
         newSymTable-> numBindings++;
         free(currentNode->key);
         free(currentNode);
      }
   }
   oldSymTable = newSymTable;
}

SymTable_T SymTable_new(void) {
   SymTable_T symTable;

   symTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (symTable == NULL) {
      return NULL;
   }

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

void SymTable_free(SymTable_T symTable) {
   struct STNode *currentNode;
   int hashCode;

   assert(symTable != NULL);
   for (hashCode = 0; hashCode < symTable->numBuckets; hashCode++) {
      for (currentNode = symTable->bucketsArray[hashCode];
           currentNode != NULL;
           currentNode = currentNode->next) {
         free((char *)currentNode->key);
         free(currentNode);
      }
   }

   free(symTable->bucketsArray);
   free(symTable);
}

size_t SymTable_getLength(SymTable_T symTable) {
   assert(symTable != NULL);
   return symTable->numBindings;
}

int SymTable_put(SymTable_T symTable, const char* key,
                 const void *value) {
   struct STNode *currentNode;
   struct STNode *newNode;
   SymTable_T tempSymTable;
   int hashCode;

   assert(symTable != NULL);
   assert(key != NULL);

   hashCode = SymTable_hash(key, symTable->numBuckets);

   currentNode = symTable->bucketsArray[hashCode];

   while (currentNode != NULL) {
      if (strcmp(currentNode->key, key) == 0) {
         return 0;
      }
      currentNode = currentNode->next;
   }

   newNode = (struct STNode*)malloc(sizeof(struct STNode));
   if (newNode == NULL) {
      return 0;
   }

   newNode->key = (char *)malloc(strlen(key) + 1);
   if (newNode->key == NULL) {
      return 0;
   }

   symTable->numBindings++;
   if ((symTable->numBindings > symTable->numBuckets) &&
       (symTable->numBuckets < (int)MAX_BUCKETS)) {

      tempSymTable = SymTable_rehash(symTable);

      if (tempSymTable != NULL) {
         symTable = tempSymTable;
         hashCode = SymTable_hash(key, symTable->numBuckets);
      }
   }

   strcpy((char*)newNode->key, key);
   newNode->value = value;

   newNode->next = symTable->bucketsArray[hashCode];
   symTable->bucketsArray[hashCode] = newNode;

   return 1;
}

void *SymTable_replace(SymTable_T symTable, const char *key, const void *value) {
   struct STNode *currentNode;
   void *oldValue;
   int hashCode;

   assert(symTable != NULL);
   assert(key != NULL);

   hashCode = SymTable_hash(key, symTable->numBuckets);
   currentNode = symTable->bucketsArray[hashCode];

   while (currentNode != NULL) {
      if (strcmp(currentNode->key, key) == 0) {
         oldValue = (void *)currentNode->value;
         currentNode->value = value;
         return oldValue;
      }
      currentNode = currentNode->next;
   }

   return NULL;
}

int SymTable_contains(SymTable_T symTable, const char *key) {
   struct STNode *currentNode;
   int hashCode;

   assert(symTable != NULL);
   assert(key != NULL);

   hashCode = SymTable_hash(key, symTable->numBuckets);
   currentNode = symTable->bucketsArray[hashCode];

   while (currentNode != NULL) {
      if (strcmp(currentNode->key, key) == 0) {
         return 1;
      }
      currentNode = currentNode->next;
   }
   return 0;
}

void *SymTable_get(SymTable_T symTable, const char *key) {
   struct STNode *currentNode;
   int hashCode;

   assert(symTable != NULL);
   assert(key != NULL);

   hashCode = SymTable_hash(key, symTable->numBuckets);
   currentNode = symTable->bucketsArray[hashCode];

   while (currentNode != NULL) {
      if (strcmp(currentNode->key, key) == 0) {
         return (void *)currentNode->value;
      }
      currentNode = currentNode->next;
   }

   return NULL;
}

void *SymTable_remove(SymTable_T symTable, const char *key) {
   struct STNode *currentNode;
   struct STNode *previousNode;
   struct STNode *temporaryNode;
   void *value;
   int hashCode;

   assert(symTable != NULL);
   assert(key != NULL);

   hashCode = SymTable_hash(key, symTable->numBuckets);
   currentNode = symTable->bucketsArray[hashCode];

   /* If symTable is empty. */
   if (currentNode == NULL) {
      return NULL;
   }

   /* If the first node contains the key. */
   if (strcmp(currentNode->key, key) == 0) {
      temporaryNode = currentNode;
      value = (void *)temporaryNode->value;
      symTable->bucketsArray[hashCode] = temporaryNode->next;
   }
   /* If the first node does not contain the key. */
   else {
      while (currentNode != NULL && (strcmp(currentNode->key, key) != 0)) {
         previousNode = currentNode;
         currentNode = currentNode->next;
      }

      /* If key is not present in symbol table. */
      if (currentNode == NULL) {
         return NULL;
      }

      temporaryNode = currentNode;
      value = (void *)temporaryNode->value;
      previousNode->next = temporaryNode->next;
   }

   free((char *)temporaryNode->key);
   free(temporaryNode);
   symTable->numBindings--;
   return value;
}

void SymTable_map(SymTable_T symTable,
                  void (*functionApply)(const char *key,  void *value,
                                        void *extra), const void *extra) {
   struct STNode *currentNode;
   int hashCode;

   assert(symTable != NULL);

   for (hashCode = 0; hashCode < symTable->numBuckets; hashCode++) {
      for (currentNode = symTable->bucketsArray[hashCode];
           currentNode != NULL;
           currentNode = currentNode->next) {
         (*functionApply)((const char*)currentNode->key,
                          (void*)currentNode->value, (void*)extra);
      }
   }
}
