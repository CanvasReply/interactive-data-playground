/* ================================
    Scenario 1.
    Objects from endpoint must be available in UE
 =============================== */

import { objectSchema } from '../schemas.js'

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

export function initialObjectsScenario (fastify) {
  fastify.get('/api/objects', {
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
}
