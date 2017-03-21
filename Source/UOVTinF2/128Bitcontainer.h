/*
	Nothing fancy here, just an implementation of a container that contains up to 128 bits , implemented by 2 uint64_t values
*/

#ifndef BITCONTAINER128_H
#define BITCONTAINER128_H

#include <stdint.h>
#include "buffer.h"
#include "csprng.h"
#include "twister.h"

typedef struct{
	uint64_t part1, part2;
} bitcontainer128;

static const bitcontainer128 empty128 = {0,0};

void serialize_bitcontainer128(writer * Buff, bitcontainer128 b, int bits);
bitcontainer128 deserialize_bitcontainer128(reader * Buff, int bits);
bitcontainer128 xor128(bitcontainer128 a, bitcontainer128 b);
bitcontainer128 randomBitcontainer128Keccak(csprng* rng);
bitcontainer128 randomBitcontainer128MT(twister* MT);
uint64_t getBit128(bitcontainer128 container, uint64_t bit);

#endif
