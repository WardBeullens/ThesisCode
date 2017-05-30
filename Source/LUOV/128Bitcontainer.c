#include "128Bitcontainer.h"

/*
	Write a bitcontainer to a char array
*/
void serialize_bitcontainer128(writer * W, bitcontainer128 b, int bits) {
	serialize_uint64_t(W, b.part1, 64);
	serialize_uint64_t(W, b.part2, bits - 64);
}

/*
	Read a bitcontainer from a char array
*/
bitcontainer128 deserialize_bitcontainer128(reader * R, int bits) {
	bitcontainer128 out;
	out.part1 = deserialize_uint64_t(R, 64);
	out.part2 = deserialize_uint64_t(R, bits - 64);
	return out;
}

/*
	xor two bitcontainers
*/
bitcontainer128 xor128(bitcontainer128 a, bitcontainer128 b) {
	bitcontainer128 BC;
	BC.part1 = a.part1 ^ b.part1;
	BC.part2 = a.part2 ^b.part2;
	return BC;
}

/*
	Randomize a bitcontainer with Keccak
*/
bitcontainer128 randomBitcontainer128Keccak(csprng* rng) {
	bitcontainer128 BC;
	BC.part1 = csprn_next_uint64_t(rng);
	BC.part2 = csprn_next_uint64_t(rng);
	return BC;
}

/*
	Randomize bitcontainer with Mersenne twister
*/
bitcontainer128 randomBitcontainer128MT(twister* MT) {
	bitcontainer128 BC;
	BC.part1 = ((uint64_t)randomMT(MT)) | (((uint64_t)randomMT(MT)) << 32);
	BC.part2 = ((uint64_t)randomMT(MT)) | (((uint64_t)randomMT(MT)) << 32);
	return BC;
}

/*
	Get a bit from the bitcontainer
*/
uint64_t getBit128(bitcontainer128 container, uint64_t bit) {
	if (bit<64)
		return (container.part1 & ((uint64_t)1) << bit);
	return (container.part2 & ((uint64_t)1) << (bit - 64));
}