#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pcs_struct_hash_UNIX.h"
#include "pcs_struct_hash.h"

static unsigned long int table_size;
static unsigned long long int memory_alloc;
/***Memory limiting feature is turned off
static unsigned long long int memory_limit;
 ***/


/** Calculate the hash table recommended size.
 *
 *	@brief The recommended size corresponds to the expected number
 *	of stored points.
 *
 */
void set_table_size(uint8_t memory)
{
    table_size = pow(2, memory);
}

/** Initialize the hash table and allocate memory.
 *
 */
hashUNIX_t ** struct_init_hash(uint8_t n, uint8_t memory)
{
    unsigned long int i;
    set_table_size(memory);

    //printf("\t\ttable_size: %lu\n",table_size);
    hashUNIX_t **table = malloc(sizeof(*table) * table_size); //i.e. sizeof(hashUNIX_t *)
    /***Memory limiting feature is turned off
    memory_limit = 100000000;
     ***/
    memory_alloc = 0LL;
    memory_alloc += sizeof(*table) * table_size;
    for(i = 0; i < table_size; i++)
    {
        table[i] = NULL;
    }
    return table;
}

unsigned long int get_hash(char *xDist)
{
    return (get_hash_UNIX(xDist) % table_size);
}

int struct_add_hash(hashUNIX_t **table, mpz_t a_out, mpz_t a_in, int length, char xDist[])
{
    unsigned long int h = 0;
    hashUNIX_t *new;
    hashUNIX_t *last;
    hashUNIX_t *next;
    int retval = 0;

    h = get_hash(xDist);
    next = table[h];
    /*
    while(next != NULL && next-> key != NULL && strncmp(xDist, next->key, strlen(xDist)) > 0)
    {
        last = next;
        next = next->next;
    }
     */

    if(next != NULL && next->key != NULL && strcmp(xDist, next->key) == 0) //collision
    {
        retval = next->length;
        mpz_set_str(a_out, next->a_, 62);

        next->a_ = mpz_get_str(next->a_, 62, a_in);
        next->length = length;

    }
    else
    {
        /***Memory limiting feature is turned off
        if(memory_alloc < memory_limit)
        {
         ***/
        new = malloc(sizeof(hashUNIX_t));
        new->key = strdup(xDist);
        new->a_ = NULL;
        new->a_ = mpz_get_str(new->a_, 62, a_in);
        new->length = length;
        new->next = NULL;

        table[h] = new;

        /*
        if(next == table[h]) //add at the beginning
        {
            new->next = next;
            table[h] = new;
        }
        else
        {
            if(next != NULL) //add in the middle
            {
                new->next = next;
            }
            last->next = new;
        }
         */
        memory_alloc += strlen(new->key) + 1;
        memory_alloc += strlen(new->a_) + 1;
        memory_alloc += sizeof(hashUNIX_t);
        //}
    }
    return retval;
}
void struct_free_hash(hashUNIX_t **table)
{
    unsigned long int i;
    hashUNIX_t *last;
    hashUNIX_t *next;
    for(i = 0; i < table_size; i++)
    {
        next = table[i];
        while(next != NULL)
        {
            free(next->key);
            free(next->a_);
            last = next;
            next = next->next;
            free(last);
        }
    }
    free(table);
}
