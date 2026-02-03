#pragma once

#include "types.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

/*
 * Very Efficient XORSHIFT64
 * */

static u64 rng_state;

void rng_seed(void)
{
    int fd = open("/dev/urandom", O_RDONLY);
    read(fd, &rng_state, sizeof(rng_state));
    close(fd);
}

static inline u64 xorshift64(void)
{
    rng_state ^= rng_state >> 12;
    rng_state ^= rng_state << 25;
    rng_state ^= rng_state >> 27;
    return rng_state * 0x2545F4914F6CDD1DULL;
}

static inline f64 random_f64(void)
{
    return (xorshift64() >> 11) * (1.0 / (1ULL << 53)) - 0.5;
}
