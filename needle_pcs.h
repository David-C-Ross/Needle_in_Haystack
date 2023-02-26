#ifndef PCS_NEEDLE_PCS_H
#define PCS_NEEDLE_PCS_H

#include <gmp.h>
#include <inttypes.h>

int pcsPcsModeDetection(uint8_t n, uint8_t memory_init, uint8_t prob_init);
int pcsRhoModeDetection(uint8_t n, uint8_t memory_init, uint8_t prob_init);

#endif