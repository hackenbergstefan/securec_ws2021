/*  -*- tab-width: 4; c-basic-offset: 4; c-file-style: "linux"; indent-tabs-mode: nil; -*-
 *
 * RSA exponentation demo with uint8 integers giving parameters:
 *   p = 13, q = 17, n = 221
 *   phi(n) = 12 * 16 = 192
 *   private_exponent = 5
 *   public_exponent = 77
 */

#include "hal.h"
#include "simpleserial.h"
#include <stdint.h>

const uint8_t modulus = 221;
const uint8_t private_exponent = 5;


/**
 * RSA exponentation on uint8_t integers.
 */
uint8_t rsa_uint8_fixed(uint8_t *input)
{
    uint8_t exponent = private_exponent;
    uint8_t message = 0xA0;

    trigger_high();

    uint16_t tmp;
    uint8_t result = 1;
    while (exponent)
    {
        if (exponent & 1)
        {
            tmp = result * message;
            result = tmp % modulus;
        }

        tmp = message * message;
        message = tmp % modulus;
        exponent >>= 1;
    }

    trigger_low();

    simpleserial_put('r', sizeof(uint8_t), &result);
    return 0;
}

int main(void)
{
    platform_init();
    init_uart();
    trigger_setup();

    simpleserial_init();
    simpleserial_addcmd('r', 0, rsa_uint8_fixed);
    while(1)
        simpleserial_get();
}
