#include "app_selector.h"

#ifdef PROGRAM2_ENABLED

#include <stdint.h>
#include "program2.h"

// RCC
#define RCC_IOPENR    (*(volatile uint32_t *)(0x4002102C))
#define RCC_APB1ENR   (*(volatile uint32_t *)(0x40021038))

// GPIOA
#define GPIOA_MODER   (*(volatile uint32_t *)(0x50000000))
#define GPIOA_ODR     (*(volatile uint32_t *)(0x50000014))

// GPIOC
#define GPIOC_MODER   (*(volatile uint32_t *)(0x50000800))
#define GPIOC_IDR     (*(volatile uint32_t *)(0x50000810))

// DAC
#define DAC_CR        (*(volatile uint32_t *)(0x40007400))
#define DAC_DHR12R1   (*(volatile uint32_t *)(0x40007408))

static uint8_t btn_last = 0;

static const uint16_t levels[] =
{
    0,      // 0%
    1024,   // 25%
    2048,   // 50%
    3072,   // 75%
    4095,   // 100%
    3072,   // 75%
    2048,   // 50%
    1024    // 25%
};

static uint8_t level_index = 0;

static void delay_us(volatile uint32_t us)
{
    us *= 2;

    while (us--)
    {
        __asm("nop");
    }
}

void app_init(void)
{
    // Enable GPIOA and GPIOC clocks
    RCC_IOPENR |= (1U << 0);
    RCC_IOPENR |= (1U << 2);

    // Enable DAC clock
    RCC_APB1ENR |= (1U << 29);

    // PA4 = Analog mode
    GPIOA_MODER |= (3U << (4 * 2));

    // PA5 = LED output
    GPIOA_MODER &= ~(3U << (5 * 2));
    GPIOA_MODER |=  (1U << (5 * 2));

    // PC13 = Button input
    GPIOC_MODER &= ~(3U << (13 * 2));

    // Enable DAC channel 1
    DAC_CR |= (1U << 0);

    // Initial level
    DAC_DHR12R1 = levels[level_index];
}

void app_loop(void)
{
    uint8_t btn_pressed =
        !(GPIOC_IDR & (1U << 13));

    // Button edge detect + debounce
    if (btn_pressed && !btn_last)
    {
        delay_us(20000);

        if (!(GPIOC_IDR & (1U << 13)))
        {
            level_index++;

            if (level_index >= (sizeof(levels) / sizeof(levels[0])))
            {
                level_index = 0;
            }

            DAC_DHR12R1 = levels[level_index];

            // Toggle green LED to indicate button press
            GPIOA_ODR ^= (1U << 5);
        }
    }

    btn_last = btn_pressed;
}

#endif