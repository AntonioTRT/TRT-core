#include "stm32l0xx_hal.h"

#include "stm32_runtime.h"

UART_HandleTypeDef huart2;
DAC_HandleTypeDef hdac;

namespace {

void SystemClock_Config();
void GPIO_Init();
void USART2_Init();
void DAC_Init();
void Error_Handler();
void diagnostic_mark(uint8_t stage);

}  // namespace

int main() {
    HAL_Init();
    GPIO_Init();
    diagnostic_mark(1);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_Delay(1000);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

    SystemClock_Config();
    diagnostic_mark(2);
    USART2_Init();
    diagnostic_mark(3);
    DAC_Init();

    trt::stm32::Runtime runtime;
    if (!runtime.initialize()) {
        Error_Handler();
    }
    diagnostic_mark(4);

    uint32_t last_heartbeat = HAL_GetTick();
    uint32_t last_button_toggle = last_heartbeat;
    bool button_diagnostic_active = false;
    while (1) {
        runtime.update_services();
        runtime.process_events();
        runtime.process_commands();
        runtime.update_state_machine();
        runtime.transmit_pending_messages();

        const uint32_t now = HAL_GetTick();
        const bool button_pressed = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET;
        if (button_pressed) {
            if (!button_diagnostic_active) {
                button_diagnostic_active = true;
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
                last_button_toggle = now;
            } else if (static_cast<uint32_t>(now - last_button_toggle) >= 100U) {
                HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
                last_button_toggle = now;
            }
        } else {
            if (button_diagnostic_active) {
                button_diagnostic_active = false;
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
                last_heartbeat = now;
            } else if (static_cast<uint32_t>(now - last_heartbeat) >= 500U) {
                HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
                last_heartbeat = now;
            }
        }
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef* uart) {
    if (uart->Instance != USART2) {
        return;
    }

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {};
    gpio.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = GPIO_AF4_USART2;
    HAL_GPIO_Init(GPIOA, &gpio);
    HAL_NVIC_SetPriority(USART2_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void HAL_DAC_MspInit(DAC_HandleTypeDef* dac) {
    if (dac->Instance != DAC) {
        return;
    }

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DAC_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {};
    gpio.Pin = GPIO_PIN_4;
    gpio.Mode = GPIO_MODE_ANALOG;
    gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio);
}

extern "C" void USART2_IRQHandler() {
    HAL_UART_IRQHandler(&huart2);
}

extern "C" void SysTick_Handler(void) {
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}

extern "C" void HardFault_Handler(void) {
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER = (GPIOA->MODER & ~(3UL << (5U * 2U))) | (1UL << (5U * 2U));
    GPIOA->OTYPER &= ~(1UL << 5U);
    GPIOA->BSRR = static_cast<uint32_t>(GPIO_PIN_5) << 16U;

    for (;;) {
        GPIOA->BSRR = GPIO_PIN_5;
        for (volatile uint32_t delay = 0; delay < 100000U; ++delay) {
            __NOP();
        }

        GPIOA->BSRR = static_cast<uint32_t>(GPIO_PIN_5) << 16U;
        for (volatile uint32_t delay = 0; delay < 100000U; ++delay) {
            __NOP();
        }
    }
}

namespace {

void diagnostic_mark(uint8_t stage) {
    for (uint8_t pulse = 0; pulse < stage; ++pulse) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        for (volatile uint32_t delay = 0; delay < 80000U; ++delay) {
            __NOP();
        }
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        for (volatile uint32_t delay = 0; delay < 80000U; ++delay) {
            __NOP();
        }
    }

    for (volatile uint32_t delay = 0; delay < 300000U; ++delay) {
        __NOP();
    }
}

void SystemClock_Config() {
    RCC_OscInitTypeDef oscillator = {};
    RCC_ClkInitTypeDef clock = {};

    oscillator.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    oscillator.HSIState = RCC_HSI_ON;
    oscillator.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    oscillator.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&oscillator) != HAL_OK) {
        Error_Handler();
    }

    clock.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    clock.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    clock.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clock.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clock, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
    SystemCoreClockUpdate();
}

void GPIO_Init() {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

    GPIO_InitTypeDef gpio = {};
    gpio.Pin = GPIO_PIN_5;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio);

    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef button = {};
    button.Pin = GPIO_PIN_13;
    button.Mode = GPIO_MODE_INPUT;
    button.Pull = GPIO_PULLUP;
    button.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &button);
}

void USART2_Init() {
    RCC_PeriphCLKInitTypeDef peripheral_clock = {};
    peripheral_clock.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    peripheral_clock.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&peripheral_clock) != HAL_OK) {
        Error_Handler();
    }

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
}

void DAC_Init() {
    hdac.Instance = DAC;
    if (HAL_DAC_Init(&hdac) != HAL_OK) {
        Error_Handler();
    }

    DAC_ChannelConfTypeDef channel = {};
    channel.DAC_Trigger = DAC_TRIGGER_NONE;
    channel.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    if (HAL_DAC_ConfigChannel(&hdac, &channel, DAC_CHANNEL_1) != HAL_OK ||
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0) != HAL_OK ||
        HAL_DAC_Start(&hdac, DAC_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
}

void Error_Handler() {
    while (1) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(50);
    }
}

}  // namespace
