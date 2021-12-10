from pygraphviz import Node
import pygraphviz as pgv

def articulation_points(g: pgv.AGraph) -> list[Node]:
    if g.is_directed():
        return None
    sol = []
    timer = 0
    def custom_dfs(n: Node, p: Node=None):
        nonlocal timer, g
        n.attr['visited'] = True
        timer += 1
        n.attr['tin'] = n.attr['low'] = timer
        children = 0
        for adj in g.neighbors(n):
            if p and adj.get_name() == p.get_name():
                continue
            if adj.attr.get('visited', False):
                n.attr['low'] = min(n.attr['low'], adj.attr['tin'])
            else:
                children += 1
                custom_dfs(adj, n)
                n.attr['low'] = min(n.attr['low'], adj.attr['low'])
                if adj.attr['low'] >= n.attr['tin'] and p:
                    sol.append(n)
        if p is None and children > 1:
            sol.append(n)

    for node in g.nodes():
        if not node.attr.get('visited', False):
            custom_dfs(node)
    for node in g.nodes():
        del node.attr['visited']
        del node.attr['tin']
        del node.attr['low']
    return sol


if __name__ == "__main__":
    g = pgv.AGraph("./graph0.gv").to_undirected()
    aps = articulation_points(g)
    print(aps)
    for ap in aps:
        ap.attr['fillcolor'] = 'red'
    g.layout()
    g.draw("result.svg")
