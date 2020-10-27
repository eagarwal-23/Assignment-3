#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

#include <stdlib.h>

typedef struct SymTable *SymTable_T;

SymTable_T SymTable_new(void);

void SymTable_free (SymTable_T symTable);

size_t SymTable_getLength(SymTable_T symTable);

int SymTable_put(SymTable_T symTable, const char *key,
                 const void *value);

void *SymTable_replace(SymTable_T symTable, const char *key,
                       const void *value);

int SymTable_contains(SymTable_T symTable, const char *key);

void *SymTable_get(SymTable_T symTable, const char *key);

void *SymTable_remove(SymTable_T symTable, const char *key);

void SynTable_map(SymTable_T symTable, void (functionApply)
                  (const char *key, void *value, void *extra),
                  const void *extra);

#endif
