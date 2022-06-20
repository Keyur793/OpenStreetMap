// graph.h <Starter Code>
// < Keyur Patel>
//
// Implemented with adjacency list representation. In this cless the userr
// can add vertex/edge and call various other function that returns detials
// of the graph class
//
// University of Illinois at Chicago
// CS 251: Fall 2021
// Project #7 - Openstreet Maps
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <unordered_map>

using namespace std;

template<typename VertexT, typename WeightT>
class graph {
 private:
    typedef unordered_map<VertexT, WeightT> vwMap;
    set<VertexT> vertices;
    unordered_map<VertexT, vwMap> adjList;

 public:
  graph() {
  }

  //
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  int NumVertices() const {
      return vertices.size();
  }

  //
  // NumEdges
  //
  // Returns the # of edges currently in the graph.
  //
  int NumEdges() const {
      int count = 0;

      for (auto a : adjList) {
          for (auto b : a.second) {
              count++;
          }
      }
      return count;
  }
    //
    // clear
    //
    // clears the graph
    //
  void clear() {
      vertices.clear();
      adjList.clear();
  }

  //
  // addVertex
  //
  // Adds the vertex v to the graph if there's room, and if so
  // returns true.  If the graph is full, or the vertex already
  // exists in the graph, then false is returned.
  //
  bool addVertex(VertexT v) {
      if (!vertices.count(v)) {
          vertices.insert(v);
          return true;
      }
      return false;
  }

  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true.  If the vertices do not exist or for some reason the
  // graph is full, false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.
  //
  bool addEdge(VertexT from, VertexT to, WeightT weight) {
      if (!vertices.count(from) || !vertices.count(to)) {
          return false;
      }
      adjList[from][to] = weight;
      return true;
  }

  //
  // getWeight
  //
  // Returns the weight associated with a given edge.  If
  // the edge exists, the weight is returned via the reference
  // parameter and true is returned.  If the edge does not
  // exist, the weight parameter is unchanged and false is
  // returned.
  //
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const {
          if (adjList.count(from)) {
              if (adjList.at(from).count(to)) {
                  weight = adjList.at(from).at(to);
                  return true;
              }
          }
      return false;
  }

  //
  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.
  //
  set<VertexT> neighbors(VertexT v) const {
      set<VertexT> neighbors;
      if (adjList.count(v)) {
          for (auto b : adjList.at(v)) {
              neighbors.insert(b.first);
          }
      }
      return neighbors;
  }

  //
  // getVertices
  //
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
  vector<VertexT> getVertices() const {
      vector<VertexT> allvertices;
      for (auto a : vertices) {
          allvertices.push_back(a);
      }
      return allvertices;
  }

  //
  // dump
  //
  // Dumps the internal state of the graph for debugging purposes.
  //
  // Example:
  //    graph<string,int>  G(26);
  //    ...
  //    G.dump(cout);  // dump to console
  //
  void dump(ostream& output) const {
  }
};
