const WebSocket = require('ws'); // Agregadas comillas
const wss = new WebSocket.Server({ port: 8082 });

// Mensaje opcional para saber que el servidor ya está escuchando
console.log('Servidor WebSocket corriendo en el puerto 8082');

//DATOS DEL JUEGO
let playerXIsConnect = false;
let playerOIsConnect = false;
let currentPlayerTurn = "X"

wss.on('connection', ws => { // Agregadas comillas
    console.log('¡Nuevo cliente conectado!'); // Agregadas comillas

    //PROPOSITO: Enviar un JSON para asignar un X o Y al nueo jugador
    let playerId = assignPlayerId()
    const answer = JSON.stringify({
        type: 'assingName',
        payload: {
            player: playerId
            }
        });
    console.log(answer);
    ws.send(answer)

    //Si estamos llenos cerramos la conexion
    if (playerId === "N"){
        ws.close(4000, "Conexión terminada por el servidor");
        return
    }
    
    //PROPOSITO: Detectar si se cierra la conexion
    ws.on('close', () => { // Agregadas comillas
        console.log('Cliente desconectado'); // Agregadas comillas
    });

    //PROPOSITO: Oir los mensajes enviados por este canal
    ws.on('message', (data) => {
        // A. Convertir el Buffer a String y parsear el JSON
        const text = data.toString();
        let messegeData;

        try {
        messegeData = JSON.parse(text);
        } catch (e) {
        console.error('El mensaje no es un JSON válido:', text);
        return;
        }

        console.log("SS");
        console.log(messegeData);
        // B. Enrutador de eventos según el 'tipo'
        switch (messegeData.type) {
            case 'clickTile': {
                //Comprobar si es del juagdor actual
                if (messegeData.payload.playerId !== currentPlayerTurn){
                    return
                }

                // Broadcast de un mensaje de chat a TODOS los clientes conectados
                const answer = JSON.stringify({
                type: 'updateBoard',
                payload: {
                    emisor: ws.id,
                    tile: messegeData.payload.tileId,
                    playerId: messegeData.payload.playerId
                }
                });

                wss.clients.forEach((client) => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(answer);
                }
                });

                //Cambiar el jugador
                currentPlayerTurn = currentPlayerTurn === "X" ? "O" : "X";
                console.log(currentPlayerTurn);


                break;
            }
        }

    });
    

});

function assignPlayerId(){
    if (playerXIsConnect === false){
        playerXIsConnect = true;
        return "X"
    } else if (playerXIsConnect === true){
        playerOIsConnect = true;
        return "O"
    } else {
        return "N"
    }
}