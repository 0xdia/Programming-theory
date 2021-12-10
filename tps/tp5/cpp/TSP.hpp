#include <bits/stdc++.h>

using namespace std;


namespace tsp {
  class city {
    public:
      city() {}
      city(string name, int id): _name(name), _id(id) {}

    private:
      string _name;
      int    _id;
  };

  class road {
    public:
      road() {}
      road(int city1, int city2, int distance) 
        : _city1(city1), _city2(city2), _distance(distance)
        {}

    private:
      int _city1, int _city2, int _distance;
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

    private:
      vector<vector<cities>> _network;
      vector<city> _cities;
      vector<road> _roads;
  };
};
