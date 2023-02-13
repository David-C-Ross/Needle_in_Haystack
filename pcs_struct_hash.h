#ifndef PCS_STRUCT_HASH_H
#define PCS_STRUCT_HASH_H

#include <gmp.h>
#include <inttypes.h>

typedef struct hashUNIX
{
    char *key;
    char *a_;
    int length;
    struct hashUNIX *next;

}hashUNIX_t;

hashUNIX_t ** struct_init_hash(uint8_t n, uint8_t memory);
int struct_add_hash(hashUNIX_t **table, mpz_t a_out, mpz_t a_in, int length, char xDist[]);
void struct_free_hash(hashUNIX_t **table);

#endif