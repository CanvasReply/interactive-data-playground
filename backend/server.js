import Fastify from 'fastify';
import websocket from '@fastify/websocket';
import swagger from '@fastify/swagger';
import swaggerUi from '@fastify/swagger-ui';
import { objectSchema } from "./schema.js";

const app = Fastify({ logger: true })

/* ================================
    Swagger Documentation Setup
 =============================== */
await app.register(swagger, {
    openapi: {
        openapi: '3.0.0',
        info: {
            title: 'Interactive Data Visualization API',
            description: 'API for providing real-time data for interactive visualizations',
            version: '1.0.0'
        },
        tags: [
            { name: 'user', description: 'User related end-points' },
            { name: 'code', description: 'Code related end-points' }
        ],
        components: {
            securitySchemes: {
                apiKey: {
                    type: 'apiKey',
                    name: 'apiKey',
                    in: 'header'
                }
            }
        },
    }
})
await app.register(swaggerUi, {
    routePrefix: '/documentation',
    uiConfig: {
        docExpansion: 'full',
        deepLinking: false
    },
    uiHooks: {
        onRequest: function (request, reply, next) { next() },
        preHandler: function (request, reply, next) { next() }
    },
    staticCSP: true,
    transformStaticCSP: (header) => header,
    transformSpecification: (swaggerObject, request, reply) => { return swaggerObject },
    transformSpecificationClone: true
})

/* ================================
    Fake Data
 =============================== */

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

/* ================================
    REST endpoint
 =============================== */

app.get('/api/objects', {
    schema: {
        response: {
            200: {
                type: 'array',
                items: objectSchema,
            }
        }
    }
}, async (req, reply) => {
    return objects;
});

/* ================================
    WebSocket endpoint
 =============================== */

app.register(websocket);
app.get('/ws', { websocket: true }, (connection, req) => {
    const interval = setInterval(() => {
        objects[0].temperature = (20 + Math.random() * 10).toFixed(1);
        objects[0].lastUpdate = new Date().toISOString();
        connection.socket.send(JSON.stringify(objects[0]));
    }, 5000);

    connection.socket.on('close', () => clearInterval(interval));
});

app.listen({ port: 3000 }, err => {
    if (err) throw err;
    console.log('Server started on port 3000');
});