from IPython.display import SVG

class TSP:
    def __init__(self, g):
        self.graph = g
        self.nodes = g.nodes()
        self.edges = g.edges()
        self.best_path = None
        self.best_distance = 1000000


    def get_distance(self, node_1, node_2):
        try:
            return int(self.graph.get_edge(node_1, node_2).attr['xlabel'])
        except Exception:
            return int(self.graph.get_edge(node_2, node_1).attr['xlabel'])

    def get_neighbors(self, node):
        neighbors = []
        for edge in self.edges:
            if edge[0] == node:
                neighbors.append(edge[1])
            elif edge[1] == node:
                neighbors.append(edge[0])

        return neighbors


    def pre_drawing(self):
        """Reset the color and the penwidth for all edges"""
        self.graph.edge_attr['color'] = '#a0a0a4'
        self.graph.edge_attr['penwidth'] = 1


    def highlight_path(self):
        """Draw the path to be followed by the salesman"""
        self.pre_drawing()
        for e in range(1, len(self.best_path)):
            self.graph.get_edge(self.best_path[e], self.best_path[e-1]).attr['color'] = 'red'
            self.graph.get_edge(self.best_path[e], self.best_path[e-1]).attr['penwidth'] = 5


    def algorithm(self, start):
        """Brute force algorithm that gives the exact solution for a TSP"""
        N = len(self.nodes)

        def algorithm_continue(visited_nodes, current_traveled_distance):
            best_path = None
            best_distance = 100000

            if len(visited_nodes) == N:
                if visited_nodes[0] in self.get_neighbors(visited_nodes[-1]):
                    return visited_nodes+[visited_nodes[0]], current_traveled_distance+self.get_distance(visited_nodes[0], visited_nodes[-1])
                
                return best_path, best_distance

            current_node = visited_nodes[-1]
            neighbors = self.get_neighbors(current_node)
            for neighbor in neighbors:
                if not neighbor in visited_nodes:
                    _visited_nodes = visited_nodes.copy()
                    _visited_nodes.append(neighbor)
                    path, distance = algorithm_continue(
                        _visited_nodes, 
                        current_traveled_distance + self.get_distance(current_node, neighbor)
                    )
                    if distance < best_distance:
                        best_distance = distance
                        best_path = path

            return best_path, best_distance

            
        self.best_path, self.best_distance = algorithm_continue([start], 0)
        self.highlight_path()