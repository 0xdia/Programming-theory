import random
from typing import cast

class Node:
    __counter: int = 0

    def __init__(self, neighbors: set['Node'], val=0, id: str=None) -> None:
        Node.__counter += 1
        self.__id = id
        if not id:
            self.__id = Node.__counter
        self.val = val
        self.neighbors = neighbors

    def __hash__(self) -> int:
        return hash(self.__id)

    def __eq__(self, o: 'Node') -> bool:
        return self.__id == o.__id

    def __str__(self) -> str:
        return f"{self.val}, {{{', '.join([str(n.__id) for n in self.neighbors])}}}"
    
    def get_id(self) -> int:
        return self.__id


class Graph:
    def __init__(self, nodes: set[Node]) -> None:
        self.nodes = nodes
    
    def __str__(self) -> str:
        return '\n'.join([node.__str__() for node in self.nodes])

    @classmethod
    def generate_random_undericted(cls, size: int, factor: float=0.5) -> 'Graph':
        g = Graph(set())
        nodes = []
        for _ in range(size):
            n = Node(set(), random.random())
            nodes.append(n)
            g.nodes.add(n)
        for i in range(size):
            for j in range(i+1, size):
                if random.random() < factor:
                    nodes[i].neighbors.add(nodes[j])
                    nodes[j].neighbors.add(nodes[i])
        return g
    
    @classmethod
    def generate_random(cls, size: int, factor: float=0.5) -> 'Graph':
        g = Graph(set())
        for _ in range(size):
            n = Node(set(), random.random())
            g.nodes.add(n)
        for node_i in g.nodes:
            for node_j in g.nodes:
                if node_i == node_j: continue
                if random.random() < factor:
                    node_i.neighbors.add(node_j)
        return g
