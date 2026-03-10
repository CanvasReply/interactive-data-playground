import Fastify from 'fastify'
import swagger from '@fastify/swagger'
import swaggerUi from '@fastify/swagger-ui'
import { applyRestEndpoints } from './scenarios/_index.js'

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

applyRestEndpoints(app)

/* ================================
    WebSocket endpoint

app.register(websocket)
app.get('/ws', { websocket: true }, (connection, req) => {
  const interval = setInterval(() => {
    objects[0].temperature = (20 + Math.random() * 10).toFixed(1)
    objects[0].lastUpdate = new Date().toISOString()
    connection.socket.send(JSON.stringify(objects[0]))
  }, 5000)

  connection.socket.on('close', () => clearInterval(interval))
})
 =============================== */

app.listen({ port: 3000 }, err => {
  if (err) throw err
  console.log('Server started on port 3000')
})
