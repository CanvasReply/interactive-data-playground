# What do we need to create a prototype

## Define the goal and use case

What do we want to achieve? 

For example:
- Visualize objects on a real-world map and select them for analysis.
- Choose the optimal route.
- Monitor the condition of equipment in a factory.

It should be a single simple task to start with (a "minimum viable prototype").

## Make a list of required data
What objects/events/parameters need to be displayed?

Example: object coordinates, their status, additional parameters (temperature, speed, etc.).

## Decide where to get the data from
For a prototype, we can simply "hardcode" an array of objects in Node.js.
In the future, connect real data sources (databases, external APIs, sensors).

If the data we use during development (the hardcoded data) is very different from the data
required for the application, making changes may take too much time.
Therefore, it’s better to plan everything carefully in advance.