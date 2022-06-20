//
//  main.cpp
//  Openstreet Maps II
//
//  Created by Keyur Patel on 4/13/22.
//

#include <iostream>
#include "graph.h"
#include "application.cpp"

int a(int argc, const char * argv[]) {
    
    graph<string, int> G;
    G.addVertex("A");
    G.addVertex("B");
    
    G.addVertex("C");
    G.addVertex("D");
    
    G.addEdge("A", "B", 100);
    G.addEdge("C", "D", 200);
    G.addEdge("A", "D", 300);
    G.addEdge("C", "C", 400);



    int n;
    G.getWeight("A", "B",  n);
    
    
    cout << n << endl;
//    for(auto a: n){
//        cout << a << endl;
//    }


    return 0;
}
