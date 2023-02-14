#ifndef PCS_STRUCT_HASH_H
#define PCS_STRUCT_HASH_H

#include <gmp.h>
#include <inttypes.h>

typedef struct hashUNIX
{
    char *key;
    char *start;
    int length;

}hashUNIX_t;

typedef struct Table
{
    hashUNIX_t ** array;
    unsigned int memory_alloc;

}Table_t;

Table_t *struct_init_hash(uint8_t n, uint8_t memory);
int struct_add_hash(Table_t *table, mpz_t a_out, mpz_t a_in, int length, char xDist[]);
void struct_free_hash(Table_t *table);

#endif