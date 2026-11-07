#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// Instancias del servidor HTTP y WebSocket
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// DATOS DEL JUEGO (Igual a las variables globales de tu index.js)
bool playerXIsConnect = false;
bool playerOIsConnect = false;
String currentPlayerTurn = "X";

// Guardar el rol de cada cliente
struct ClientState {
  String role;
};

// Función para asignar "X", "O" o "N" (lleno)
String assignPlayerId() {
  if (!playerXIsConnect) {
    playerXIsConnect = true;
    return "X";
  } else if (!playerOIsConnect) {
    playerOIsConnect = true;
    return "O";
  } else {
    return "N";
  }
}

// Manejador de eventos de WebSockets (Oye los mensajes del cliente)
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  
  // 1. NUEVO CLIENTE CONECTADO
  if (type == WS_EVT_CONNECT) {
    String playerId = assignPlayerId();

    ClientState *state = new ClientState();
    state->role = playerId;
    client->_tempObject = state;

    // Crear JSON { type: 'assingName', payload: { player: playerId } }
    DynamicJsonDocument doc(256);
    doc["type"] = "assingName";
    doc["payload"]["player"] = playerId;

    String response;
    serializeJson(doc, response);
    client->text(response);

    // Si está lleno, cerrar conexión
    if (playerId == "N") {
      client->close(4000, "Conexión terminada por el servidor");
    }
  } 
  
  // 2. CLIENTE DESCONECTADO
  else if (type == WS_EVT_DISCONNECT) {
    if (client->_tempObject != NULL) {
      ClientState *state = (ClientState *)client->_tempObject;
      if (state->role == "X") playerXIsConnect = false;
      if (state->role == "O") playerOIsConnect = false;
      delete state;
    }
  } 
  
  // 3. MENSAJE RECIBIDO (clickTile)
  else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      
      DynamicJsonDocument doc(512);
      DeserializationError error = deserializeJson(doc, data, len);
      if (error) return;

      const char* msgType = doc["type"];

      if (strcmp(msgType, "clickTile") == 0) {
        const char* incomingPlayerId = doc["payload"]["playerId"];
        int tileId = doc["payload"]["tileId"];

        // Validar si es el turno del jugador que envió el click
        if (String(incomingPlayerId) != currentPlayerTurn) {
          return;
        }

        // Preparar broadcast (updateBoard)
        DynamicJsonDocument answerDoc(512);
        answerDoc["type"] = "updateBoard";
        answerDoc["payload"]["tile"] = tileId;
        answerDoc["payload"]["playerId"] = incomingPlayerId;

        String answer;
        serializeJson(answerDoc, answer);

        // Enviar a todos los conectados (wss.clients.forEach en Node)
        ws.textAll(answer);

        // Alternar turno
        currentPlayerTurn = (currentPlayerTurn == "X") ? "O" : "X";
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Iniciar memoria para guardar HTML/CSS/JS
  if (!LittleFS.begin()) {
    Serial.println("Error al montar LittleFS");
    return;
  }

  // Crear punto de acceso Wi-Fi propio del ESP8266
  WiFi.softAP("TicTacToe-ESP8266", "12345678");
  Serial.print("Servidor listo en: http://");
  Serial.println(WiFi.softAPIP());

  // Configurar rutas y WebSockets
  ws.onEvent(onEvent);
  server.addHandler(&ws);
  
  // Servir archivos de la carpeta 'data' automáticamente
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server.begin();
}

void loop() {
  ws.cleanupClients();
}