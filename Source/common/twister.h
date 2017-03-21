#ifndef TWISTER_H
#define TWISTER_H

#include <stdint.h>

/* Mersenne Twister */
typedef struct{
	uint32_t state[625];
	uint32_t   *next;     /* next random value is computed from here */
	int      left;      /* can *next++ this many times before reloading */
} twister;

void seedMT(twister* MT, uint32_t seed);
uint32_t randomMT(twister* MT);

#endif
