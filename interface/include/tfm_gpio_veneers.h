#ifndef __TFM_GPIO_VENEERS_H__
#define __TFM_GPIO_VENEERS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// return 0 on success, non-zero on failure
uint8_t tfm_gpio_enable_output(uint8_t pin);
uint8_t tfm_gpio_set(uint8_t pin);
uint8_t tfm_gpio_clear(uint8_t pin);
uint8_t tfm_gpio_disable(uint8_t pin);
uint8_t tfm_gpio_interrupt_enable(uint8_t pin, void (*cb) (void));

#ifdef __cplusplus
}
#endif

#endif /* __TFM_GPIO_VENEERS_H__ */

