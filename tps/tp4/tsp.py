from IPython.display import SVG

class TSP:
    def __init__(self, g):
        self.graph = g
        self.nodes = g.nodes()
        self.edges = g.edges()

    def get_distance(self, node_1, node_2):
        try:
            return self.graph.get_edge(node_1, node_2).attr['xlabel']
        except Exception:
            return self.graph.get_edge(node_2, node_1).attr['xlabel']


    def pre_drawing(self):
        """Reset the color and the penwidth for all edges"""
        self.graph.edge_attr['color'] = '#a0a0a4'
        self.graph.edge_attr['penwidth'] = 1

    def draw(self):
        """Draw the path to be followed by the salesman"""
        self.pre_drawing()
        """ for edge in path:
            self.graph.get_edges(edge[0], edge[1]).attr['color'] = 'red'
            self.graph.get_edges(edge[0], edge[1]).attr['penwidth'] = 5 """

        self.graph.layout()
        self.graph.draw("result.svg")
        SVG("./result.svg")

    def algorithm(self, start):
        """Brute force algorithm that gives the exact solution for a TSP"""
        pass