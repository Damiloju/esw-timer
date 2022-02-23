/**
 * @brief Example usage of Timer peripheral. Play the beginning of Gaudeamus
 *        on the buzzer.
 *
 * Gaudeamus notes
 * https://www.8notes.com/scores/21488.asp
 * 
 * Andante maestoso ~ 80 BPM -> 1 beat ~ 3/4s
 * 
 * F4 3/4 beat-> 562 ms
 * C4 1/4 beat -> 188 ms
 * C4 1 beat -> 750 ms
 * F4 1 beat -> 750 ms
 * D4 3/4 beat-> 562 ms
 * D4 1/4 beat -> 188 ms
 * D4 2 beats -> 1500 ms
 * E4 3/4 beat-> 562 ms
 * F4 1/4 beat -> 188 ms
 * G4 1 beat -> 750 ms
 * E4 1 beat -> 750 ms
 * F4 1/2 beat -> 375 ms
 * A4 1/2 beat -> 375 ms
 * F4 2 beats -> 1500 ms


 * Frequencies (37500 clock)
 * https://pages.mtu.edu/~suits/notefreqs.html
 * C4 261,63 Hz -> 142 top
 * D4 293,66 Hz -> 127 top
 * E4 329,63 Hz -> 113 top
 * F4 349,23 Hz -> 106
 * G4 392 Hz -> 95
 * A4 440 Hz -> 84
 * B4 
 * C5 523,25 -> 71
 * D5 587,33 -> 63
 * E5 659,25 -> 56
 * F5 698,46 -> 53
 * G5 783,99 -> 47
 * A5 880 -> 42
 * 
 * 
 * EFR32 Application Note on Timers
 * https://www.silabs.com/documents/public/application-notes/AN0014.pdf
 *
 * EFR32MG12 Wireless Gecko Reference Manual (Timer p672)
 * https://www.silabs.com/documents/public/reference-manuals/efr32xg12-rm.pdf
 *
 * Timer API documentation 
 * https://docs.silabs.com/mcu/latest/efr32mg12/group-TIMER
 * 
 * ARM RTOS API
 * https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS.html
 * 
 * Copyright Thinnect Inc. 2019
 * Copyright ProLab TTÜ 2022
 * @license MIT
 * @author Johannes Ehala
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "retargetserial.h"
#include "cmsis_os2.h"
#include "platform.h"

#include "SignatureArea.h"
#include "DeviceSignature.h"

#include "loggers_ext.h"
#include "logger_fwrite.h"

#include "timer_handler.h"

#include "loglevels.h"
#define __MODUUL__ "main"
#define __LOG_LEVEL__ (LOG_LEVEL_main & BASE_LOG_LEVEL)
#include "log.h"

// Include the information header binary
#include "incbin.h"
INCBIN(Header, "header.bin");

static uint32_t timer_freq;
static void buz_control_loop (void *args);

// Heartbeat thread, initialize Timer and print heartbeat messages.
void hp_loop ()
{
    #define ESWGPIO_HB_DELAY 10 // Heartbeat message delay, seconds
    
    // Initialize GPIO and Timer
    buzzer_gpio_init();
    timer_freq = timer0_init(); // No mutex, because only written once and buzzer thread hasn't been created yet
    
    info1("Timer frequency %lu Hz", timer_freq);
    
    // Create a thread for buzzer control.
    const osThreadAttr_t buz_thread_attr = { .name = "buz_onoff" };
    osThreadNew(buz_control_loop, NULL, &buz_thread_attr);    
    
    for (;;)
    {
        osDelay(ESWGPIO_HB_DELAY*osKernelGetTickFreq());
        info1("Heartbeat");
    }
}

static void buz_control_loop (void *args)
{
    #define BUZZER_NOTE_C5  71 // TOP value at 37500 Hz
    #define BUZZER_NOTE_D5  63 // TOP value at 37500 Hz
    #define BUZZER_NOTE_E5  56 // TOP value at 37500 Hz
    #define BUZZER_NOTE_F5  53 // TOP value at 37500 Hz
    #define BUZZER_NOTE_G5  47 // TOP value at 37500 Hz
    #define BUZZER_NOTE_A5  42 // TOP value at 37500 Hz
    
    #define BUZZER_HALF_NOTE    1500 // ms (andante maestoso)
    #define BUZZER_QARTER_NOTE  750 // ms (andante maestoso)
    #define BUZZER_1_8_NOTE     375 // ms (andante maestoso)
    #define BUZZER_3_16_NOTE    562 // ms (andante maestoso)
    #define BUZZER_1_16_NOTE    188 // ms (andante maestoso)
    #define BUZZER_INTRA_NOTE   10 // ms (andante maestoso)
    
    uint8_t count = 1;
    
    timer0_set_top_val(0);
    osDelay(3000);
    
    for (;;)
    {
        timer0_set_top_val(BUZZER_NOTE_F5);
        info1("Gau");
        osDelay((uint32_t)((float)BUZZER_3_16_NOTE/1000*osKernelGetTickFreq()));
        
        timer0_set_top_val(BUZZER_NOTE_C5);
        info1("de");
        osDelay((uint32_t)((float)BUZZER_1_16_NOTE/1000*osKernelGetTickFreq()));
        
        timer0_set_top_val(0);
        osDelay(BUZZER_INTRA_NOTE);
        
        timer0_set_top_val(BUZZER_NOTE_C5);
        info1("a-");
        osDelay((uint32_t)((float)BUZZER_QARTER_NOTE/1000*osKernelGetTickFreq()));
        
        timer0_set_top_val(BUZZER_NOTE_F5);
        info1("mus");
        osDelay((uint32_t)((float)BUZZER_QARTER_NOTE/1000*osKernelGetTickFreq()));
        
        timer0_set_top_val(BUZZER_NOTE_D5);
        info1("i-");
        osDelay((uint32_t)((float)BUZZER_3_16_NOTE/1000*osKernelGetTickFreq()));
        
        timer0_set_top_val(0);
        osDelay(BUZZER_INTRA_NOTE);
        
        timer0_set_top_val(BUZZER_NOTE_D5);
        info1("g-");
        osDelay((uint32_t)((float)BUZZER_1_16_NOTE/1000*osKernelGetTickFreq()));
        
        timer0_set_top_val(0);
        osDelay(BUZZER_INTRA_NOTE);
        
        timer0_set_top_val(BUZZER_NOTE_D5);
        info1("tur");
        osDelay((uint32_t)((float)BUZZER_HALF_NOTE/1000*osKernelGetTickFreq()));
        
        timer0_set_top_val(BUZZER_NOTE_E5);
        info1("ju-");
        osDelay((uint32_t)((float)BUZZER_3_16_NOTE/1000*osKernelGetTickFreq()));
        
        timer0_set_top_val(BUZZER_NOTE_F5);
        info1("ve-");
        osDelay((uint32_t)((float)BUZZER_1_16_NOTE/1000*osKernelGetTickFreq()));
        
        timer0_set_top_val(BUZZER_NOTE_G5);
        info1("nes");
        osDelay((uint32_t)((float)BUZZER_QARTER_NOTE/1000*osKernelGetTickFreq()));
        
        timer0_set_top_val(BUZZER_NOTE_E5);
        info1("dum");
        osDelay((uint32_t)((float)BUZZER_QARTER_NOTE/1000*osKernelGetTickFreq()));
        
        timer0_set_top_val(BUZZER_NOTE_F5);
        info1("su-");
        osDelay((uint32_t)((float)BUZZER_1_8_NOTE/1000*osKernelGetTickFreq()));
        
        timer0_set_top_val(BUZZER_NOTE_A5);
        info1("u-");
        osDelay((uint32_t)((float)BUZZER_1_8_NOTE/1000*osKernelGetTickFreq()));
        
        timer0_set_top_val(BUZZER_NOTE_F5);
        info1("mus");
        osDelay((uint32_t)((float)BUZZER_HALF_NOTE/1000*osKernelGetTickFreq()));

        timer0_set_top_val(0);
        osDelay(BUZZER_INTRA_NOTE);

        if(count == 2)
        {
            timer0_set_top_val(0);
            count = 1;
            osDelay(1500);
        }
        else count++;
    }
}

int logger_fwrite_boot (const char *ptr, int len)
{
    fwrite(ptr, len, 1, stdout);
    fflush(stdout);
    return len;
}

int main ()
{
    PLATFORM_Init();

    // Configure log message output
    RETARGET_SerialInit();
    log_init(BASE_LOG_LEVEL, &logger_fwrite_boot, NULL);

    info1("ESW-Timer "VERSION_STR" (%d.%d.%d)", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    // Initialize OS kernel.
    osKernelInitialize();

    // Create a thread.
    const osThreadAttr_t hp_thread_attr = { .name = "hp" };
    osThreadNew(hp_loop, NULL, &hp_thread_attr);

    if (osKernelReady == osKernelGetState())
    {
        // Switch to a thread-safe logger
        logger_fwrite_init();
        log_init(BASE_LOG_LEVEL, &logger_fwrite, NULL);

        // Start the kernel
        osKernelStart();
    }
    else
    {
        err1("!osKernelReady");
    }

    for(;;);
}
