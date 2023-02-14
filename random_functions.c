#include <stdlib.h> /* exit */
#include <inttypes.h>
#include <gmp.h>
#include <time.h>

static uint8_t nb_bits;
static mpz_t flavor;
static mpz_t threshold;

uint32_t jenkins( uint32_t a)
{
    a = (a+0x7ed55d16) + (a<<12);
    a = (a^0xc761c23c) ^ (a>>19);
    a = (a+0x165667b1) + (a<<5);
    a = (a+0xd3a2646c) ^ (a<<9);
    a = (a+0xfd7046c5) + (a<<3);
    a = (a^0xb55a4f09) ^ (a>>16);
    return a;
}

/** Evaluate the function f at a given point.
 *
 * 	@param[in]	point	    Current point.
 */
void f(mpz_t point) {
    mpz_t temp;
    mpz_init(temp);
    mpz_xor(point, point, flavor);

    if (mpz_cmp(point, threshold) < 0) {
        mpz_set_ui(point, 1);
    } else {
        mpz_set_ui(temp, jenkins(mpz_get_ui(point)));
        //srand(mpz_get_ui(point));
        //mpz_set_ui(temp, rand());
        mpz_mod_2exp(point, temp, nb_bits);
    }
    mpz_clear(temp);
}

void init_f(uint8_t n_init, mpz_t flavor_init, mpz_t threshold_init) {
    nb_bits = n_init;
    mpz_init_set(flavor, flavor_init);
    mpz_init_set(threshold, threshold_init);
}
