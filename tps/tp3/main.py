from graph import Node, Graph

n = 5

g = Graph.generate_random_undericted(n)
print(g)
print()
g.serialize()
print()
print(Graph.load())
