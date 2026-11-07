# Limitaciones conocidas

1. The Bombe de este proyecto es educativa. No reproduce tambores, menus electricos ni la velocidad de la maquina de 1940.
2. La busqueda recorre 26^3 posiciones iniciales con orden de rotores, anillos, reflector y plugboard fijos. El espacio historico completo incluye Walzenlage, Ringstellung y Steckerbrett.
3. Un candidato sin contradicciones no es una clave confirmada. Solo la verificacion con Enigma comprueba el crib, y aun asi pueden existir colisiones en cribs cortos.
4. Con un crib corto y sin stecker conocido, el filtro logico deja muchas candidatas. La demostracion recomendada usa plugboard identidad o un crib de al menos 6 letras.
5. No se implementan rotors griegos (M4), UKW-D ni Uhr.
6. El texto se reduce a A-Z. No hay cifras, puntuacion ni el teclado historico de 26 teclas con lámpara.
7. El ESP32 no cifra en tiempo real tecla a tecla con teclado mecanico: la entrada es web.
8. Los LED no se han validado en hardware en el entorno de desarrollo.
9. El punto de acceso usa WPA2 con clave en claro en el firmware. Cambiar `WIFI_AP_PASSWORD` antes de una exposicion publica si se desea.
10. La lista de candidatos en pantalla esta limitada a 32 filas. El contador total de candidatas no se recorta.
11. No hay persistencia: al reiniciar el ESP32 se pierde el estado.
12. PlatformIO no estaba disponible en el entorno de compilacion del proyecto; el nucleo C++ si se compilo y se probo en PC.
