# STM32 NUCLEO-L073RZ Hello World

Proyecto PlatformIO para la placa **NUCLEO-L073RZ** (STM32L073RZT6), usando
STM32Cube HAL y el programador ST-LINK integrado.

## Funcionalidad

- Hace parpadear el LED de usuario LD2 (PA5) cada 500 ms.
- Configura USART2 en PA2 (TX) y PA3 (RX), a 115200 baudios, 8N1.
- Envía `Hello World from TRT-STM32` al arrancar y cada segundo.
- El comando `1` enciende LD2 y responde `ACK 1 LED ON`.
- El comando `0` apaga LD2 y responde `ACK 0 LED OFF`.
- Otros bytes reciben `ERR use 0 or 1`.

Todos los mensajes serie terminan en CRLF. El LED continúa parpadeando como
indicación de estado normal hasta recibir `0` o `1`. Después de recibir un
comando válido, el LED queda bajo control manual y conserva el estado indicado
hasta el siguiente comando o un reinicio.

## Requisitos

- Python 3
- PlatformIO Core
- Cable USB conectado al puerto ST-LINK de la NUCLEO-L073RZ
- Controlador ST-LINK instalado en Windows

Instala PlatformIO Core si aún no está instalado:

```powershell
python -m pip install platformio
```

Desde la raíz del repositorio, compila:

```powershell
python -m platformio run -d TRT-stm32
```

Compila y flashea por ST-LINK:

```powershell
python -m platformio run -d TRT-stm32 -t upload
```

Abre el monitor serie a 115200 baudios:

```powershell
python -m platformio device monitor -d TRT-stm32 -b 115200
```

El monitor serie usa el puerto COM virtual de ST-LINK; si PlatformIO no lo
detecta automáticamente, lista los puertos con:

```powershell
python -m platformio device list
```

Luego indica el puerto detectado con `--port`, por ejemplo:

```powershell
python -m platformio device monitor -d TRT-stm32 -b 115200 --port COM5
```

Escribe `1` o `0` en el monitor para controlar el LED. El mensaje de inicio
también se envía en cuanto arranca el firmware, por lo que puede ser necesario
abrir el monitor antes de resetear la placa para verlo.

## Integración con `trt-cli`

Esta carpeta contiene un proyecto PlatformIO convencional con un entorno
`nucleo_l073rz`. Un `trt-cli` que admita proyectos PlatformIO puede compilar y
subirlo usando las operaciones equivalentes a `platformio run` y
`platformio run -t upload`. Este repositorio no incluye la herramienta
`trt-cli` ni define su sintaxis; consulta su documentación para conocer el
comando exacto con el que selecciona este proyecto y el entorno.
