import random
from pyvis.network import Network

class Node:
    _counter: int = 0

    def __init__(self, neighbors: set['Node'], val=0, id: str=None) -> None:
        self._id = id
        if not id:
            self._id = Node._counter
        Node._counter += 1
        self.val = val
        self.neighbors = neighbors

    def __hash__(self) -> int:
        return hash(self._id)

    def __eq__(self, o: 'Node') -> bool:
        return self._id == o._id

    def __str__(self) -> str:
        return f"{self._id}: {self.val}, {{{', '.join([str(n._id) for n in self.neighbors])}}}"
    
    @property
    def id(self):
        return self._id


class Graph:
    def __init__(self, nodes: dict[str, Node]) -> None:
        self.nodes = nodes
    
    def __str__(self) -> str:
        return '\n'.join([node.__str__() for node in self.nodes.values()])
    
    def get_node(self, id: str) -> Node:
        return self.nodes.get(id)
    
    def is_there(self, id: str) -> bool:
        return id in self.nodes

    def add_node(self, node: Node) -> bool:
        if node.id in self.nodes: return False
        self.nodes[node.id] = node
        return True
    
    def remove_node(self, id: str) -> Node:
        to_remove = self.nodes.pop(id)
        if to_remove:
            for node in self.nodes.values():
                node.neighbors.discard(to_remove)
        return to_remove
    
    def is_connected(self, node1: str, node2: str) -> bool:
        return node2 in self.nodes[node1].neighbors
    
    def connect(self, node1: str, node2: str) -> bool:
        old = self.is_connected(node1, node2)
        if not old:
            self.nodes[node1].neighbors.add(self.nodes[node2])
        return old

    @classmethod
    def generate_random_undericted(cls, size: int, factor: float=0.5) -> 'Graph':
        g = Graph({})
        nodes: list[Node] = []
        for _ in range(size):
            n = Node(set(), round(random.random(), 2))
            nodes.append(n)
            g.add_node(n)
        for i in range(size):
            for j in range(i+1, size):
                if random.random() < factor:
                    nodes[i].neighbors.add(nodes[j])
                    nodes[j].neighbors.add(nodes[i])
        return g

    @classmethod
    def generate_random(cls, size: int, factor: float=0.5) -> 'Graph':
        g = Graph({})
        for _ in range(size):
            n = Node(set(), round(random.random(), 2))
            g.add_node(n)
        for node_i in g.nodes.values():
            for node_j in g.nodes.values():
                if node_i == node_j: continue
                if random.random() < factor:
                    node_i.neighbors.add(node_j)
        return g

    @classmethod
    def load(cls, file_path: str='./graph_serialized.txt') -> 'Graph':
        with open(file_path, 'r', encoding="UTF-8") as f:
            g = Graph({})
            nodes = f.readline().split()
            i = 0
            for n in nodes:
                n = n.split(':')
                if len(n) == 2:
                    id = n[0]
                else:
                    id = i
                    i += 1
                g.add_node(Node(set(), n[-1], id))
            nb_edges = int(f.readline())
            for _ in range(nb_edges):
                e = f.readline().split()
                g.connect(e[0], e[1])
        return g

    @classmethod
    def serialize(cls, graph: 'Graph', file_path: str='./graph_serialized.txt') -> None:
        with open(file_path, 'w', encoding="UTF-8") as f:
            nodes_line = ' '.join([f"{n.id}:{n.val}" for n in graph.nodes.values()])
            edges = []
            for n in graph.nodes.values():
                for e in n.neighbors:
                    edges.append(f"{n.id} {e.id}")
            nb_edges = len(edges)
            f.write('\n'.join([nodes_line, str(nb_edges), *edges]))

    @classmethod
    def vis(
        cls, graph: 'Graph', file_path: str, height: str="700px", width: str="100%",
        directed: bool=True, notebook: bool=False
    ) -> None:
        net = Network(height, width, directed, notebook)
        for node_id in graph.nodes:
            net.add_node(node_id, shape="circle")
        for node1 in graph.nodes.values():
            for node2 in node1.neighbors:
                net.add_edge(node1.id, node2.id)
        net.toggle_physics(False)
        net.show(file_path)