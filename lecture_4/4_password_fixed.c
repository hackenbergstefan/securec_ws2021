/*  -*- tab-width: 4; c-basic-offset: 4; c-file-style: "linux"; indent-tabs-mode: nil; -*-
 */

#include "hal.h"
#include "simpleserial.h"
#include <stdint.h>
#include <string.h>

static const uint8_t stored_password[] = "infineon";

uint8_t check_password(uint8_t *input)
{
    trigger_high();

    uint8_t password_wrong = 0;
    for (unsigned int i = 0; i < sizeof(stored_password) - 1; i++)
    {
        uint8_t stored = stored_password[i];
        uint8_t attempt = input[i];
        password_wrong |= stored ^ attempt;
    }

    trigger_low();

    simpleserial_put('p', 1, &password_wrong);
    return 0;
}

int main(void)
{
    platform_init();
    init_uart();
    trigger_setup();

    simpleserial_init();
    simpleserial_addcmd('p', 10, check_password);
    while(1)
        simpleserial_get();
}
