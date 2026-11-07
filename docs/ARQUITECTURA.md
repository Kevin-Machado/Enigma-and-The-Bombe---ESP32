# Arquitectura

El firmware es un unico programa C++ para ESP32 (Arduino Framework). No hay logica criptografica en el navegador.

## Modulos

```text
src/
  main.cpp                 arranque, bucle no bloqueante
  config/hardware_config.h pines, Wi-Fi, limites de memoria
  enigma/                  alfabeto, rotor, plugboard, reflector, maquina
  bombe/                   menu, restricciones, verificador, motor de busqueda
  hardware/leds.cpp        GPIO no bloqueante de los tres LED
  web/                     HTTP, JSON, paginas embebidas, API
```

## Flujo

1. `setup()` inicializa GPIO, arranca el punto de acceso Wi-Fi y el servidor HTTP.
2. `loop()` atiende peticiones HTTP y avanza un trozo de The Bombe (`BOMBE_STEPS_PER_SLICE` posiciones).
3. Enigma y Bombe no comparten estado. La verificacion crea una instancia independiente de Enigma.

## Separacion de responsabilidades

| Modulo | Responsabilidad |
| --- | --- |
| `enigma_machine` | cifrado historico, avance y doble paso |
| `CryptoMenu` | pares crib/cifrado y rechazo de auto-cifrado |
| `ConstraintEngine` | stecker como involucion y contradicciones |
| `Verifier` | confirma un candidato con Enigma |
| `BombeEngine` | recorre 26^3 posiciones sin bloquear el HTTP |
| `BombeLeds` | amarillo / rojo / verde |
| `web::api*` | validacion y JSON |
| paginas HTML | presentacion y sondeo periodico |

## Memoria

Las paginas se almacenan en flash (cadenas constantes). Los buffers HTTP estan acotados (`HTTP_MAX_BODY`, `MAX_MESSAGE_LEN`). The Bombe guarda como maximo `MAX_CANDIDATES` (32) filas y prioriza las verificadas.

## Por que no Python ni servicios externos

El objetivo es una maqueta autonoma en exposicion. El ESP32 no necesita Internet. Toda la busqueda ocurre en el microcontrolador.
