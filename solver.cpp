#include "solver.h"

Solver::Solver():g(81), woawSound(), naniSound(){
    vertex_iter vIt, vItEnd, rowIt;

    for(boost::tie(vIt, vItEnd) = vertices(g); vIt != vItEnd; ++vIt){
        int columnIndex = (get(vertex_index, g, *vIt)) % 9;
        int rowIndex = (get(vertex_index, g, *vIt)) / 9;
        int i = columnIndex;
        int j = rowIndex;

        if(columnIndex == 0) rowIt = vIt;

        for(vertex_iter wIt = rowIt; wIt != vIt; ++wIt){
            add_edge(*wIt, *vIt, g);
        }

        for(int k = 1; k <= rowIndex; ++k){
            add_edge(*(vIt - k * 9), *vIt, g);
        }
        vertex_iter beginGroupIt = vIt - i % 3 - 9 * (j % 3);
        vertex_iter wIt = beginGroupIt;

        for(int l = 0; l < 3; ++l){
            if(j % 3==l) continue;
            wIt = beginGroupIt + 9 * l;
            for(int k = 0; k < i % 3; ++k){
                add_edge(*wIt, *vIt, g);
                ++wIt;
            }
        }
    }
}

void Solver::loadSoundEffects(){
    woawSound.setSource(QUrl::fromLocalFile(":/woaw.wav"));
    woawSound.setLoopCount(1);
    woawSound.setVolume(0.5f);

    naniSound.setSource(QUrl::fromLocalFile(":/nani.wav"));
    naniSound.setLoopCount(1);
    naniSound.setVolume(0.5f);
}

bool Solver::isGridFeasible(const std::array<int, 81>& grid){
    std::array<int, 81> grid_transpose;
    std::array<int, 9> block;

    for(int i = 0; i < 9; ++i){
        for(int j = 0; j < 9; ++j){
            //std::cout << "grid[" << i << "][" << j << "] = " << grid[9 * i + j] << std::endl;
            grid_transpose[9 * j + i] = grid[9 * i + j];
            //std::cout << "New block:" << std::endl;
            for(int k = 0; k < 3; ++k){
                for(int l = 0; l < 3; ++l){
                    block[3 * k + l] = grid[9 * (i - i % 3 + k) + j - j % 3 + l];
                }
            }
            if(i == 0){
                for(int l = 0; l < 9; ++l){
                    grid_transpose[9 * j + l] = grid[9 * l + j];
                }
            }

            if(grid[9 * i + j] == 0)
                continue;

            if(std::find(grid.cbegin() + 9 * i + j + 1, grid.cbegin() + 9 * (i + 1), grid[9 * i + j]) != (grid.cbegin() + 9 * (i + 1))){
                //std::cout << "duplicate element in row." << std::endl;
                return false;
            }

            if(std::find(grid_transpose.cbegin() + 9 * j + i + 1, grid_transpose.cbegin() + 9 * (j + 1), grid_transpose[9 * j + i]) != (grid_transpose.cbegin() + 9 * (j + 1))){
                //std::cout << "duplicate element in column." << std::endl;
                return false;
            }

            if(std::find(block.cbegin() + 3 * (i % 3) + (j % 3) + 1, block.cend(), grid[9 * i + j]) != block.cend()){
                //std::cout << "duplicate element in block." << std::endl;
                return false;
            }
        }
    }
    return true;
}

std::vector<Vertex> Solver::gridToGraph(const std::array<int, 81>& grid){
    std::vector<Vertex> uncolored_vertices;
    vertex_iter vIt = vertices(g).first;

    for(int i = 0; i < 81; ++i){
        //std::cout << grid[i] << std::endl;
        put(vertex_color, g, *vIt, grid[i]);
        if(!grid[i]){
            uncolored_vertices.push_back(*vIt);
        }
        ++vIt;
    }
    //std::cout << "-------------" << std::endl;
    return uncolored_vertices;
}

bool Solver::backtrack_coloring(const std::vector<Vertex>& vertices_vec, const std::vector<Vertex>::const_iterator current_vertexIt, const int& num_colors, int& iterations_count){
    if(current_vertexIt == vertices_vec.end()) return true;
    else{
        //int run = 0;
        std::list<int> available_colors;
        for(int i = 1; i <= num_colors; ++i) available_colors.push_back(i);

        while(1){
            //++run;
            ++iterations_count;
            if(get(vertex_color, g, *current_vertexIt)){ // backtracked
                //std::cout << "Backtracked to vertex " << *current_vertexIt << " run = " << run << std::endl;
                auto colorIt = std::find(available_colors.begin(), available_colors.end(), get(vertex_color, g, *current_vertexIt));
                if(colorIt != available_colors.end())
                    available_colors.erase(colorIt);
                put(vertex_color, g, *current_vertexIt, 0);
            }
            //else
                //std::cout << "Moved to vertex " << *current_vertexIt << " run = " << run << std::endl;

            adjacency_iter neighborsIt, neighborsItEnd; // Get color of neighbors
            for(boost::tie(neighborsIt, neighborsItEnd) = adjacent_vertices(*current_vertexIt, g); neighborsIt != neighborsItEnd; ++neighborsIt){
                if(get(vertex_color, g, *neighborsIt)){
                    auto colorIt = std::find(available_colors.begin(), available_colors.end(), get(vertex_color, g, *neighborsIt));
                    if(colorIt != available_colors.end())
                        available_colors.erase(colorIt);
                }
            }
            if(!available_colors.empty()){
                put(vertex_color, g, *current_vertexIt, available_colors.front());
                //std::cout << "Color of vertex " << *current_vertexIt << " set to " << get(vertex_color, g, *current_vertexIt) << std::endl;
                if(backtrack_coloring(vertices_vec, current_vertexIt + 1, num_colors, iterations_count))
                    return true;
            }
            else{
               //std::cout << "Unable to color vertex " << *current_vertexIt << std::endl;
               return false;
            }
        }
    }
}

bool Solver::solveFromTable(QTableWidget* table, int& iterations_count){
    std::array<int, 81> grid;
    for(int i = 0; i < 81; ++i){
        grid[i] = table->item(i / 9, i % 9)->text().toInt();
        if(grid[i] < 1 || grid[i] > 9){
            grid[i] = 0;
            table->item(i / 9, i % 9)->setText("");
        }
    }

    if(isGridFeasible(grid)){
        std::vector<Vertex> vertices_vec{gridToGraph(grid)};
        if(backtrack_coloring(vertices_vec, vertices_vec.begin(), 9, iterations_count)){
            for(std::pair<vertex_iter, vertex_iter> vp = vertices(g); vp.first != vp.second; ++vp.first){
                int index = get(vertex_index, g, *vp.first);
                if(get(vertex_color, g, *vp.first))
                    table->item(index / 9, index % 9)->setText(QString("%1").arg(get(vertex_color, g, *vp.first)));
                else
                    table->item(index / 9, index % 9)->setText("");
            }
            return true;
        }
        return false;
    }
    return false;
}

void Solver::playWoawSound(){
    if(woawSound.status() == QSoundEffect::Ready)
        woawSound.play();
}
void Solver::playNaniSound(){
    if(naniSound.status() == QSoundEffect::Ready)
        naniSound.play();
}
