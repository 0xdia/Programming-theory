#include <bits/stdc++.h>

using namespace std;

#define rall(x) x.rbegin(), x.rend()
#define v1 second.first
#define v2 second.second
#define weight first

typedef pair<int, pair<int, int>> edge;    // (weight, (vertex1, vertex2))
typedef pair<int, int> neighbour;          // (edge weight connecting them, neighbour vertex)
typedef pair<int, int> position;           // (cost, vertex)

class HillClimbingSolver {
  public:
    HillClimbingSolver(int v, int e, vector<edge> edges)
    : _v(v), _e(e)
    { 
      _graph = vector<vector<neighbour>>(v);
      for (edge& e: edges) {
        _graph[e.v1].push_back(neighbour(e.weight, e.v2));
        _graph[e.v2].push_back(neighbour(e.weight, e.v1));
      }
      
      _visited.assign(v, false);
    }

    int climb(int root, int peak) {
      stack<position> stk;
      stk.push(position(0, root));
      
      while (stk.empty() == false) {
        position x = stk.top();
        stk.pop();

        if (x.second == peak) {
          return x.first;
        }

        if (! _visited[x.second]) {
          _visited[x.second] = true;
          vector<position> positions;
          for (neighbour& nbr: _graph[x.second]) {
            if (_visited[nbr.second])
              continue;

            positions.push_back(position(x.weight+nbr.weight, nbr.second));
          }
          
          sort(rall(positions));
        
          for (position& p: positions)
            stk.push(p);
        }
      }

      return -1; // peak not found
    }

  private:
    int _v, _e;
    vector<vector<neighbour>> _graph;
    vector<bool> _visited;
};
