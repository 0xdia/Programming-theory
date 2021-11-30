#include <bits/stdc++.h>
#include "HillClimbingSolver.hpp"

using namespace std;

int main() {
  freopen("input", "r", stdin);

  int v, e, root, peak;
  vector<edge> edges;

  cin >> v >> e;
  for (int i=0; i<e; i++) {
    edge edg;
    cin >> edg.weight >> edg.v1 >> edg.v2;
    edges.push_back(edg);
  }

  cin >> root >> peak;

  HillClimbingSolver s = HillClimbingSolver(v, e, edges);

  int nearly_optimal_cost = s.climb(root, peak);

  cout << "Nearly optimal solution: " << nearly_optimal_cost << endl;

  return 0;
}
