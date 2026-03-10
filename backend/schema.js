export const objectSchema = {
  type: 'object',
  properties: {
    id: { type: 'integer' },
    name: { type: 'string' },
    position: {
      type: 'object',
      properties: {
        x: { type: 'number' },
        y: { type: 'number' },
        z: { type: 'number' }
      }
    },
    status: { type: 'string' },
    type: { type: 'string' },
    temperature: { type: 'number' },
    lastUpdate: { type: 'string' }
  }
}
