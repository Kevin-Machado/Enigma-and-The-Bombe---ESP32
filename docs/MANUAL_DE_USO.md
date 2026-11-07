# Manual de uso

## 1. Hardware

1. Conectar los tres LED segun `docs/CONEXIONES_LED.md`.
2. Ajustar pines en `src/config/hardware_config.h` si no se usan 25, 26 y 27.
3. Alimentar el ESP32 por USB.

## 2. Carga del programa

Con PlatformIO:

```bash
pio run -e esp32dev -t upload
```

SSID: `EnigmaBombe`  
Clave: `turing1939`

## 3. Conexion

1. En el telefono u ordenador, unirse a `EnigmaBombe`.
2. Abrir `http://192.168.4.1` (si no abre, revisar la IP en el monitor serie).
3. Elegir Enigma o The Bombe.

## 4. Enigma

1. Elegir tres rotores distintos, reflector, anillos y posiciones (tres letras).
2. Opcional: pares de plugboard (`AB CD EF`).
3. Escribir texto y pulsar Cifrar / Descifrar.
4. Para descifrar, restablecer las mismas posiciones iniciales y pegar el criptograma.

Espacios se omiten. Numeros y simbolos no se cifran.

## 5. The Bombe

1. Introducir texto cifrado, crib, alineacion (0 = comienzo) y la misma configuracion de rotores/anillos que se desea probar.
2. Pulsar Iniciar busqueda.
3. El LED amarillo se enciende. El rojo parpadea al descartar. El verde se enciende solo si Enigma confirma un crib.
4. Se puede cancelar o reiniciar en cualquier momento.

Interpretacion:

- Candidata: no hubo contradiccion logica.
- Verificada: el simulador de Enigma reprodujo el crib. Sigue sin ser una prueba de que sea la unica clave posible.
