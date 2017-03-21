#ifndef CSPRNG_H
#define CSPRNG_H

#include <stdint.h>
#include "buffer.h"

#define csprng_state_size 200
#define csprng_seed_rate 136
#define csprng_gen_rate 136

/* cryptographically secure pseudorandom generator */
typedef struct
{
	unsigned char state[csprng_state_size];
	int chars_used;
} csprng;

int csprng_init(csprng* rng);
int csprng_seed(csprng* rng, uint64_t seed_length, const unsigned char * seed);
int csprng_generate(csprng* rng, uint64_t buffer_length, unsigned char * buffer);

void csprng_seed_uint64_t(csprng* rng, uint64_t seed);

uint64_t csprn_next_uint64_t(csprng* rng);
uint16_t csprn_next_uint16_t(csprng* rng);

#endif
