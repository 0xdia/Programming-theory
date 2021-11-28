from graph import Node, Graph
from pyvis.network import Network

n = 20
g = Graph.generate_random_undericted(n)
print(g)
print()
Graph.serialize(g)
print()
print(Graph.load())

Graph.vis(g, "./vis.html", directed=False)
