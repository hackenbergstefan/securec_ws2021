/*  -*- tab-width: 4; c-basic-offset: 4; c-file-style: "linux"; indent-tabs-mode: nil; -*-
 *
 * RSA exponentation demo with uint8 integers and variable parameters.
 */

#include "hal.h"
#include "simpleserial.h"
#include <stdint.h>

/**
 * RSA exponentation on uint8_t integers.
 */
uint8_t rsa_uint8(uint8_t *input)
{
    uint8_t message = input[0],
        exponent = input[1],
        modulus = input[2];

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
    simpleserial_addcmd('r', 3 * sizeof(uint8_t), rsa_uint8);
    while(1)
        simpleserial_get();
}
