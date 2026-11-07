# Interfaz web

El ESP32 crea un punto de acceso y un servidor HTTP en el puerto 80. El navegador solo presenta datos y envia JSON.

## Paginas

| Ruta | Contenido |
| --- | --- |
| `/` | Portada con acceso a Enigma y The Bombe |
| `/enigma` | Configuracion y cifrado |
| `/bombe` | Busqueda, progreso, candidatos y LED |
| `/style.css` | Estilos |
| `/app.js` | Comunicacion fetch |

## API

| Metodo | Ruta | Uso |
| --- | --- | --- |
| GET | `/api/info` | catalogo de rotores |
| POST | `/api/enigma` | cifrar / descifrar |
| POST | `/api/enigma/reset` | restablecer |
| GET | `/api/enigma/state` | posiciones actuales |
| POST | `/api/bombe/start` | iniciar busqueda |
| GET | `/api/bombe/status` | progreso |
| GET | `/api/bombe/candidates` | lista de candidatos |
| POST | `/api/bombe/cancel` | cancelar |
| POST | `/api/bombe/reset` | reiniciar |

Cuerpo maximo: 2048 bytes. Respuestas JSON con `ok` verdadero o falso.

The Bombe se consulta cada 400 ms desde el navegador. El servidor sigue respondiendo porque la busqueda avanza por trozos en `loop()`.
