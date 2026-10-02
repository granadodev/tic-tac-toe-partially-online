const WebSocket = require('ws'); // Agregadas comillas
const wss = new WebSocket.Server({ port: 8082 });

// Mensaje opcional para saber que el servidor ya está escuchando
console.log('Servidor WebSocket corriendo en el puerto 8082');

wss.on('connection', ws => { // Agregadas comillas
    console.log('¡Nuevo cliente conectado!'); // Agregadas comillas
    
    ws.on('close', () => { // Agregadas comillas
        console.log('Cliente desconectado'); // Agregadas comillas
    });
});
