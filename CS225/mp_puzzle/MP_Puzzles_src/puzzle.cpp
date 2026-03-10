/**
 * @file puzzle.cpp
 * Implementation of puzzle class.
 */
#include "puzzle.h"
#include <set>
#include<algorithm>
#include <map>
 PuzzleState::PuzzleState() {
    for (int i = 0; i < height_ ; i++) {
        for(int j = 0 ; j < width_ ; j++) {
            char length_ = i*width_ + j + 1;
            if( i*width_ + j + 1 != 16) {
            puzzle_state_[i][j] = length_;
            } else if( i*width_ + j + 1 == 16) {
            puzzle_state_[i][j] = 0;
            }
        }
    }

 }

 PuzzleState::PuzzleState(const std::array<char, 16> state) {
    for (int i = 0; i < height_ ; i++) {
        for(int j = 0 ; j < width_ ; j++) {
            int length_ = i*width_ + j;
            //if(length_ != 16) {
            puzzle_state_[i][j] = state[length_];
            // } else if(length_ == 16) {
            // puzzle_state_[i][j] = '0';
            // }
        }
    }
 }

std::array<char, 16> PuzzleState ::asArray() const {
    std::array<char, 16> puzzle_state_as_array{};
     for (int i = 0; i < height_ ; i++) {
        for(int j = 0 ; j < width_ ; j++) {
            int length_ = i*width_ + j;
            puzzle_state_as_array[length_] = puzzle_state_[i][j];
        }
    }
    return puzzle_state_as_array;
}

 bool PuzzleState::operator==(const PuzzleState &rhs) const {
    for(int i = 0 ; i < width_ ; i++) {
        for(int j = 0; j < height_; j++) {
            if( rhs.puzzle_state_[i][j] != puzzle_state_[i][j]){
                return false;
            }
        }
    }
    return true; 
 }
 bool PuzzleState::operator!=(const PuzzleState &rhs) const {

    return !(*this == rhs);
 }

  bool PuzzleState:: operator<(const PuzzleState &rhs) const {
    for(int i = 0; i < height_ ; i++) {
        for(int j = 0; j < width_;j++){
            if(puzzle_state_[i][j] < rhs.puzzle_state_[i][j]) {
                return true;
            }
            if(puzzle_state_[i][j] > rhs.puzzle_state_[i][j]) {
                return false;
            }
        }
    }
    return false;  
  }
  PuzzleState PuzzleState:: getNeighbor(Direction direction) const {

    int empty_row = -1, empty_col = -1;
    for (int i = 0; i < height_; ++i) {
        for (int j = 0; j < width_; ++j) {
            if (puzzle_state_[i][j] == 0) {
                empty_row = i;
                empty_col = j;
                break;
            }
        }
        if (empty_row != -1) break;
    }

    // Define movement deltas for each direction
    int dx = 0, dy = 0;
    switch (direction) {
        case Direction::UP:
            dx = 1; dy = 0;
            break;
        case Direction::DOWN:
            dx = -1; dy = 0;
            break;
        case Direction::LEFT:
            dx = 0; dy = 1;
            break;
        case Direction::RIGHT:
            dx = 0; dy = -1;
            break;
    }

    // Calculate the new position of the empty tile
    int new_row = empty_row + dx;
    int new_col = empty_col + dy;

    // Check if the new position is within bounds
    if (new_row >= 0 && new_row < height_ && new_col >= 0 && new_col < width_) {
        // Create a copy of the current state
        PuzzleState new_state = *this;

        // Swap the empty tile with the adjacent tile in the new position
        std::swap(new_state.puzzle_state_[empty_row][empty_col],
                  new_state.puzzle_state_[new_row][new_col]);

        return new_state; // Return the valid new state
    } else {
        // create a zero state 
        PuzzleState zero_state; 
        for (int i = 0; i < height_ ; i++) {
            for(int j = 0 ; j < width_ ; j++) {
            char state = 0;
            zero_state.puzzle_state_[i][j] = state;
        }
    }
     return zero_state;   
    }
  }

  std::vector<PuzzleState> PuzzleState::getNeighbors() const {
    int empty_row = -1, empty_col = -1;
    for (int i = 0; i < height_; ++i) {
        for (int j = 0; j < width_; ++j) {
            if (puzzle_state_[i][j] == 0) {
                empty_row = i;
                empty_col = j;
                break;
            }
        }
        if (empty_row != -1) break;
    }

    std::vector<PuzzleState> all_neighbours; 
    
     // Directions: Up, Down, Left, Right
    const std::vector<std::pair<int, int>> directions = {
        // {-1, 0}, {1, 0}, {0, -1}, {0, 1}
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    for (const auto& [dx, dy] : directions) {
        int new_row = empty_row + dx;
        int new_col = empty_col + dy;

        // Check if the new position is valid
        if (new_row >= 0 && new_row < height_ && new_col >= 0 && new_col < width_) {
            // Create a new neighbor state by swapping tiles
            PuzzleState neighbor = *this;
            std::swap(neighbor.puzzle_state_[empty_row][empty_col], 
                      neighbor.puzzle_state_[new_row][new_col]);
            all_neighbours.push_back(neighbor);
        }
    }
    return all_neighbours;
  }

int PuzzleState::manhattanDistance(const PuzzleState desiredState) const {
    int manhattan_distance = 0;
    
    std::vector<int> current_rows(16);
    std::vector<int> current_cols(16);
    std::vector<int> desired_rows(16);
    std::vector<int> desired_cols(16);
    
    // Find positions
    for (int i = 0; i < height_; ++i) {
        for (int j = 0; j < width_; ++j) {
            char elem = puzzle_state_[i][j];
            if (elem != 0) {
                current_rows[elem] = i;
                current_cols[elem] = j;
            }
            
            char desired_elem = desiredState.puzzle_state_[i][j];
            if (desired_elem != 0) {
                desired_rows[desired_elem] = i;
                desired_cols[desired_elem] = j;
            }
        }
    }
    
    for (int elem = 1; elem < 16; ++elem) {
        manhattan_distance += std::abs(current_rows[elem] - desired_rows[elem]) + 
                               std::abs(current_cols[elem] - desired_cols[elem]);
    }
    
    return manhattan_distance;
}


std::vector<PuzzleState> solveBFS(const PuzzleState &startState, const PuzzleState &desiredState, size_t *iterations) {
    if (iterations != nullptr) {
        *iterations = 0;
    }

    std::queue<PuzzleState> path_queue;
    std::map<PuzzleState, PuzzleState> parent; // Tracks the parent of each state
    //std::set<PuzzleState> visited; // Tracks visited states

    // Push the start state into the queue and mark it as visited
    path_queue.push(startState);
    //visited.insert(startState);

    // Initialize the parent for the start state
    parent[startState] = startState;
    //int count = 0;

    // Perform BFS
    while (!path_queue.empty()) {
        PuzzleState current_state = path_queue.front();
        path_queue.pop();

        // Increment iterations count
        if (iterations != nullptr) {
            (*iterations)++;
        }
        
        // Check if the current state is the desired state
        if (current_state == desiredState) {
            // Construct the path from startState to desiredState
            std::vector<PuzzleState> final_path;
            while (current_state != startState) {
                final_path.push_back(current_state);
                current_state = parent[current_state];
            }
            final_path.push_back(startState);
            std::reverse(final_path.begin(), final_path.end());
            return final_path;
        }

        // Process neighbors of the current state
        std::vector<PuzzleState> neighbors = current_state.getNeighbors();
        for (PuzzleState &neighbor : neighbors) {
            if (parent.find(neighbor) == parent.end()) { // Check if not visited
                //visited.insert(neighbor); // Mark as visited immediately
                parent[neighbor] = current_state; // Set the parent of the neighbor
                path_queue.push(neighbor); // Add the neighbor to the queue
            }
        }
    }

    // If no solution is found, return an empty vector
    //std::cout << count << std::endl;
    return {};
}


std::vector<PuzzleState> reconstruct_path(std::map<PuzzleState, PuzzleState>& cameFrom, PuzzleState current, const PuzzleState& startState) {
    std::vector<PuzzleState> final_path ;
            while (current != startState) {
                final_path.push_back(current);
                current = cameFrom[current];
            }
            final_path.push_back(startState);
            std::reverse(final_path.begin(), final_path.end());
            return final_path;
}

std::vector<PuzzleState> solveAstar(const PuzzleState& startState, const PuzzleState& desiredState, size_t* iterations) {
    // Safely handle iterations pointer
     if(iterations!=NULL){
        *iterations = 0;
    }
    // size_t local_iterations = 0;
    // size_t* iter_ptr = iterations ? iterations : &local_iterations;
     auto compare = [](const std::pair<double,PuzzleState>& x, const std::pair<double,PuzzleState>& y) {
        return x.first > y.first;
    };

    // Priority queue for open set (min-heap based on f_score)
    //using queue_element = std::pair<double, PuzzleState>;
    std::priority_queue<std::pair<double, PuzzleState>, std::vector<std::pair<double, PuzzleState>>, decltype(compare)> openSet(compare);

    // Map to reconstruct the path
    std::map<PuzzleState, PuzzleState> cameFrom;

    // Cost maps
    std::map<PuzzleState, double> g_score; // Cost from start to current
    std::map<PuzzleState, double> f_score; // Estimated total cost (g + h)

    // Initialize start state
    g_score[startState] = 0.0;
    f_score[startState] = startState.manhattanDistance(desiredState);
    openSet.push(std::make_pair(f_score[startState], startState));

    // Visited set to avoid revisiting nodes
    //std::set<PuzzleState> visited;

    while (!openSet.empty()) {
        // Get the node with the lowest f_score
        PuzzleState current = openSet.top().second;
        openSet.pop();

        // Increment iterations 
        if (iterations != nullptr) {
            (*iterations)++;
        }

        // If the desired state is reached, reconstruct the path
        if (current == desiredState) {
            return reconstruct_path(cameFrom, current, startState);
        }

        // If already visited, skip
        // if (visited.count(current)) {
        //     continue;
        // }
        // visited.insert(current);

        // Process neighbors
        for (const auto& neighbor : current.getNeighbors()) {
            // if (visited.count(neighbor)) {
            //     continue; // Skip visited nodes
            // }

            double tentative_gScore = g_score[current] + 1; // Assume uniform cost (1) for each move

            // If this path to the neighbor is better
            if (!g_score.count(neighbor) || tentative_gScore < g_score[neighbor]) {
                cameFrom[neighbor] = current;
                g_score[neighbor] = tentative_gScore;
                f_score[neighbor] = tentative_gScore + neighbor.manhattanDistance(desiredState);

                // Add neighbor to the open set
                openSet.push(std::make_pair(f_score[neighbor], neighbor));
            }
        }
    }

    // If no solution is found, return an empty vector
    return {};
}