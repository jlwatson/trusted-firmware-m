/*
 * Super simple GPIO veneers
 */

#include "tfm_gpio_veneers.h"
#include "tfm_secure_api.h"
#include "tfm_api.h"
#include "spm_partition_defs.h"
#include "platform_base_address.h"
#include "uart_stdout.h"

#define N_PINS 16

typedef void __attribute__((cmse_nonsecure_call)) ns_interrupt_cb(void);
ns_interrupt_cb *gpio_cb = NULL;

void GPIO_10_IRQHandler() {
    int32_t *statusclr = (int32_t *) (MUSCA_GPIO_S_BASE + 0x38);
    *statusclr = 1 << 10;

    LOG_MSG("GPIO_10_IRQHandler triggered!");
    if (gpio_cb) gpio_cb();
}

// returns 0 on success, non-zero on failure
__tfm_secure_gateway_attributes__
uint8_t tfm_gpio_enable_output(uint8_t pin) {
    if (pin >= N_PINS) {
        return pin;
    }

    int32_t *gpio_base = (int32_t *) MUSCA_GPIO_S_BASE;
    *(gpio_base + 4) |= (0x1 << pin);

    return 0;
}

__tfm_secure_gateway_attributes__
uint8_t tfm_gpio_interrupt_enable(uint8_t pin, void (*cb) (void)) {
    if (pin != 10) {
        pin = 10;
    }

    NVIC_EnableIRQ(GPIO_10_IRQn);
    int32_t *intset = (int32_t *) (MUSCA_GPIO_S_BASE + 0x20);
    *intset = (1 << pin);

    int32_t *typeset = (int32_t *) (MUSCA_GPIO_S_BASE + 0x28);
    *typeset = (1 << pin);

    gpio_cb = (ns_interrupt_cb *)cb; 
    LOG_MSG("interrupt enabled");
}

__tfm_secure_gateway_attributes__
uint8_t tfm_gpio_set(uint8_t pin) {
    if (pin >= N_PINS) {
        return pin;
    }

    int32_t *gpio_base = (int32_t *) MUSCA_GPIO_S_BASE;
    *(gpio_base + 1) &= ~(0x1 << pin);

    return 0;
}

__tfm_secure_gateway_attributes__
uint8_t tfm_gpio_clear(uint8_t pin) {
    if (pin >= N_PINS) {
        return pin;
    }

    int32_t *gpio_base = (int32_t *) MUSCA_GPIO_S_BASE;
    *(gpio_base + 1) |= (0x1 << pin);

    return 0;
}

__tfm_secure_gateway_attributes__
uint8_t tfm_gpio_disable(uint8_t pin) {
    if (pin >= N_PINS) {
        return pin;
    }

    int32_t *gpio_base = (int32_t *) MUSCA_GPIO_S_BASE;
    *(gpio_base + 5) |= (0x1 << pin);

    return 0;
}

