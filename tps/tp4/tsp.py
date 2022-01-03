#!/usr/bin/env python3

import pygraphviz as pgv

class TSP:
    def __init__(self, g):
        self.graph = g
        self.nodes = g.nodes()
        self.edges = g.edges()
        self.best_path = None
        self.best_distance = None


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
        for edge in self.best_path:
            self.graph.get_edge(edge[0], edge[1]).attr['color'] = 'red'
            self.graph.get_edge(edge[0], edge[1]).attr['penwidth'] = 5


    def algorithm(self, start):
        """Brute force algorithm that gives the exact solution for a TSP"""
        self.best_path = None
        self.best_distance = 1000000000000

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
        
        path_as_sequence_of_edges = []
        for i in range(1, len(self.best_path)):
            path_as_sequence_of_edges.append((self.best_path[i-1], self.best_path[i]))

        self.best_path = path_as_sequence_of_edges
        self.highlight_path()

    def hill_climbing(self, start):
        """Hill climbing heuristic"""
        self.best_path = [start]
        self.best_distance = 0
        N = len(self.nodes)

        while len(self.best_path) < N:
            we = []
            for edge in self.edges:
                nei = self.get_neighbors(self.best_path[-1])
                for n in nei:
                    we.append((self.get_distance(self.best_path[-1], n), self.best_path[-1], n))

            we.sort()
            for w in we:
                """ if not w[1] in self.best_path:
                    self.best_distance += w[0]
                    self.best_path.append(w[1])
                    break """
                if not w[2] in self.best_path:
                    self.best_distance += w[0]
                    self.best_path.append(w[2])
                    break

        self.best_distance += self.get_distance(start, self.best_path[-1])
        self.best_path.append(start)
        path_as_sequence_of_edges = []
        for i in range(1, len(self.best_path)):
            path_as_sequence_of_edges.append((self.best_path[i-1], self.best_path[i]))

        self.best_path = path_as_sequence_of_edges
        self.highlight_path()


if __name__ == '__main__':
    g = pgv.AGraph("./graph.dot").to_undirected()
    instance = TSP(g)