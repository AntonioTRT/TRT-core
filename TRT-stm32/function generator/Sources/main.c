#include <stdint.h>

// Registros RCC
#define RCC_IOPENR    (*(volatile uint32_t *)(0x4002102C))
#define RCC_APB1ENR   (*(volatile uint32_t *)(0x40021038))

// Registros GPIO
#define GPIOA_MODER   (*(volatile uint32_t *)(0x50000000))
#define GPIOA_ODR     (*(volatile uint32_t *)(0x50000014))
#define GPIOC_MODER   (*(volatile uint32_t *)(0x50000800))
#define GPIOC_IDR     (*(volatile uint32_t *)(0x50000810))

// Registros DAC
#define DAC_CR        (*(volatile uint32_t *)(0x40007400))
#define DAC_DHR12R1   (*(volatile uint32_t *)(0x40007408))

void delay_us(volatile uint32_t us) {
    us *= 2;
    while (us--) {
        __asm("nop");
    }
}

int main(void) {
    // 1. Activar reloj para GPIOA, GPIOC y DAC
    RCC_IOPENR  |= (1U << 0) | (1U << 2);
    RCC_APB1ENR |= (1U << 29);

    // 2. Configurar PA4 (Pin A2 de Arduino) en modo Analógico
    GPIOA_MODER |= (3U << (4 * 2));

    // 3. Configurar PA5 (LED LD2) en modo Salida
    GPIOA_MODER &= ~(3U << (5 * 2));
    GPIOA_MODER |=  (1U << (5 * 2));

    // 4. Configurar PC13 (Botón azul) en modo Entrada
    GPIOC_MODER &= ~(3U << (13 * 2));

    // 5. Habilitar Canal 1 del DAC
    DAC_CR |= (1U << 0);

    uint8_t mode = 0;          // 0: Pulso | 1: Diente de Sierra | 2: Triangular
    uint8_t btn_last = 0;

    uint16_t saw_val = 0;
    int16_t tri_val = 0;
    uint8_t tri_dir = 0;       // 0: Rampa ascendente | 1: Rampa descendente
    uint32_t step_count = 0;

    while (1) {
        // Leer el estado del Botón Azul (PC13 activo en LOW)
        uint8_t btn_pressed = !(GPIOC_IDR & (1U << 13));

        // Detección de flanco con filtro antirrebote (20 ms)
        if (btn_pressed && !btn_last) {
            delay_us(20000);
            if (!(GPIOC_IDR & (1U << 13))) {
                mode = (mode + 1) % 3;  // Alterna cíclicamente entre los 3 modos (0, 1, 2)
                GPIOA_ODR ^= (1U << 5); // Conmuta el LED verde para confirmar la pulsación

                // Reiniciar contadores para una transición limpia entre señales
                saw_val = 0;
                tri_val = 0;
                tri_dir = 0;
                step_count = 0;
            }
        }
        btn_last = btn_pressed;

        // Generador analógico en el pin PA4 (A2)
        switch (mode) {
            case 0: // Onda de Pulso / Cuadrada
                step_count++;
                if (step_count < 10) {
                    DAC_DHR12R1 = 4095; // Alto (3.3V)
                } else if (step_count < 20) {
                    DAC_DHR12R1 = 0;    // Bajo (0V)
                } else {
                    step_count = 0;
                }
                delay_us(100);
                break;

            case 1: // Onda Diente de Sierra
                saw_val += 128;
                if (saw_val > 4095) saw_val = 0;
                DAC_DHR12R1 = saw_val;
                delay_us(20);
                break;

            case 2: // Onda Triangular
                if (tri_dir == 0) { // Subiendo
                    tri_val += 128;
                    if (tri_val >= 4095) {
                        tri_val = 4095;
                        tri_dir = 1;
                    }
                } else { // Bajando
                    if (tri_val <= 128) {
                        tri_val = 0;
                        tri_dir = 0;
                    } else {
                        tri_val -= 128;
                    }
                }
                DAC_DHR12R1 = (uint16_t)tri_val;
                delay_us(20);
                break;
        }
    }
}
