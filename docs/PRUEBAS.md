# Pruebas

Las pruebas de host se ejecutan en este entorno con:

```bash
make test
```

No se ha dispuesto de un ESP32 fisico en el entorno de desarrollo. Las pruebas de GPIO, Wi-Fi y navegador quedan pendientes de la placa real. No se inventan resultados de hardware.

## Resultados reales (host, 7 noviembre 2026)

Compilador: g++ 12.2.0, `-std=c++17 -O2 -DHOST_BUILD`.

### Enigma

| Prueba | Resultado |
| --- | --- |
| Cableados I-VIII e inversos | PASS |
| Reflectores A, B, C (involucion, sin puntos fijos) | PASS |
| Plugboard: pares, rechazo de AA y letra repetida | PASS |
| Doble paso ADU -> ADV / AEW / BFX | PASS |
| Ninguna letra se cifra como si misma en AAA | PASS |
| Reciprocidad cifrado/descifrado | PASS |
| Espacios omitidos, numeros ignorados | PASS |
| Vector I-II-III B AAA/AAA `AAAAA -> BDZGO` | PASS |
| Ring setting distinto cambia el texto | PASS |

### The Bombe

| Prueba | Resultado |
| --- | --- |
| Menu rechaza auto-cifrado | PASS |
| Menu de 6 enlaces | PASS |
| Alineacion fuera de rango | PASS |
| Permutacion identidad provoca contradiccion de stecker | PASS |
| Verificador confirma crib sin alterar Enigma del usuario | PASS |
| Cancelacion y reset | PASS |
| Busqueda completa 17.576 posiciones, crib WETTER | PASS |
| Posicion AAA verificada y crib recuperado | PASS |
| LED verde logico tras verificacion | PASS |

Salida informativa de la busqueda completa (plugboard identidad conocido):

```text
INFO  crib WETTER cifrado AAA -> KLHFRT
INFO  evaluadas=17576 descartadas=17575 candidatas=1 verificadas=1
```

Ejecutado de nuevo tras el ajuste del stecker conocido: **65 pass, 0 fail**.

### API y LED logicos (host)

| Prueba | Resultado |
| --- | --- |
| `/api/info` y cifrado via JSON | PASS |
| Crib imposible rechazado | PASS |
| Amarillo, pulso rojo no bloqueante, verde, allOff | PASS |

### Hardware e integracion (ESP32)

| Prueba | Estado |
| --- | --- |
| Compilacion PlatformIO `esp32dev` | no ejecutada (PlatformIO no esta instalado en este entorno) |
| Punto de acceso Wi-Fi | pendiente de placa |
| Navegador en `/` `/enigma` `/bombe` | pendiente de placa |
| LED fisicos GPIO 25/26/27 | pendiente de placa |
| HTTP durante la busqueda | disenado por trozos; no medido en silicio |

Cuando se disponga de la placa, seguir `docs/GUIA_DEMOSTRACION.md` y anotar aqui los resultados reales.
