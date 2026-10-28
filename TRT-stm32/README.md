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
- PA2/PA3 están configurados como USART2 AF4 con pull-up. El reloj usa HSI16
  directo a 16 MHz, sin PLL, con divisores AHB/APB en 1 y latencia
  `FLASH_LATENCY_0`; `SystemCoreClockUpdate()` sincroniza la variable global
  del CMSIS. USART2 selecciona PCLK1 explícitamente antes de inicializar la HAL
  a 115200 baudios.
- `SysTick_Handler()` llama a `HAL_IncTick()` y `HAL_SYSTICK_IRQHandler()`;
  ambas funciones de HAL que dependen del tick, como `HAL_Delay()`, requieren
  que la interrupción de SysTick no quede en el `Default_Handler` del startup.
- El polling UART limpia explícitamente ORE, NE, FE y PE antes de cada lectura,
  antes de iniciar una transmisión y desde el callback de error para mantener
  disponible el receptor tras ruido o overrun.
- El despachador procesa como máximo un comando por iteración. El bucle mantiene
  las fases cooperativas de servicios, eventos, comandos, estado y transmisión;
  no usa `HAL_Delay` ni espera mensajes del host.
- LD2 está en PA5 y DAC canal 1
  (PA4) se inicializa mediante HAL.
- LD2 conmuta cada 500 ms como heartbeat del bucle principal. B1 en PC13 está
  configurado como entrada con pull-up y se interpreta activo en nivel bajo;
  mantenerlo pulsado hace parpadear LD2 inmediatamente cada 100 ms.
- Board ID: `0x0002`; nombre: `NUCLEO-L073RZ`; versión del firmware: `0.1.0`.
  La revisión se declara como `unspecified` y el número de serie como un
  marcador determinista, no como una lectura del UID del MCU.
- Capacidades declaradas: SYSTEM, GPIO, LED y DAC. GPIO expone LD2 como pin 5
  y DAC expone el canal 1. No se anuncian comandos de control de periféricos
  porque no pertenecen al conjunto de comandos V0.1.

El firmware atiende PING (`0x0001`, respuesta ACK `0x8001`), FW_VERSION (`0x0002`), BOARD_VERSION
(`0x0003`), INFO (`0x0004`), STATUS (`0x0005`) y CAPABILITIES (`0x0006`).
Los frames usan el wire format V0.1 compartido (`trt-core` implementa
actualmente el campo CRC16 como `0x0000`, según su fase de implementación).
No se envían mensajes ASCII de bienvenida por el mismo USART.

## Compilar y cargar

Requisitos: Python 3, PlatformIO Core y el toolchain ST-LINK instalado.

Desde la raíz del repositorio:

```powershell
python -m platformio run -d TRT-core\TRT-stm32
python -m platformio run -d TRT-core\TRT-stm32 -t upload
python -m platformio device monitor -d TRT-core\TRT-stm32 -b 115200
python TRT-core\TRT-stm32\tools\ping_vcp.py --port COM4
```

El monitor abre el puerto COM virtual del ST-LINK. Para especificar uno
manualmente, añade `--port COM5` al comando de monitor.
El script `tools/ping_vcp.py` envía un único PING TRT v0.1 a 115200 8N1 y
verifica la respuesta ACK (`0x8001`) de board `0x0002`. Requiere `pyserial`
(`python -m pip install pyserial`) y puede ejecutarse desde la raíz del workspace.

Después de cambiar o compilar el firmware, vuelve a cargarlo antes de probarlo
por VCP. En PowerShell desde la raíz del workspace:

```powershell
python TRT-core\TRT-stm32\tools\ping_vcp.py --port COM4
python -m trt discover
python -m trt board info 2 --port COM4
```

El LED LD2 debe alternar cada 500 ms mientras el runtime está activo; al mantener
B1 (botón azul, PC13) pulsado debe parpadear cada 100 ms. Durante el diagnóstico,
las marcas de fase de LD2 señalan la progresión de la inicialización. La
configuración de reloj usa HSI16 directo a 16 MHz sin PLL; USART2 usa PCLK1
explícito, PA2/PA3 en AF4, pull-up y 115200 8N1. Si el ping
agota el tiempo, confirma que COM4
pertenece al ST-LINK VCP, que ningún monitor serie mantiene abierto el puerto y
que se ha cargado el binario recién compilado.
