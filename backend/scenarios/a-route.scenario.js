import { aRouteScenarioSchema } from '../schemas.js'

/* ================================
    Scenario 3.
    The endpoint gives a random start and end point, and UE moves an actor between them.
 =============================== */

const randomScenarioQuery = {
  type: 'object',
  properties: {
    xmin: { type: 'number', default: -1000 },
    xmax: { type: 'number', default: 1000 },
    ymin: { type: 'number', default: -1000 },
    ymax: { type: 'number', default: 1000 },
    z: { type: 'number', default: 40 },            // @NOTE: UE: centimeters!
    speedMin: { type: 'number', default: 200 },
    speedMax: { type: 'number', default: 400 },
    mode: { type: 'string', default: 'straight' },
    withinBounds: { type: 'boolean', default: true }
  },
  additionalProperties: false
}

function randRange (min, max) {
  return min + Math.random() * (max - min)
}

export function aRouteScenario (fastify) {
  fastify.get('/api/a-route', {
    schema: {
      querystring: randomScenarioQuery,
      response: { 200: aRouteScenarioSchema }
    }
  }, async (request, reply) => {
    const {
      xmin = -1000, xmax = 1000,
      ymin = -1000, ymax = 1000,
      z = 40,
      speedMin = 200, speedMax = 400,
      mode = 'straight',
      withinBounds = true
    } = request.query

    if (withinBounds) {
      const width = xmax - xmin
      const height = ymax - ymin
      const maxPossible = Math.hypot(width, height)
      if (maxPossible < 400) {
        return reply.code(400).send({
          error: 'Space size is too low. Minimum size is 400'
        })
      }
    }

    const maxAttempts = 50
    let start = null
    let end = null

    for (let attempt = 0; attempt < maxAttempts; attempt++) {
      const sx = randRange(xmin, xmax)
      const sy = randRange(ymin, ymax)

      const d = randRange(400, 1000)            // расстояние в см
      const theta = randRange(0, 2 * Math.PI)

      const ex = sx + d * Math.cos(theta)
      const ey = sy + d * Math.sin(theta)

      if (!withinBounds ||
                (ex >= xmin && ex <= xmax && ey >= ymin && ey <= ymax)) {
        start = { x: sx, y: sy, z }
        end = { x: ex, y: ey, z }
        break
      }
    }

    if (!start) {
      return reply.code(400).send({
        error: 'Error on generating route: could not find valid start/end points within bounds after multiple attempts. Please adjust the area or disable bounds checking.'
      })
    }

    const speed = randRange(speedMin, speedMax)
    return { start, end, speed, mode }
  })
}
