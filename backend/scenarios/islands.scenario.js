import { anIsland, } from '../schemas.js'

export function islandsScenario (fastify) {
  fastify.get('/api/islands',
    {
      schema: {
        response: {
          200: {
            type: 'array',
            items: anIsland,
          }
        }
      }
    },
    async (req, reply) => {
      return [
        { id: 'I1', pos: [-350, -700, 0], bars: [12, 5, 18, 7, 3], label: 'Alpha' },
        { id: 'I2', pos: [550, -650, 0], bars: [9, 2, 11, 4, 6], label: 'Beta' },
        { id: 'I3', pos: [-700, -110, 0], bars: [20, 3, 14, 8, 10], label: 'Gamma' },
        { id: 'I4', pos: [200, 700, 0], bars: [7, 9, 5, 6, 12], label: 'Delta' },
        { id: 'I5', pos: [700, 100, 0], bars: [4, 15, 6, 2, 9], label: 'Epsilon' }
      ]
    }
  )
}
