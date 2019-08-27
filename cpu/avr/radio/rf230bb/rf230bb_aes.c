#include <string.h>
#include "contiki.h"
#include "aes-128.h"
#include "radio.h"
#include "rf230bb.h"

static uint8_t static_key [16];

static void set_key (const uint8_t *key)
{
    memcpy (static_key, key, 16);
}

static void encrypt (uint8_t *state)
{
    rf230_aes_encrypt_ecb(static_key, state, state);
}

const struct aes_128_driver rf230bb_aes_128_driver = {
    set_key,
    encrypt
};
