# Tic-Tac-Toe Local con ESP8266

¡El clásico juego de **Tres en Raya (Tic-Tac-Toe)** multijugador en tiempo real! 

A diferencia de los juegos en línea tradicionales, este proyecto utiliza un microcontrolador **ESP8266** como servidor web autónomo y punto de acceso Wi-Fi. No requiere conexión a Internet ni una computadora encendida para el backend: solo alimenta el ESP8266 y juega desde cualquier smartphone, tablet o laptop.

---

## ¿Cómo funciona?

1. **Red Wi-Fi Autónoma:** El ESP8266 crea su propia red Wi-Fi local (Punto de Acceso / AP) usando su módulo integrado.
2. **Conexión de Jugadores:** Los dispositivos móviles o laptops se conectan a la red del ESP8266.
3. **Servidor HTTP & WebSockets:** El ESP8266 sirve los archivos web (`index.html`, `style.css`, `script.js`) almacenados en su memoria Flash (LittleFS) y establece un canal de **WebSockets** bidireccional en tiempo real.
4. **Sincronización:** Cada vez que un jugador realiza un movimiento:
   - El cliente envía la jugada por WebSocket.
   - El servidor en C++ valida el turno y transmite (*broadcast*) la actualización del tablero a todos los clientes conectados.
   - El servidor gestiona el estado del juego (turnos, victorias y empates).

---

## Requisitos del Sistema

### Hardware
* **ESP8266** (NodeMCU v1.0, ESP-12E o similar).
* Cable Micro-USB / USB-C (de datos, para alimentación y programación).

### Software & Herramientas
* [Visual Studio Code](https://code.visualstudio.com/)
* Extensión **PlatformIO IDE** para VS Code.
* [Git](https://git-scm.com/) (para clonar el repositorio).

---

## Librerías y Dependencias

El backend en C++ utiliza las siguientes librerías gestionadas automáticamente por **PlatformIO**:

* **[ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer):** Servidor HTTP asíncrono y gestor de WebSockets no bloqueante para el ESP8266.
* **[ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP):** Librería base de socket TCP asíncrono sobre la cual opera `ESPAsyncWebServer`.
* **[ArduinoJson](https://arduinojson.org/):** Librería eficiente para serializar y deserializar los mensajes JSON intercambio entre cliente y servidor.
* **[LittleFS](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html):** Sistema de archivos liviano para leer la partición Flash donde se almacenan el HTML, CSS y JS.
* **[ESP8266WiFi](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html):** Control del módulo de red Wi-Fi integrado para crear el Access Point (AP).

---

## 📁 Estructura del Proyecto

```text
tic-tac-toe-esp8266/
├── data/                 <-- Archivos estáticos del Frontend (Servidos a los clientes)
│   ├── index.html        
│   ├── script.js        
│   └── style.css        
├── src/
│   └── main.cpp          <-- Backend en C++ (Gestión de Wi-Fi, HTTP y WebSockets)
└── platformio.ini        <-- Configuración de placa y dependencias C++
```

---


## Instrucciones de Juego

1. Alimenta el ESP8266 conectándolo a cualquier puerto USB o cargador de celular de 5V.
2. Conéctate a la red Wi-Fi generada por el chip:
   - **SSID:** `TicTacToe-ESP8266`
   - **Contraseña:** `12345678`
3. Abre el navegador web en dos dispositivos e ingresa a la IP: `http://192.168.4.1`.
4. ¡A jugar! El primer dispositivo en entrar será el jugador **X** y el segundo el jugador **O**.