// application.cpp <Starter Code>
// <Keyur Patel>
//
// This is the main application class. This handles all of the user
// inputs and preforms alorithms for the applications.
//
// University of Illinois at Chicago
// CS 251: Fall 2021
// Project #7 - Openstreet Maps
//
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"
#include "graph.h"
#include <queue>
#include <stack>

#include <set>

using namespace std;
using namespace tinyxml2;

const double INF = numeric_limits<double>::max();



class prioritize {
 public:
  bool operator()(const pair<long long, double>& p1,
                  const pair<long long, double>& p2) const {
    return p1.second > p2.second;
  }
};
// searchBuilding
//
// Helper function that searches for the building that the user has
// inputed. First seacch through the buuldings vector then compare
// to the query
BuildingInfo searchBuilding(string query,
                            vector<BuildingInfo>& Buildings, bool& found) {
    BuildingInfo Bi;
    found = false;
    for (auto a : Buildings) {
        if (a.Abbrev == query || a.Fullname == query) {
            Bi = a;
            found = true;
            break;
        } else if (a.Fullname.find(query) != string::npos) {
            // Checks if its a substring
            Bi = a;
            found = true;
            break;
        }
    }
    return Bi;
}


// nearestBuilding
//
// Helper funcion that finds the nearest building at
// midpoint. Does this by using the distBetween2Points
// function and finds the smllest distanec
BuildingInfo nearestBuilding(Coordinates midpoint,
                vector<BuildingInfo>& Buildings) {
    double min = INF;
    BuildingInfo Bi;

    for (auto a : Buildings) {
        double distance =  distBetween2Points(midpoint.Lat,
                           midpoint.Lon, a.Coords.Lat, a.Coords.Lon);
        // gets the distancce
        if (distance < min) {
            min = distance;
            Bi = a;
        }
    }
    return Bi;
}


// nearestNode
//
// Helper function to find the nearest node to the building.
// First searches through each footway and Node in foot way
// and finds the minimun distance to the nodes
long long nearestNode(BuildingInfo b, map<long long,
Coordinates>& Nodes, vector<FootwayInfo>& Footways) {
    double min = INF;
    long long minNode = 0;

    for (auto a : Footways) {
        for (auto x : a.Nodes) {
            long long curr = x;
            double distance = distBetween2Points(Nodes[curr].Lat,
                    Nodes[curr].Lon, b.Coords.Lat, b.Coords.Lon);
            // gets the distance
            if (distance < min) {
                min = distance;
                minNode = x;
            }
        }
    }
    return minNode;
}


// Runs Dijkstra algorithm
map<long long, long long> Dijkstra(graph<long long, double> G,
        long long startV, map<long long, double>& distances) {
    vector<long long>  visited;
    set <long long> found;
    map<long long, long long> predecessors;

    priority_queue<pair<long long, double>,
    vector<pair<long long, double>>, prioritize> unvisitedQueue;

    for (auto a : G.getVertices()) {
        distances[a] = INF;
        predecessors[a] = 0;
        unvisitedQueue.push(make_pair(a, INF));
    }
    distances[startV] = 0;
    unvisitedQueue.push(make_pair(startV, 0));

    while (!unvisitedQueue.empty()) {
        long long currentV = unvisitedQueue.top().first;
        unvisitedQueue.pop();

        if (distances[currentV] == INF) {
            break;
        } else if (found.count(currentV)) {
            continue;
        } else {
            visited.push_back(currentV);
            found.insert(currentV);

            set<long long> neighbors = G.neighbors(currentV);
            // Iterates through the all the neighbors
            for (auto adjV : neighbors) {
                double weight;
                G.getWeight(currentV, adjV, weight);

                double altWeight = distances[currentV] + weight;

                if (altWeight < distances[adjV]) {
                    distances[adjV] = altWeight;
                    predecessors[adjV] = currentV;
                    unvisitedQueue.push(make_pair(adjV, altWeight));
                }
            }
        }
    }
    return predecessors;
}


// getPath
//
// Helper function that returns the path to the destination.
// First pushes the current node into the stack then pops
// out andd insert into a vector so its in order
vector<long long> getPath(map<long long, long long>
                          predecessors, long long endV) {
    vector<long long> path;
    stack<long long> pathStack;

    long long currentV = endV;
    while (currentV != 0) {
        pathStack.push(currentV);
        currentV = predecessors[currentV];
    }
    while (!pathStack.empty()) {
        currentV = pathStack.top();
        pathStack.pop();
        path.push_back(currentV);
    }
    return path;
}


// displayPath
//
// Helper function to help display the paths. First gets the path vector and then
// displays each path
void displayPath(map<long long, long long> D, map<long long, long long> D2,
                 long long End, map<long long, double>
                 distances, map<long long, double> distances2) {
    vector<long long> path1 = getPath(D, End);
    vector<long long> path2 = getPath(D2, End);

    cout << "Person 1's distance to dest: " <<
    distances[End]  << " miles" << endl;

    cout << "Path: ";
    for (int i = 0; i < path1.size(); i++) {
        if (i < path1.size()-1) {
            cout << path1[i] << "->";
        } else {
            cout << path1[i] << "\n\n";
        }
    }
    cout << "Person 2's distance to dest: " <<
    distances2[End] << " miles" << endl;

    cout << "Path: ";
    for (int i = 0; i < path2.size(); i++) {
        if (i < path2.size()-1) {
            cout << path2[i] << "->";
        } else {
            cout << path2[i] << "\n\n";
        }
    }
}


// findNextNearest
//
// Helper function to help find the next nearest destination. It first finds
// the next closest building then runs Dijkstra again to get the new shortest
// path to the new destination and then displays the path.
void findNextNearest(set<string> found, BuildingInfo b,
Coordinates midpoint, vector<BuildingInfo>& Buildings,
map<long long, double> distances, map<long long, double> distances2,
long long End, map<long long, Coordinates>& Nodes, vector<FootwayInfo>& Footways,
graph<long long, double> G, long long Start1, long long Start2) {
    map<long long, long long> D1, D2;
    while (distances[End] >= INF || distances2[End] >=INF) {
        cout << "At least one person was unable to reach the destination ";
        cout << "building. Finding next closest building...\n\n";

        double min = INF;
        BuildingInfo Bi;

        for (auto a : Buildings) {
            double distance =  distBetween2Points(midpoint.Lat,
                    midpoint.Lon, a.Coords.Lat, a.Coords.Lon);
            if (distance < min && !found.count(a.Fullname)) {
                min = distance;
                Bi = a;
            }
        }
        cout << "New destination building:\n " << Bi.Fullname << "\n (" <<
        Bi.Coords.Lat <<", " << Bi.Coords.Lon << ")\n";

        End = nearestNode(Bi, Nodes, Footways);
        cout << "Nearest destination node:" << endl;
        cout << " " << End << endl;
        cout << " (" << Nodes[End].Lat << ", " << Nodes[End].Lon << ")" << "\n\n";

        D1 = Dijkstra(G, Start1, distances);
        D2 = Dijkstra(G, Start2, distances2);

        found.insert(Bi.Fullname);
    }
    displayPath(D1, D2, End, distances, distances2);
}


// Displays all the detials pertaining to the Peoples point destinations and nodes
void displayDetails(BuildingInfo B1, BuildingInfo B2, long long Start1,
                    long long Start2, long long End, map<long long,
                    Coordinates>& Nodes, BuildingInfo buildingCenter) {
    cout << "Person 1's point:" << endl;
    cout << " "  << B1.Fullname << endl;
    cout << " ("  << B1.Coords.Lat << ", "  << B1.Coords.Lon << ")"<< endl;

    cout << "Person 2's point:" << endl;
    cout << " "  << B2.Fullname << endl;
    cout << " ("  << B2.Coords.Lat << ", "  << B2.Coords.Lon << ")"<< endl;

    cout << "Destination Building:" << endl;
    cout << " "  << buildingCenter.Fullname << endl;
    cout << " ("  << buildingCenter.Coords.Lat << ", "
    << buildingCenter.Coords.Lon << ")"<< "\n\n";

    cout << "Nearest P1 node:" << endl;
    cout << " " << Start1 << endl;
    cout << " (" << Nodes[Start1].Lat << ", " << Nodes[Start1].Lon << ")" << endl;

    cout << "Nearest P2 node:" << endl;
    cout << " " << Start2 << endl;
    cout << " (" << Nodes[Start2].Lat << ", " << Nodes[Start2].Lon << ")" << endl;

    cout << "Nearest destination node:" << endl;
    cout << " " << End << endl;
    cout << " (" << Nodes[End].Lat << ", " << Nodes[End].Lon << ")" << "\n\n";
}


// Main function that handles all of the application details. Call other helper
// functions to reduce complexity.
void application(
    map<long long, Coordinates>& Nodes, vector<FootwayInfo>& Footways,
    vector<BuildingInfo>& Buildings, graph<long long, double> G) {
    string person1Building, person2Building; bool foundBuilding1, foundBuilding2;
    cout << endl;

    while (true) {
        cout << "Enter person 1's building (partial name or abbreviation), or #> ";
        getline(cin, person1Building);
        if (person1Building == "#") { break;}
        cout << "Enter person 2's building (partial name or abbreviation)> ";
        getline(cin, person2Building);

        BuildingInfo B1 = searchBuilding(person1Building, Buildings, foundBuilding1);
        BuildingInfo B2 = searchBuilding(person2Building, Buildings, foundBuilding2);

        if (!foundBuilding1) {
             cout << "Person 1's building not found" << endl;
             continue;
        }
        if (!foundBuilding2) {
            cout << "Person 2's building not found" << endl;
            continue;
        }
        Coordinates midpoint = centerBetween2Points(B1.Coords.Lat,
                    B1.Coords.Lon, B2.Coords.Lat, B2.Coords.Lon);
        BuildingInfo buildingCenter = nearestBuilding(midpoint, Buildings);

        long long Start1 = nearestNode(B1, Nodes, Footways);
        long long Start2 = nearestNode(B2, Nodes, Footways);
        long long End = nearestNode(buildingCenter, Nodes, Footways);

        map<long long, double> distances, distances2;
        map<long long, long long> D1 = Dijkstra(G, Start1, distances);
        map<long long, long long> D2 = Dijkstra(G, Start2, distances2);
        displayDetails(B1, B2, Start1, Start2, End, Nodes, buildingCenter);

        set<string> found;
        if (distances[Start2] >= INF) {
            cout << "Sorry, destination unreachable." << endl;
        } else if (distances[End] >= INF || distances2[End] >=INF) {
            found.insert(buildingCenter.Fullname);
            findNextNearest(found, buildingCenter, midpoint, Buildings,
            distances, distances, End, Nodes, Footways, G, Start1, Start2);
        } else {
            displayPath(D1, D2, End , distances, distances2);
        }
        cout << endl;
  }
}

int main() {
  // maps a Node ID to it's coordinates (lat, lon)
  map<long long, Coordinates>  Nodes;

  graph<long long, double> G;
  // info about each footway, in no particular order
  vector<FootwayInfo>          Footways;
  // info about each building, in no particular order
  vector<BuildingInfo>         Buildings;
  XMLDocument                  xmldoc;

  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);

  string def_filename = "map.osm";
  string filename;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "") {
    filename = def_filename;
  }

  //
  // Load XML-based map file
  //
  if (!LoadOpenStreetMap(filename, xmldoc)) {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }

  //
  // Read the nodes, which are the various known positions on the map:
  //
  int nodeCount = ReadMapNodes(xmldoc, Nodes);

  //
  // Read the footways, which are the walking paths:
  //
  int footwayCount = ReadFootways(xmldoc, Footways);

  //
  // Read the university buildings:
  //
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  //
  // Stats
  //
  assert(nodeCount == (int)Nodes.size());
  assert(footwayCount == (int)Footways.size());
  assert(buildingCount == (int)Buildings.size());

  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;

    for (auto a : Nodes) {
        G.addVertex(a.first);
    }

    for (auto a : Footways) {
        for (int i = 0; i < a.Nodes.size()-1; i++) {
            double lat1 = Nodes.at(a.Nodes[i]).Lat;
            double long1 = Nodes.at(a.Nodes[i]).Lon;
            double lat2 = Nodes.at(a.Nodes[i+1]).Lat;
            double long2 = Nodes.at(a.Nodes[i+1]).Lon;
            double distance = distBetween2Points(lat1, long1, lat2, long2);
            G.addEdge(a.Nodes[i], a.Nodes[i+1], distance);
            G.addEdge(a.Nodes[i+1], a.Nodes[i], distance);
        }
    }
  cout << "# of vertices: " << G.NumVertices() << endl;
  cout << "# of edges: " << G.NumEdges() << endl;
  cout << endl;

  string userInput;
  cout << "Enter \"a\" for the standard application or "
        << "\"c\" for the creative component application> ";
  getline(cin, userInput);
  if (userInput == "a") {
    // TO DO: add argument for the graph you make.
    application(Nodes, Footways, Buildings, G);
  } else if (userInput == "c") {
    // TO DO: add arguments
    // creative();
  }

  cout << "** Done **" << endl;
  return 0;
}
