#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "pcs_struct_hash.h"

static unsigned int table_size;

/** Initialize the hash table and allocate memory.
 *
 */
Table_t *structInitHash(uint8_t memory) {

    unsigned int i;
    table_size = (int) pow(2, memory);

    //printf("\t\ttable_size: %lu\n",table_size);
    Table_t *table = malloc(sizeof(Table_t));
    hashUNIX_t **array = malloc(sizeof(*array) * table_size); //i.e. sizeof(hashUNIX_t *)

    for (i = 0; i < table_size; i++) {
        array[i] = NULL;
    }

    table->array = array;
    //Memory limiting feature is turned on
    table->memory_alloc = 0;

    return table;
}

int structAddHash(Table_t *table, mpz_t a_out, mpz_t a_in, int length, char xDist[]) {
    unsigned int i;
    hashUNIX_t *new;
    hashUNIX_t *next;
    int retval = 0;

    for (i = 0; i < table_size; ++i) {
        next = table->array[i];

        if (next != NULL && next->key != NULL && strcmp(xDist, next->key) == 0) //findCollision
        {
            retval = next->length;
            mpz_set_str(a_out, next->start, 62);

            next->start = mpz_get_str(next->start, 62, a_in);
            next->length = length;

            return retval;
        }
    }
    // add new element to table
    new = malloc(sizeof(hashUNIX_t));
    new->key = strdup(xDist);
    new->start = NULL;
    new->start = mpz_get_str(new->start, 62, a_in);
    new->length = length;

    if (table->memory_alloc < table_size) {
        table->array[table->memory_alloc] = new;
    } else {
        i = table->memory_alloc % table_size;
        free(table->array[i]->key);
        free(table->array[i]->start);
        free(table->array[i]);

        table->array[i] = new;
    }
    table->memory_alloc ++;

    return retval;
}

void structFreeHash(Table_t *table) {
    unsigned int i;
    for (i = 0; i < table_size; i++) {
        if (table->array[i] != NULL) {
            free(table->array[i]->key);
            free(table->array[i]->start);
            free(table->array[i]);
        }
    }
    free(table->array);
    free(table);
}
