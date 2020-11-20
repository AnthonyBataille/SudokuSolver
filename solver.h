#ifndef SOLVER_H
#define SOLVER_H

#include <QSoundEffect>
//#include <QMainWindow>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <iostream>
#include <utility>
#include <vector>
#include <list>
#include <iterator>
#include <algorithm>

#include "ui_mainwindow.h"

using namespace boost;

typedef property<vertex_color_t, int> vertex_col;
typedef adjacency_list<vecS, vecS, undirectedS, vertex_col> Graph;
//typedef property_map<Graph, vertex_index_t>::type vertex_index_map_t;
//typedef property_map<Graph, vertex_color_t>::type vertex_color_map_t;

typedef graph_traits<Graph>::vertex_descriptor Vertex;
typedef graph_traits<Graph>::vertex_iterator vertex_iter;

typedef graph_traits<Graph>::adjacency_iterator adjacency_iter;

typedef std::shared_ptr<Graph> graph_ptr;



class Solver{
    public:
        Solver();
        bool solveFromTable(QTableWidget* table, int& iterations_count);
        void playWoawSound();
        void playNaniSound();
        void loadSoundEffects();
    private:
        Graph g;
        QSoundEffect woawSound;
        QSoundEffect naniSound;

        std::vector<Vertex> gridToGraph(const std::array<int, 81>& grid);
        bool isGridFeasible(const std::array<int, 81>& grid);
        bool backtrack_coloring(const std::vector<Vertex>& vertices_vec, const std::vector<Vertex>::const_iterator current_vertexIt, const int& num_colors, int& iterations_count);
};

#endif // SOLVER_H
