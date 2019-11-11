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

typedef struct {
    uint32_t data;
    uint32_t dataout;
    uint32_t reserved0[2];
    uint32_t outenset;
    uint32_t outenclr;
    uint32_t altfuncset;
    uint32_t altfuncclr;
    uint32_t intenset;
    uint32_t intenclr;
    uint32_t inttypeset;
    uint32_t inttypeclr;
    uint32_t intpolset;
    uint32_t intpolclr;
    uint32_t intstatusintclr; 
} gpio_t;

typedef void __attribute__((cmse_nonsecure_call)) ns_interrupt_cb(void);

static gpio_t *gpio = (gpio_t *) MUSCA_GPIO_S_BASE;
static ns_interrupt_cb *callbacks[N_PINS];

ns_interrupt_cb *gpio_cb = NULL;

// All specific pin handlers fall into this one
void GPIO_Handler(uint8_t pin) {
    gpio->intstatusintclr = 1 << pin;

    LOG_MSG("GPIO_IRQHandler triggered!");
    if (callbacks[pin]) callbacks[pin]();
}

void GPIO_0_IRQHandler() { GPIO_Handler(0); }
void GPIO_1_IRQHandler() { GPIO_Handler(1); }
void GPIO_2_IRQHandler() { GPIO_Handler(2); }
void GPIO_3_IRQHandler() { GPIO_Handler(3); }
void GPIO_4_IRQHandler() { GPIO_Handler(4); }
void GPIO_5_IRQHandler() { GPIO_Handler(5); }
void GPIO_6_IRQHandler() { GPIO_Handler(6); }
void GPIO_7_IRQHandler() { GPIO_Handler(7); }
void GPIO_8_IRQHandler() { GPIO_Handler(8); }
void GPIO_9_IRQHandler() { GPIO_Handler(9); }
void GPIO_10_IRQHandler() { GPIO_Handler(10); }
void GPIO_11_IRQHandler() { GPIO_Handler(11); }
void GPIO_12_IRQHandler() { GPIO_Handler(12); }
void GPIO_13_IRQHandler() { GPIO_Handler(13); }
void GPIO_14_IRQHandler() { GPIO_Handler(14); }
void GPIO_15_IRQHandler() { GPIO_Handler(15); }

// returns 0 on success, non-zero on failure
__tfm_secure_gateway_attributes__
uint8_t tfm_gpio_enable_output(uint8_t pin) {
    if (pin >= N_PINS) {
        return pin;
    }

    gpio->outenset = 1 << pin;

    return 0;
}

__tfm_secure_gateway_attributes__
uint8_t tfm_gpio_interrupt_enable(uint8_t pin, gpio_int_config *cfg) {
    if (pin >= N_PINS) {
        return pin;
    }

    NVIC_EnableIRQ(GPIO_0_IRQn + pin);
    gpio->intenset = 1 << pin;
    gpio->inttypeset = cfg->type << pin;
    gpio->intpolset = cfg->polarity << pin;

    callbacks[pin] = NULL;
    if (cfg->cb) {
        callbacks[pin] = (ns_interrupt_cb *)cfg->cb; 
    }

    return 0;
}

__tfm_secure_gateway_attributes__
uint8_t tfm_gpio_interrupt_disable(uint8_t pin) {
    if (pin >= N_PINS) {
        return pin;
    }

    NVIC_DisableIRQ(GPIO_0_IRQn + pin);
    gpio->intenclr = 1 << pin; 

    callbacks[pin] = NULL;

    return 0;
}

__tfm_secure_gateway_attributes__
uint8_t tfm_gpio_set(uint8_t pin) {
    if (pin >= N_PINS) {
        return pin;
    }

    gpio->dataout &= ~(1 << pin);
    return 0;
}

__tfm_secure_gateway_attributes__
uint8_t tfm_gpio_clear(uint8_t pin) {
    if (pin >= N_PINS) {
        return pin;
    }

    gpio->dataout |= 1 << pin;
    return 0;
}

__tfm_secure_gateway_attributes__
uint8_t tfm_gpio_disable(uint8_t pin) {
    if (pin >= N_PINS) {
        return pin;
    }

    gpio->outenclr = 1 << pin;
    return 0;
}

