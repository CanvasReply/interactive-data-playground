import { objectSchema } from '../schemas.js'

/* ================================
    Scenario 2.
    The endpoint spawns a sensor's data, UE draws it with colored spheres (color depends on temperature).
 =============================== */

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

export function sensorsScenario (fastify) {
  fastify.get('/api/sensors', {
    schema: {
      response: {
        200: {
          type: 'array',
          items: objectSchema
        }
      }
    }
  }, async (req, reply) => {
    const next = pickNext()
    if (next) ACC.push(makeSensorAt(next))
    return ACC
  })
}
