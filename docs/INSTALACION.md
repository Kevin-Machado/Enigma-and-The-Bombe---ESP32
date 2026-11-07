# Instalacion, compilacion y carga

## Pruebas en PC (obligatorio antes de la exposicion)

```bash
make test
```

Debe terminar con `65 pass, 0 fail` (o el recuento actual si se anaden pruebas).

## Firmware ESP32 con PlatformIO

1. Instalar [PlatformIO](https://platformio.org/).
2. En la raiz del proyecto:

```bash
pio run -e esp32dev
pio run -e esp32dev -t upload
pio device monitor -b 115200
```

Placa por defecto: `esp32dev`. Si el modulo es otro (WROOM, Wrover, C3), cambiar `board` en `platformio.ini`.

## Arduino IDE (alternativa)

1. Instalar el nucleo ESP32 de Espressif.
2. Abrir `src/main.cpp` no basta: el proyecto esta organizado en varios `.cpp`. Preferir PlatformIO.
3. Si se insiste en Arduino IDE, copiar `src/` como sketch con pestanas y anadir `-DCORE_DEBUG_LEVEL=0`.

## Configuracion que debe completar el usuario

Archivo: `src/config/hardware_config.h`

| Macro | Valor por defecto | Que ajustar |
| --- | --- | --- |
| `BOMBE_LED_YELLOW_PIN` | 25 | GPIO real del LED amarillo |
| `BOMBE_LED_RED_PIN` | 26 | GPIO real del LED rojo |
| `BOMBE_LED_GREEN_PIN` | 27 | GPIO real del LED verde |
| `LED_ACTIVE_HIGH` | 1 | 0 si el LED es activo en bajo |
| `WIFI_AP_SSID` | EnigmaBombe | nombre de la red |
| `WIFI_AP_PASSWORD` | turing1939 | minimo 8 caracteres |
| `BOMBE_STEPS_PER_SLICE` | 16 | bajar si el HTTP se siente lento |

## Bibliotecas

No hay dependencias externas. Se usan `WiFi.h` y `WebServer.h` del nucleo Arduino-ESP32.
