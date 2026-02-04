#pragma once
#include "types.h"

/*
 * Very Efficient XORSHIFT64 RNG
 */

void rng_seed(void);
u64 xorshift64(void);
f64 random_f64(void);
