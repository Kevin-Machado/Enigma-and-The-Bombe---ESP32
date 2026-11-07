# Maquina Enigma

Implementacion de Enigma I / M3 de tres rotores, Wehrmacht / Luftwaffe.

## Fuentes de los cableados

Tablas documentadas por Tony Sale (codesandciphers.org.uk) y Dirk Rijmenants. No se han inventado permutaciones.

### Rotores

| Rotor | Cableado | Muesca |
| --- | --- | --- |
| I | EKMFLGDQVZNTOWYHXUSPAIBRCJ | Q |
| II | AJDKSIRUXBLHWTMCQGZNPYFVOE | E |
| III | BDFHJLCPRTXVZNYEIWGAKMUSQO | V |
| IV | ESOVPZJAYQUIRHXLNFTGKDCMWB | J |
| V | VZBRGITYUPSDNHLXAWMJQOFECK | Z |
| VI | JPGVOUMFYQBENHZRDKASXLICTW | ZM |
| VII | NZJHGRCXMYSWBOUFAIVLPEKQDT | ZM |
| VIII | FKQHTLXOCBJSPDZRAMEWNIUYGV | ZM |

### Reflectores

| UKW | Cableado |
| --- | --- |
| A | EJMZALYXVBWFCRQUONTSPIKHGD |
| B | YRUHQSLDPXNGOKMIEBFZCWVJAT |
| C | FVPJIAOYEDRZXWGCTKUQSBNMHL |

## Recorrido de la senal

Plugboard -> rotor derecho -> medio -> izquierdo -> reflector -> izquierdo inverso -> medio inverso -> derecho inverso -> plugboard.

La transformacion de un rotor con posicion P y anillo R es:

```text
shift = (P - R) mod 26
salida = wiring[(entrada + shift) mod 26] - shift
```

La inversa usa el cableado invertido con el mismo shift.

## Avance y doble paso

Los rotores avanzan **antes** de cifrar cada letra.

- El derecho avanza siempre.
- Si el derecho esta en muesca, el medio avanza.
- Si el medio esta en muesca, avanzan el medio y el izquierdo.

Eso produce el doble paso historico: con I-II-III y posiciones ADU (A=izq, D=medio, U=der, un paso antes de la muesca V del III):

1. A D V
2. A E W  (el medio avanza porque el derecho estaba en V)
3. B F X  (doble paso: el medio estaba en E, muesca del II)

## Tratamiento de texto

- Solo se cifran A-Z (mayusculas o minusculas).
- Espacios y saltos de linea se omiten.
- Numeros y simbolos se ignoran y se cuentan.

Cifrar y descifrar usan el mismo mecanismo. Hay que restaurar las posiciones iniciales.

## Vector de referencia ejecutado

Configuracion: rotores I II III, reflector B, anillos AAA, posiciones AAA, sin plugboard.

```text
AAAAA -> BDZGO
```

El descifrado de BDZGO con la misma configuracion recupera AAAAA. Este vector se usa en las pruebas de host.
