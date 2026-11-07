# The Bombe (version educativa)

No es una reproduccion de la Bombe electromecanica de 1940. Es un buscador didactico basado en:

- crib (fragmento de claro conocido)
- menu de pares claro/cifrado
- stecker como involucion
- la propiedad de que Enigma no cifra una letra como si misma
- verificacion con el simulador de Enigma

## Espacio de busqueda

Con orden de rotores, reflector, anillos y plugboard fijos se recorren:

```text
26 x 26 x 26 = 17.576
```

posiciones iniciales. Eso no es el espacio completo de claves de Enigma.

## Algoritmo

1. Construir el menu: cada letra del crib se empareja con la letra cifrada en la misma posicion (mas la alineacion).
2. Si alguna letra coincidiera consigo misma, la busqueda se rechaza (Enigma nunca lo hace).
3. Para cada terna (L, M, R):
   - avanzar el nucleo (rotores + reflector, sin plugboard) por cada offset del crib
   - obtener la permutacion del nucleo en ese paso
   - propagar restricciones del stecker
   - si las 26 hipotesis del registro de prueba fallan, se descarta (LED rojo)
   - si no hay contradiccion, la terna es **candidata**
4. Cada candidata se verifica con una instancia nueva de Enigma. Solo si el crib reaparece se marca como verificada (LED verde).

Una candidata sin contradicciones **no** es una clave confirmada.

## LED

- Amarillo: busqueda en curso
- Rojo: pulso breve al descartar por contradiccion
- Verde: al menos un candidato verificado con Enigma

## Ejemplo de demostracion

Claro `WETTER`, configuracion I II III, B, anillos AAA, posiciones AAA, sin plugboard, produce el cifrado `KLHFRT`.

The Bombe, con crib `WETTER` y texto `KLHFRT`, recorre 17.576 posiciones y verifica AAA.

Resultado real de `make test` (plugboard identidad conocido):

```text
evaluadas=17576 descartadas=17575 candidatas=1 verificadas=1
```

Con el stecker tratado como parametro constante (identidad si el campo va vacio), el filtro logico deja una sola candidata y la verificacion con Enigma la confirma: `AAA` recupera `WETTER`.
