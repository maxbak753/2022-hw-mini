/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// API documentation: https://raspberrypi.github.io/pico-sdk-doxygen/modules.html

// Fade an LED between low and high brightness.
// An interrupt handler updates the PWM slice's output level each time the counter wraps.

#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

#ifndef PICO_DEFAULT_LED_PIN
#error pwm/led_fade example requires a board with a regular LED
#endif

#define MAX_LED_BRIGHTNESS 255
#define MIN_LED_BRIGHTNESS 0


// ALTERED CODE: START ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\\\\\~
// The code has been altered so there are two slow brightness fades up & down
// followed by two fast brightness fades up & down
// ALTERED CODE: END ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/////~


void on_pwm_wrap() {
// this is the interrupt handler, called each time the PWM counter wraps
    static int fade = 0;
    static bool going_up = true;
    // ALTERED CODE: START ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\\\\\~
    // Counts how many fades & down have occurred
    static int counter = 0;
    // ALTERED CODE: END ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/////~
    // Clear the interrupt flag that brought us here
    pwm_clear_irq(pwm_gpio_to_slice_num(PICO_DEFAULT_LED_PIN));

    // ALTERED CODE: START ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\\\\\~
    // It will fade up and down twice slowly and then twice 15x faster, and repeat

    // Fade up & down SLOW
    if (counter < 4) {
        if (going_up) {
            ++fade;
            if (fade > MAX_LED_BRIGHTNESS) {
                fade = MAX_LED_BRIGHTNESS;
                going_up = false;
                ++counter;
            }
        } else {
            --fade;
            if (fade < MIN_LED_BRIGHTNESS) {
                fade = MIN_LED_BRIGHTNESS;
                going_up = true;
                ++counter;
            }
        
        }
    // Fade up & down FAST
    } else if (counter >= 4) {
        if (going_up) {
            fade = fade + 15;
            if (fade > MAX_LED_BRIGHTNESS) {
                fade = MAX_LED_BRIGHTNESS;
                going_up = false;
                ++counter;
            }
        } else {
            fade = fade - 15;
            if (fade < MIN_LED_BRIGHTNESS) {
                fade = MIN_LED_BRIGHTNESS;
                going_up = true;
                ++counter;
            }
        }
    }

    if (counter >= 8) {
        counter = 0;
    }

    // ALTERED CODE: END ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/////~

    // Square the fade value to make the LED's brightness appear more linear
    // note uint16_t range (0-65535)
    pwm_set_gpio_level(PICO_DEFAULT_LED_PIN, fade * fade);
    //https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__pwm.html#ga279d1ba7dcc8f19619f389317efb41fd
}


int main(void) {
    // Tell the LED pin that the PWM is in charge of its value.
    gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_PWM);
    // Figure out which slice we just connected to the LED pin
    uint slice_num = pwm_gpio_to_slice_num(PICO_DEFAULT_LED_PIN);

    // Mask our slice's IRQ output into the PWM block's single interrupt line
    pwm_clear_irq(slice_num);
    pwm_set_irq_enabled(slice_num, true);

    // Register the interrupt handler: here it's function on_pwm_wrap() defined above
    irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    // Get some sensible defaults for the slice configuration. By default, the
    // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    pwm_config config = pwm_get_default_config();

    // Set divider, reduces counter clock to sysclock/this value
    // ALTERED CODE: START ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\\\\\~
    // Now it is sysclock / 8.f instead of 4.f --> clock is slowed down by half
    pwm_config_set_clkdiv(&config, 8.f);
    // ALTERED CODE: END ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/////~

    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);

    // This empty loop allows the PWM interrupt handler to effectively take over program control.
    // tight_loop_contents() is an empty no-op function that's a placeholder.
    while (true)
        tight_loop_contents();
}
