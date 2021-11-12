from graph import Node, Graph
from pyvis.network import Network

n = 20
g = Graph.generate_random(n)
print(g)
print()
g.serialize()
print()
print(Graph.load())

Graph.vis(g, "./res.html")
