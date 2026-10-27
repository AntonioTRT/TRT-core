#include "stm32l0xx_hal.h"
#include <string.h>

#define LED_PIN GPIO_PIN_5
#define LED_GPIO_PORT GPIOA

static UART_HandleTypeDef huart2;

static void SystemClock_Config(void);
static void GPIO_Init(void);
static void USART2_Init(void);
static void UART_Send(const char *message);
static void Error_Handler(void);

int main(void)
{
    uint32_t last_blink = 0;
    uint32_t last_hello = 0;
    uint8_t command;
    uint8_t manual_led_control = 0;

    HAL_Init();
    SystemClock_Config();
    GPIO_Init();
    USART2_Init();

    UART_Send("Hello World from TRT-STM32\r\n");
    last_hello = HAL_GetTick();

    while (1)
    {
        uint32_t now = HAL_GetTick();

        if (!manual_led_control && (uint32_t)(now - last_blink) >= 500U)
        {
            HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
            last_blink = now;
        }

        if ((uint32_t)(now - last_hello) >= 1000U)
        {
            UART_Send("Hello World from TRT-STM32\r\n");
            last_hello = HAL_GetTick();
        }

        HAL_StatusTypeDef status = HAL_UART_Receive(&huart2, &command, 1, 0);
        if (status == HAL_OK)
        {
            if (command == '1')
            {
                manual_led_control = 1;
                HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET);
                UART_Send("ACK 1 LED ON\r\n");
            }
            else if (command == '0')
            {
                manual_led_control = 1;
                HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_RESET);
                UART_Send("ACK 0 LED OFF\r\n");
            }
            else
            {
                UART_Send("ERR use 0 or 1\r\n");
            }
        }
        else if (status != HAL_TIMEOUT)
        {
            Error_Handler();
        }
    }
}

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef oscillator = {0};
    RCC_ClkInitTypeDef clock = {0};

    oscillator.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    oscillator.HSIState = RCC_HSI_ON;
    oscillator.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    oscillator.PLL.PLLState = RCC_PLL_ON;
    oscillator.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    oscillator.PLL.PLLMUL = RCC_PLL_MUL4;
    oscillator.PLL.PLLDIV = RCC_PLL_DIV2;

    if (HAL_RCC_OscConfig(&oscillator) != HAL_OK)
    {
        Error_Handler();
    }

    clock.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                    | RCC_CLOCKTYPE_PCLK1;
    clock.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clock.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clock.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&clock, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }
}

static void GPIO_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_RESET);
    gpio.Pin = LED_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_PORT, &gpio);
}

static void USART2_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef *uart)
{
    GPIO_InitTypeDef gpio = {0};

    if (uart->Instance != USART2)
    {
        return;
    }

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = GPIO_AF4_USART2;
    HAL_GPIO_Init(GPIOA, &gpio);
}

static void UART_Send(const char *message)
{
    if (HAL_UART_Transmit(&huart2, (uint8_t *)message,
                          (uint16_t)strlen(message), 100) != HAL_OK)
    {
        Error_Handler();
    }
}

static void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
