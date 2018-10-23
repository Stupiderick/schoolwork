/**
 * @file graph_tools.cpp
 * This is where you will implement several functions that operate on graphs.
 * Be sure to thoroughly read the comments above each function, as they give
 *  hints and instructions on how to solve the problems.
 */

#include "graph_tools.h"

/**
 * Finds the minimum edge weight in the Graph graph.
 * THIS FUNCTION IS GRADED.
 *
 * @param graph - the graph to search
 * @return the minimum weighted edge
 *
 * @todo Label the minimum edge as "MIN". It will appear blue when
 *  graph.savePNG() is called in minweight_test.
 *
 * @note You must do a traversal.
 * @note You may use the STL stack and queue.
 * @note You may assume the graph is connected.
 *
 * @hint Initially label vertices and edges as unvisited.
 */
int GraphTools::findMinWeight(Graph& graph)
{
    vector<Vertex> myVertices = graph.getVertices();
    vector<Edge> myEdges = graph.getEdges();
    queue<Vertex> myQueue;
    int minWeight = -1;
    Vertex minVertexSource;
    Vertex minVertexDest;

    /** Set all of the vertices as UNEXPLORED. */
    for (unsigned i = 0; i < myVertices.size(); i++) {
        graph.setVertexLabel(myVertices[i], "UNEXPLORED");
    }

    /** Set all of the edges as UNEXPLORED. */
    for (unsigned i = 0; i < myEdges.size(); i++) {
        graph.setEdgeLabel(myEdges[i].source, myEdges[i].dest, "UNEXPLORED");
    }

    myQueue.push(myVertices[0]);
    graph.setVertexLabel(myVertices[0], "VISITED");

    while (!myQueue.empty()) {
        Vertex parent = myQueue.front();
        vector<Vertex> adjacent = graph.getAdjacent(parent);
        myQueue.pop();

        for (unsigned i = 0; i < adjacent.size(); i++) {
            if (graph.getVertexLabel(adjacent[i]) == "UNEXPLORED") {
                graph.setVertexLabel(adjacent[i], "VISITED");
                graph.setEdgeLabel(parent, adjacent[i], "DISCOVERY");

                if (minWeight == -1 || graph.getEdgeWeight(parent, adjacent[i]) < minWeight) {
                    minWeight = graph.getEdgeWeight(parent, adjacent[i]);
                    minVertexSource = parent;
                    minVertexDest = adjacent[i];
                }
                myQueue.push(adjacent[i]);

            } else if (graph.getEdgeLabel(parent, adjacent[i]) == "UNEXPLORED") {
                graph.setEdgeLabel(parent, adjacent[i], "CROSS");

                if (minWeight == -1 || graph.getEdgeWeight(parent, adjacent[i]) < minWeight) {
                    minWeight = graph.getEdgeWeight(parent, adjacent[i]);
                    minVertexSource = parent;
                    minVertexDest = adjacent[i];
                }
            }
        }
    }

    graph.setEdgeLabel(minVertexSource, minVertexDest, "MIN");
    return minWeight;
}

/**
 * Returns the shortest distance (in edges) between the Vertices
 *  start and end.
 * THIS FUNCTION IS GRADED.
 *
 * @param graph - the graph to search
 * @param start - the vertex to start the search from
 * @param end - the vertex to find a path to
 * @return the minimum number of edges between start and end
 *
 * @todo Label each edge "MINPATH" if it is part of the minimum path
 *
 * @note Remember this is the shortest path in terms of edges,
 *  not edge weights.
 * @note Again, you may use the STL stack and queue.
 * @note You may also use the STL's unordered_map, but it is possible
 *  to solve this problem without it.
 *
 * @hint In order to draw (and correctly count) the edges between two
 *  vertices, you'll have to remember each vertex's parent somehow.
 */
int GraphTools::findShortestPath(Graph& graph, Vertex start, Vertex end)
{
    if (start == end) {
        return 0;
    }

    vector<Vertex> myVertices = graph.getVertices();
    vector<Edge> myEdges = graph.getEdges();
    queue<Vertex> myQueue;
    unordered_map<Vertex, Vertex> findParent;
    int minDistance = 1;

    for (unsigned i = 0; i < myVertices.size(); i++) {
        graph.setVertexLabel(myVertices[i], "UNEXPLORED");
    }

    for (unsigned i = 0; i < myEdges.size(); i++) {
        graph.setEdgeLabel(myEdges[i].source, myEdges[i].dest, "UNEXPLORED");
    }

    myQueue.push(start);
    graph.setVertexLabel(start, "VISITED");

    while (!myQueue.empty()) {
        Vertex parent = myQueue.front();
        vector<Vertex> adjacent = graph.getAdjacent(parent);
        myQueue.pop();

        for (unsigned i = 0; i < adjacent.size(); i++) {
            if (graph.getVertexLabel(adjacent[i]) == "UNEXPLORED") {
                graph.setVertexLabel(adjacent[i], "VISITED");
                graph.setEdgeLabel(parent, adjacent[i], "DISCOVERY");
                findParent.insert(make_pair(adjacent[i], parent));
                myQueue.push(adjacent[i]);
            } else if (graph.getEdgeLabel(parent, adjacent[i]) == "UNEXPLORED") {
                graph.setEdgeLabel(parent, adjacent[i], "CROSS");
            }
        }
    }

    Vertex finding = end;

    /** Add distance. */
    while (findParent[finding] != start) {
        graph.setEdgeLabel(findParent[finding], finding, "MINPATH");
        finding = findParent[finding];
        minDistance++;
    }

    graph.setEdgeLabel(findParent[finding], finding, "MINPATH");

    return minDistance;
}

/**
 * Finds a minimal spanning tree on a graph.
 * THIS FUNCTION IS GRADED.
 *
 * @param graph - the graph to find the MST of
 *
 * @todo Label the edges of a minimal spanning tree as "MST"
 *  in the graph. They will appear blue when graph.savePNG() is called.
 *
 * @note Use your disjoint sets class from MP 7.1 to help you with
 *  Kruskal's algorithm. Copy the files into the libdsets folder.
 * @note You may call std::sort instead of creating a priority queue.
 */
void GraphTools::findMST(Graph& graph)
{
    vector<Vertex> myVertices = graph.getVertices();
    vector<Edge> myEdges = graph.getEdges();
    DisjointSets dsetsVertices;

    int numOfVertices = int(myVertices.size());
    int count = 0;
    int i = 0;

    for (unsigned i = 0; i < myVertices.size(); i++) {
        graph.setVertexLabel(myVertices[i], "UNEXPLORED");
        dsetsVertices.addelements(myVertices[i]);
    }

    for (unsigned i = 0; i < myEdges.size(); i++) {
        graph.setEdgeLabel(myEdges[i].source, myEdges[i].dest, "UNEXPLORED");
    }

    /** Sort the vector from the smallest to the largest. */
    std::sort(myEdges.begin(), myEdges.end());

    while (count < numOfVertices - 1) {
        Edge currEdge = myEdges[i];
        Vertex currSource = currEdge.source;
        Vertex currDest = currEdge.dest;
        i++;

        if (dsetsVertices.find(currSource) != dsetsVertices.find(currDest)) {
            dsetsVertices.setunion(currSource, currDest);
            graph.setEdgeLabel(currSource, currDest, "MST");
            count++;
        }
    }
}
