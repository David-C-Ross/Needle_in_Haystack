#include <stdlib.h> /* exit */
#include <inttypes.h>
#include <gmp.h>
#include <time.h>

static uint8_t nb_bits;
static uint8_t prob;
static uint32_t table_size;

uint32_t jenkins(uint32_t a)
{
    a = (a+0x7ed55d16) + (a<<12);
    a = (a^0xc761c23c) ^ (a>>19);
    a = (a+0x165667b1) + (a<<5);
    a = (a+0xd3a2646c) ^ (a<<9);
    a = (a+0xfd7046c5) + (a<<3);
    a = (a^0xb55a4f09) ^ (a>>16);
    return a;
}

uint32_t hashint( uint32_t a)
{
    a += ~(a<<15);
    a ^=  (a>>10);
    a +=  (a<<3);
    a ^=  (a>>6);
    a += ~(a<<11);
    a ^=  (a>>16);
    return a;
}

void get_offset(mpz_t point, mpz_t offset, mpz_t next) {
    mpz_xor(next, point, offset);
}

/** Evaluate the function f at a given point.
 *
 * 	@param[in]	point	    Current point.
 */
void f(mpz_t point, mpz_t flavor) {
    mpz_t temp;
    get_offset(point, flavor, point);

    uint32_t p = mpz_get_ui(point);
    uint32_t j = jenkins(p);
    uint32_t i = hashint(j);
    mpz_init_set_ui(temp, j);

    if (mpz_divisible_2exp_p(temp, prob)) {
        mpz_set_ui(point, 1);
    } else {
        i = i & (table_size - 1);
        mpz_set_ui(point, i);
        //mpz_fdiv_r_2exp(point, temp, nb_bits);
    }
    mpz_clear(temp);
}

void init_f(uint8_t n_init, uint8_t prob_init) {
    nb_bits = n_init;
    prob = prob_init;
    table_size = 1 << nb_bits;
}
