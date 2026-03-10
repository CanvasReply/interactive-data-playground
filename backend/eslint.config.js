import neostandard from 'neostandard'

export default [
  // ...neostandard({ ts: true }),
  ...neostandard(),
  { ignores: ['node_modules/**', 'dist/**', 'build/**', 'coverage/**'] }
]
