import { initialObjectsScenario } from './objects.scenario.js'
import { sensorsScenario } from './sensors.scenario.js'
import { aRouteScenario } from './a-route.scenario.js'
import { islandsScenario } from './islands.scenario.js'

export function applyRestEndpoints (fastify) {
  initialObjectsScenario(fastify)
  sensorsScenario(fastify)
  aRouteScenario(fastify)
  islandsScenario(fastify)
}
