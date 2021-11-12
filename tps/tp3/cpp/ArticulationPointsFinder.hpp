#include <bits/stdc++.h>

using namespace std;

#define all(x) x.begin(), x.end()
#define belongs(Set, x) Set.find(x) != Set.end() 

class ArticulationPointsFinder {
  public:
    ArticulationPointsFinder(int v, int e, vector<pair<int,int>> edges)
      : _v(v), _e(e), _edges(edges) 
      {
        _sets = vector<set<int>>(v);
        _included.assign(v, false);
        _graph = vector<vector<int>>(v);
        
        for (int i=0; i<_e; i++) {
          _graph[_edges[i].first].push_back(edges[i].second);
          _graph[_edges[i].second].push_back(edges[i].first);
        }
      }
    
    void find_connected_components() {
      if (_sets_ready)
        return;

      for (int i=0; i<_v; i++) {
        if (_included[i]) 
          continue;

        _visited.assign(_v, false);
        _search_set(i, vector<int>(1, i));
        for (int u: _sets[i])
          _included[u] = true;
      }

      _sets_ready = true;
    }

    set<int> find_articulation_points() {
      set<int> result;
    
      this->find_connected_components(); 

      for (auto& edge: _edges) {
        bool cutting = true;
        for (auto& set: _sets) {
          if (belongs(set, edge.first) && belongs(set, edge.second)) {
            cutting = false;
            break;
          }
        }

        if (cutting) {
          result.insert(edge.first);
          result.insert(edge.second);
        }
      }
  
      return result;
    }

    vector<vector<int>> graph_structure() {
      return _graph;
    }

  private:
    void _search_set(int& cause, vector<int> current_path) {
      int current = current_path.back();
      _visited[current] = true; 
      for (int& u: _graph[current]) {
        if (u == cause)
          _sets[cause].insert(all(current_path));
        else if (! _visited[u] && ! _included[u]) {
          current_path.push_back(u);
          _search_set(cause, current_path);
          current_path.pop_back();
        }
      }
    }
    
    const int _v, _e;

    bool _sets_ready = false;

    vector<vector<int>> _graph;
    vector<pair<int, int>> _edges;
    vector<set<int>> _sets;
    vector<bool> _visited, _included;
};
