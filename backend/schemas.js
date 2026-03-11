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

export const anIsland = {
  type: 'object',
  additionalProperties: false,
  required: ['id', 'pos', 'bars'],
  properties: {
    id: { type: 'string', minLength: 1 },
    label: { type: 'string' },
    pos: {
      type: 'array',
      prefixItems: [
        { type: 'number', minimum: -1000, maximum: 1000 }, // X
        { type: 'number', minimum: -1000, maximum: 1000 }, // Y
        { type: 'number', const: 0 }                        // Z
      ],
      minItems: 3,
      maxItems: 3
    },
    bars: {
      type: 'array',
      minItems: 5,
      maxItems: 5,
      items: { type: 'number', minimum: 0 }
    },
    rotation: { type: 'number' },
    scale: { type: 'number', exclusiveMinimum: 0 },
    palette: {
      type: 'array',
      minItems: 5,
      maxItems: 5,
      items: { type: 'string' }
    }
  }
}
