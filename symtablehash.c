#include <assert.h>
#include <string.h>
#include <malloc.h>
#include "symtable.h"

enum{BUCKET_COUNT = 509};

struct STNode {
   const char* key;
   const void* value;
   struct STNode *next;
};

struct SymTable {
   struct STNode **bucketsArray;
   int numBindings;
   int numBuckets;
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

SymTable_T SymTable_new(void) {
   SymTable_T symTable;

   symTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (symTable == NULL) {
      return NULL;
   }

   symTable->bucketsArray = (struct STNode **)calloc(BUCKET_COUNT,
                                              sizeof(struct STNode *));

   if (symTable->bucketsArray == NULL) {
      return NULL;
   }

   symTable->numBindings = 0;
   symTable->numBuckets = BUCKET_COUNT;

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

   strcpy((char*)newNode->key, key);
   newNode->value = value;

   newNode->next = symTable->bucketsArray[hashCode];
   symTable->bucketsArray[hashCode] = newNode;
   symTable->numBindings++;
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
