#include "csprng.h"
#include "LinearAlgebra.h"

void KeccakF1600_StatePermute(void *state);

/**
* csprng_init
* Set the csprng state to all zeros.
*/
int csprng_init(csprng* rng)
{
	unsigned short int i;
	for (i = 0; i < csprng_state_size; ++i)
	{
		rng->state[i] = 0;
	}
	rng->chars_used = 0;

	return 1;
}

/**
* csprng_seed
* Randomize the csprng's state by feeding it a seed.
* @params
*  * rng : the csprng object, which contains a state
*  * seed_length : unsigned short int describing the length (in
*    bytes) of the seed
*  * seed : pointer to the byte array of the seed
*/
int csprng_seed(csprng* rng, uint64_t seed_length, const unsigned char * seed)
{
	unsigned short int i, j;

	/* Absorb all the full input blocks */
	for (i = 0; i < seed_length / csprng_seed_rate; ++i)
	{
		for (j = 0; j < csprng_seed_rate; ++j)
		{
			rng->state[j] ^= seed[i*csprng_seed_rate + j];
		}
		KeccakF1600_StatePermute(rng->state);
	}

	/* Absorb the remainder of the last input block */
	for (j = 0; j < seed_length % csprng_seed_rate; ++j)
	{
		rng->state[j] ^= seed[i*csprng_seed_rate + j];
	}
	KeccakF1600_StatePermute(rng->state);

	return 1;
}

/**
* csprng_generate
* Generate a string of random bytes of the given length from the
* csprng.
* @params
*  * rng : the csprng object which contains a randomized state
*  * buffer_length : the number of random output bytes to produce
*  * buffer : the memory address to store the desired random bytes
*    to
*/
int csprng_generate(csprng* rng, uint64_t buffer_length, unsigned char * buffer)
{
	unsigned short int i, j;

	/* squeeze out all the full output blocks */
	for (i = 0; i < buffer_length / csprng_gen_rate; ++i)
	{
		for (j = 0; j < csprng_gen_rate; ++j)
		{
			buffer[i*csprng_gen_rate + j] = rng->state[j];
		}
		KeccakF1600_StatePermute(rng->state);
	}
	/* squeeze out the remaining bytes of the last output block */
	for (j = 0; j < buffer_length % csprng_gen_rate; ++j)
	{
		buffer[i*csprng_gen_rate + j] = rng->state[j];
	}
	/*KeccakF1600_StatePermute(rng->state);*/

	return 1;
}

/*
	Generates a uint64_t

	rng : The pseudorandom number generator

	returns : a pseudorandomly generated uint64_t value
*/
uint64_t csprn_next_uint64_t(csprng* rng) {
	uint64_t output;
	if (rng->chars_used + 8 > csprng_gen_rate) {
		KeccakF1600_StatePermute(rng->state);
		rng->chars_used = 0;
	}
	output = ((uint64_t) rng->state[rng->chars_used] ) | (((uint64_t)rng->state[rng->chars_used+1]) << 8) 
		            | (((uint64_t)rng->state[rng->chars_used+2]) << 16) | (((uint64_t)rng->state[rng->chars_used+3]) << 24) 
					| (((uint64_t)rng->state[rng->chars_used+4]) << 32) | (((uint64_t)rng->state[rng->chars_used+5]) << 40) 
					| (((uint64_t)rng->state[rng->chars_used+6]) << 48) | (((uint64_t)rng->state[rng->chars_used+7]) << 56);
	rng->chars_used += 8;
	return output;
}

/*
	Generates a uint16_t

	rng : The pseudorandom number generator

	returns : a pseudorandomly generated uint16_t value
*/
uint16_t csprn_next_uint16_t(csprng* rng) {
	uint16_t output;
	if (rng->chars_used + 2 > csprng_gen_rate) {
		KeccakF1600_StatePermute(rng->state);
		rng->chars_used = 0;
	}
	output = ((uint16_t)rng->state[rng->chars_used]) | ((uint16_t)(rng->state[rng->chars_used+1] << 8)) ;
	rng->chars_used += 2;
	return output;
}

/*
	Initializes the rng and seeds a uint64_t

	rng : the pseudorandom number generator

	seed : the seed
*/
void csprng_seed_uint64_t(csprng* rng, uint64_t seed) {
	unsigned char s[8];
	writer W = newWriter(s);

	csprng_init(rng);
	serialize_uint64_t(&W, seed, 64);
	csprng_seed(rng, 8, s);
}
