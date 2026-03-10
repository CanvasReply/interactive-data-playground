import Fastify from 'fastify'
import websocket from '@fastify/websocket'
import swagger from '@fastify/swagger'
import swaggerUi from '@fastify/swagger-ui'
import { objectSchema } from './schema.js'

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

const objects = [
  {
    id: 1,
    name: 'Sensor A',
    position: { x: 10, y: 20, z: 0 },
    status: 'active',
    type: 'sensor',
    temperature: 23.7,
    lastUpdate: new Date().toISOString()
  },
  {
    id: 2,
    name: 'Sensor B',
    position: { x: 30, y: 50, z: 0 },
    status: 'warning',
    type: 'sensor',
    temperature: 75.2,
    lastUpdate: new Date().toISOString()
  }
]

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
  return objects
})

const GRID_SPACING = 2
const GRID_POINTS = []
for (let x = -250; x <= 250; x += GRID_SPACING) {
  for (let y = -250; y <= 250; y += GRID_SPACING) {
    GRID_POINTS.push({ x, y })
  }
}

const Z_MIN = 10; const Z_MAX = 60
const ACC = []
const USED = new Set()
let lastPoint = null

const key = ({ x, y }) => `${x},${y}`
const isClose = (p, last) => {
  if (!last) return false
  return Math.abs(p.x - last.x) <= GRID_SPACING &&
        Math.abs(p.y - last.y) <= GRID_SPACING
}

function pickNext () {
  const unused = GRID_POINTS.filter(p => !USED.has(key(p)))
  if (unused.length === 0) return null

  const far = unused.filter(p => !isClose(p, lastPoint))
  const pool = far.length ? far : unused // фолбэк, если «далёких» не осталось

  const p = pool[Math.floor(Math.random() * pool.length)]
  USED.add(key(p))
  lastPoint = p
  return p
}

function makeStatus (temperature) {
  if (temperature > 85) return 'error'
  if (temperature > 70) return 'warning'
  if (temperature > 5) return 'active'
  return 'inactive'
}

function makeSensorAt ({ x, y }) {
  const temperature = +(Math.random() * 95).toFixed(1)
  return {
    position: {
      x,
      y,
      z: +(Math.random() * (Z_MAX - Z_MIN) + Z_MIN).toFixed(2),
    },
    temperature: temperature / 10,
    status: makeStatus(temperature),
  }
}

app.get('/api/sensors', {
  schema: {
    response: {
      200: {
        type: 'array',
        items: objectSchema, // не забудьте добавить поле scale: { type: 'number' }
      }
    }
  }
}, async (req, reply) => {
  const next = pickNext()
  if (next) ACC.push(makeSensorAt(next))
  return ACC
})

/* ================================
    WebSocket endpoint
 =============================== */

app.register(websocket)
app.get('/ws', { websocket: true }, (connection, req) => {
  const interval = setInterval(() => {
    objects[0].temperature = (20 + Math.random() * 10).toFixed(1)
    objects[0].lastUpdate = new Date().toISOString()
    connection.socket.send(JSON.stringify(objects[0]))
  }, 5000)

  connection.socket.on('close', () => clearInterval(interval))
})

app.listen({ port: 3000 }, err => {
  if (err) throw err
  console.log('Server started on port 3000')
})
