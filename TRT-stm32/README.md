# TRT-core en NUCLEO-L073RZ

Proyecto PlatformIO para integrar el runtime compartido de `trt-core` en una
NUCLEO-L073RZ (STM32L073RZT6). El firmware compila directamente las fuentes de
`trt-core/core` y `trt-core/modules`; no mantiene copias del framework en este
proyecto.

## Integración

- `Runtime` compone `BoardContext`, `CommandRegistry`, `CommandDispatcher`,
  `ProtocolParser` y `ProtocolEncoder` de TRT-core.
- USART2 usa PA2 (TX), PA3 (RX), 115200 baudios, 8N1 y el puerto COM virtual
  del ST-LINK.
- RX se recoge por polling no bloqueante; TX utiliza una cola circular y
  `HAL_UART_Transmit_IT`.
- El despachador procesa como máximo un comando por iteración. El bucle mantiene
  las fases cooperativas de servicios, eventos, comandos, estado y transmisión;
  no usa `HAL_Delay` ni espera mensajes del host.
- El reloj del sistema se configura a 32 MHz. LD2 está en PA5 y DAC canal 1
  (PA4) se inicializa mediante HAL.
- Board ID: `0x0002`; nombre: `NUCLEO-L073RZ`; versión del firmware: `0.1.0`.
  La revisión se declara como `unspecified` y el número de serie como un
  marcador determinista, no como una lectura del UID del MCU.
- Capacidades declaradas: SYSTEM, GPIO, LED y DAC. GPIO expone LD2 como pin 5
  y DAC expone el canal 1. No se anuncian comandos de control de periféricos
  porque no pertenecen al conjunto de comandos V0.1.

El firmware atiende PING (`0x0001`), FW_VERSION (`0x0002`), BOARD_VERSION
(`0x0003`), INFO (`0x0004`), STATUS (`0x0005`) y CAPABILITIES (`0x0006`).
Los frames usan el wire format V0.1 compartido (`trt-core` implementa
actualmente el campo CRC16 como `0x0000`, según su fase de implementación).
No se envían mensajes ASCII de bienvenida por el mismo USART.

## Compilar y cargar

Requisitos: Python 3, PlatformIO Core y el toolchain ST-LINK instalado.

Desde la raíz del repositorio:

```powershell
python -m platformio run -d TRT-stm32
python -m platformio run -d TRT-stm32 -t upload
python -m platformio device monitor -d TRT-stm32 -b 115200
```

El monitor abre el puerto COM virtual del ST-LINK. Para especificar uno
manualmente, añade `--port COM5` al comando de monitor.
