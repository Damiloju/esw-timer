/**
 * @file timer_handler.c
 *
 * @brief Init TIMER0 to control buzzer with PWM signal.
 *
 * @author Johannes Ehala, ProLab.
 * @license MIT
 *
 * Copyright ProLab, TTÜ. 15 January 2022
 */

#include "em_cmu.h"
#include "em_timer.h"

#include "timer_handler.h"

void buzzer_gpio_init()
{
    // Enable clock for GPIO
    CMU_ClockEnable(cmuClock_GPIO, true);

    // Configure GPIO pin
    GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);
}

/**
 * @brief Init TIMER0 to regulate PWM dutycycle.
 */
uint32_t timer0_init(void)
{
    // Enable clocks.
    CMU_ClockEnable(cmuClock_TIMER0, true);

    // Init compare mode initialization and configure
    TIMER_InitCC_TypeDef ccInit = TIMER_INITCC_DEFAULT;
    ccInit.mode = timerCCModeCompare;
    ccInit.cofoa = timerOutputActionToggle;

    // TODO Init
    TIMER_InitCC(TIMER0, 0, &ccInit);

    TIMER_TopSet(TIMER0, 100);

    TIMER0->ROUTEPEN = 1; // TIMER_ROUTE_CC0PEN;
    TIMER0->ROUTELOC0 = TIMER_ROUTELOC0_CC0LOC_LOC0;

    // Timer general initialization and config.
    TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
    timerInit.prescale = timerPrescale1024;
    timerInit.enable = true;

    TIMER_Init(TIMER0, &timerInit);

    return (uint32_t)(CMU_ClockFreqGet(cmuClock_TIMER0) / (uint32_t)(0x1 << timerInit.prescale));
}
