import Fastify from 'fastify';
import websocket from '@fastify/websocket';

const fastify = Fastify({ logger: true })

fastify.register(websocket);

let objects = [
    {
        id: 1,
        name: "Sensor A",
        position: { x: 10, y: 20, z: 0 },
        status: "active",
        type: "sensor",
        temperature: 23.7,
        lastUpdate: new Date().toISOString()
    },
    {
        id: 2,
        name: "Sensor B",
        position: { x: 30, y: 50, z: 0 },
        status: "warning",
        type: "sensor",
        temperature: 75.2,
        lastUpdate: new Date().toISOString()
    }
];

// REST endpoint
fastify.get('/api/objects', async (req, reply) => {
    return objects;
});

// WebSocket endpoint
fastify.get('/ws', { websocket: true }, (connection, req) => {
    const interval = setInterval(() => {
        objects[0].temperature = (20 + Math.random() * 10).toFixed(1);
        objects[0].lastUpdate = new Date().toISOString();
        connection.socket.send(JSON.stringify(objects[0]));
    }, 5000);

    connection.socket.on('close', () => clearInterval(interval));
});

fastify.listen({ port: 3000 }, err => {
    if (err) throw err;
    console.log('Server started on port 3000');
});