from graph import Node, Graph
from pyvis.network import Network

n = 8
g = Graph.generate_random(n)
print(g)
print()
Graph.serialize(g)
print()
print(Graph.load())

Graph.vis(g, "./res.html")
