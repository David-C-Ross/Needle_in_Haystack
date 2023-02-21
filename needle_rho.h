#ifndef PCS_NEEDLE_RHO_H
#define PCS_NEEDLE_RHO_H

#include <gmp.h>
#include <inttypes.h>

void rho_mode_detection(uint8_t n, uint8_t prob_init);
void nested_rho(mpz_t start_point, mpz_t collision);
void rho(mpz_t start_point, mpz_t collision);

#endif //PCS_NEEDLE_RHO_H
