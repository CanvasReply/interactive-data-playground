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

export const aRouteScenarioSchema = {
  type: 'object',
  properties: {
    start: {
      type: 'object',
      properties: {
        x: { type: 'number' },
        y: { type: 'number' },
        z: { type: 'number' }
      },
    },
    end: {
      type: 'object',
      properties: {
        x: { type: 'number' },
        y: { type: 'number' },
        z: { type: 'number' },
      }
    },
    speed: { type: 'number' },
    mode: { type: 'string' }
  }
}
