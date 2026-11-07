# Conexion de los tres LED

Los LED pertenecen solo a The Bombe. Enigma no usa LED.

## Pines por defecto (ESP32 DevKit)

| Funcion | Color | GPIO | Definicion |
| --- | --- | --- | --- |
| Busqueda en curso | Amarillo | 25 | `BOMBE_LED_YELLOW_PIN` |
| Contradiccion | Rojo | 26 | `BOMBE_LED_RED_PIN` |
| Candidato verificado | Verde | 27 | `BOMBE_LED_GREEN_PIN` |

Editar en `src/config/hardware_config.h` si el hardware usa otros pines.

## Cableado (anodo al pin)

```text
GPIO 25 ----[ 220 ohm ]----|>|---- GND     amarillo
GPIO 26 ----[ 220 ohm ]----|>|---- GND     rojo
GPIO 27 ----[ 220 ohm ]----|>|---- GND     verde
```

- Polaridad activa en ALTO (`LED_ACTIVE_HIGH 1`).
- Resistencia 220 ohm o 330 ohm.
- Catodo comun a GND del ESP32.
- No conectar LED sin resistencia.

## Comportamiento

| Momento | Amarillo | Rojo | Verde |
| --- | --- | --- | --- |
| Reposo | off | off | off |
| Busqueda | on | pulso 80 ms al descartar | off hasta verificar |
| Fin con verificados | off | off | on |
| Fin sin verificados | off | off | off |
| Cancelar | off | off | se mantiene si ya hubo verificados |

El pulso rojo es no bloqueante: se apaga en `leds.tick()` comparando `millis()`.
