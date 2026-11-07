# Guia de demostracion (18 de noviembre de 2026)

Duracion sugerida: 8 a 12 minutos.

## Antes de salir

- ESP32 cargado y alimentado.
- Tres LED cableados (amarillo 25, rojo 26, verde 27).
- Telefono u ordenador con Wi-Fi, pagina abierta y brillo alto.
- Ensayar una vez el crib `WETTER` para conocer el tiempo real de busqueda.

## 1. Encender

Conectar USB. El monitor serie, si se usa, muestra:

```text
Enigma + The Bombe
AP SSID: EnigmaBombe
URL: http://192.168.4.1
```

Los LED deben estar apagados.

## 2. Conectar el publico (o el presentador)

1. Wi-Fi `EnigmaBombe`, clave `turing1939`.
2. Navegador: `http://192.168.4.1`.
3. Mostrar la portada: dos maquinas, un microcontrolador, sin Internet.

## 3. Enigma (3 minutos)

1. Abrir Enigma.
2. Rotores I, II, III, reflector B, anillos AAA, posiciones AAA, plugboard vacio.
3. Cifrar `AAAAA`. Resultado esperado: `BDZGO`.
4. Restablecer las mismas posiciones y pegar `BDZGO`. Debe volver `AAAAA`.
5. Frase para el tribunal: cifrar y descifrar son el mismo camino; sin la clave inicial el texto no se recupera.

Opcional: cifrar `WETTER` y dejar el resultado `KLHFRT` para The Bombe.

## 4. The Bombe (5 minutos)

1. Abrir The Bombe.
2. Misma configuracion I II III, B, anillos AAA, plugboard vacio.
3. Texto cifrado `KLHFRT`, crib `WETTER`, alineacion 0.
4. Iniciar. El LED amarillo se enciende.
5. Explicar mientras corre:
   - se prueban 17.576 posiciones, no todas las claves de Enigma
   - el rojo senala una contradiccion logica, no un simple recuento de letras
   - el verde solo se enciende si Enigma reproduce el crib
6. Al terminar: amarillo off, verde on, candidato `AAA` verificado, recuperado `WETTER`.

Frase de cierre: una candidata no es una clave; la verificacion con la propia Enigma es el criterio.

## 5. Si algo falla

| Sintoma | Accion |
| --- | --- |
| No aparece la red | reset del ESP32, esperar 5 s |
| La pagina no carga | IP `192.168.4.1`, desactivar datos moviles |
| LED apagados | revisar GND comun y resistencias |
| Bombe error de crib | una letra del crib coincide con el cifrado; cambiar alineacion o crib |
| Busqueda larga | no tocar; se puede cancelar y mostrar el progreso ya evaluado |

## 6. Mensaje historico (30 s)

Enigma protegia el trafico aleman. En Bletchley Park, Turing y el equipo de la Bombe explotaron cribs y contradicciones del stecker para reducir el espacio de claves. Esta maqueta muestra ese principio, no la maquina original.
