#include <bits/stdc++.h>

using namespace std;

namespace tsp {
  class city {
    public:
      city() {}
      city(string name, int id): name(name), id(id) {}

    private:
      string name;
      int    id;
  };

  class road {
    public:
      road() {}
      road(int city1, int city2, int distance) 
        : city1(city1), city2(city2), distance(distance)
        {}

    private:
      int city1, int city2, int distance;
  };

  class network {
    public:
      network() {}
      network(vector<city>& cities, vector<road>& roads)
        : _cities(cities), _roads(road)
        {
          _network(cities.size(), vector<city>(0));
          for (road r in roads) {
            _network[road.city1].push_back(r);
            _network[road.city2].push_back(r);
          }
        }

      int distance(int city1, int city2) {
        for (road r: _roads)
          if (r.city1==city1 && r.city2==city2 || r.city2==city1 && r.city1==city2)
            return r.distance;

        return 0;
      }

/*       int solve_iterative(int starting_city) {
        int answer = INT_MAX;

        vector<bool> visited(_cities.size(), false);
        visited[starting_city] = true;

        stack<pair<int, int>> currently_visited_cities; // (city, distance)
        currently_visited_cities.push({starting_city, 0});

        while (current_cities.empty() == false) {
          auto last_visited_city = currently_visited_cities.top();

          for (city c: _network[last_visited_city.first]) {
            if (c.id == starting_city && currently_visited_cities.size()==_cities.size()) {
              answer = min(answer, last_visited_city.second + distance(last_visited_city.first, starting_city));
              currently_visited_cities.pop();
              continue;
            }

            if (visited[c.id] == false) {
              visited[c.id] = true;
              currently_visited_cities.push({c.id, last_visited_city.first+distance(c.id, last_visited_city.first)});
              break;
            }

            currently_visited_cities.pop();
          }
        }

        return answer;
      } */

      int solve_recursive(int starting_city, int last_visited_city, int distance, vector<bool> visited) {
        visited[last_visited_city] = true;
        
        int answer = INT_MAX;

        for (city c: _network[last_visited_city]) {
          if (starting_city == c.id) {
            return distance + distance(last_visited_city, starting_city);
          }

          if (visited[last_visited_city] == false) {
            answer = min(answer, solve_recursive(c.id, distance + distance(c.id, last_visited_city), visited));
          }
        }

        return answer;
      }

    private:
      vector<vector<cities>> _network;
      vector<city> _cities;
      vector<road> _roads;
  };
};
