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

}  // namespace

int main() {
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();
    USART2_Init();
    DAC_Init();

    trt::stm32::Runtime runtime;
    if (!runtime.initialize()) {
        Error_Handler();
    }

    while (1) {
        runtime.update_services();
        runtime.process_events();
        runtime.process_commands();
        runtime.update_state_machine();
        runtime.transmit_pending_messages();
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
    gpio.Pull = GPIO_NOPULL;
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

namespace {

void SystemClock_Config() {
    RCC_OscInitTypeDef oscillator = {};
    RCC_ClkInitTypeDef clock = {};

    oscillator.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    oscillator.HSIState = RCC_HSI_ON;
    oscillator.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    oscillator.PLL.PLLState = RCC_PLL_ON;
    oscillator.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    oscillator.PLL.PLLMUL = RCC_PLL_MUL4;
    oscillator.PLL.PLLDIV = RCC_PLL_DIV2;
    if (HAL_RCC_OscConfig(&oscillator) != HAL_OK) {
        Error_Handler();
    }

    clock.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    clock.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clock.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clock.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clock, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler();
    }
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
}

void USART2_Init() {
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
    __disable_irq();
    while (1) {
    }
}

}  // namespace
