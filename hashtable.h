/*
 * corelib.h - Core C Standard Library Implementation
 *
 * Copyright (c) 2025 mladicstefan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    void* key;
    void* value;
    uint32_t hash;
    bool is_occupied;
} entry_t;

typedef struct {
    entry_t* entries;
    size_t table_size;
    size_t count;
} hashtable_t;

static inline uint32_t djb2_hash(void * key, size_t len);
static inline uint32_t djb2_hash(void * key, size_t len){
    uint32_t hash = 5381; //base for DJB2 hash
    for (size_t i =0; i < len; i++){
        hash = ((hash << 5) + hash) + ((uint8_t*)key)[i]; //multiply by 2^5th by shifting
    }
    return hash;
}

static inline size_t hash_to_index(void* key, size_t key_len,size_t table_size);
static inline size_t hash_to_index(void* key, size_t key_len,size_t table_size){
    return djb2_hash(key,key_len) % table_size;
}

