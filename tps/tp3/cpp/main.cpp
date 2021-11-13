#include <bits/stdc++.h>
#include "ArticulationPointsFinder.hpp"

using namespace std;

int main() {
  freopen("input", "r", stdin);

  int v, e;
  cin >> v;
  cin >> e;

  vector<pair<int, int>> edges(e);

  for(int i=0; i<e; i++)
    cin >> edges[i].first >> edges[i].second;

  ArticulationPointsFinder finder = ArticulationPointsFinder(v, e, edges);

  set<int> s = finder.find_articulation_points();
  
  for(auto& u: s)
    cout << u << ' ';
  
  cout << endl;

  return 0;
}
