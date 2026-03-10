/* Your code here! */
#include "maze.h"
#include <random>
#include <iostream>
#include <queue>
#include <vector>
#include "iterator"
#include <algorithm>
using namespace cs225;
using namespace std;
SquareMaze::SquareMaze(){
    maze_height_ = 0;
    maze_width_ = 0;
    maze_size_ = 0;
}

void SquareMaze::makeMaze(int width, int height) {
    // making a new maze of given size 
    maze_height_ = height; 
    maze_width_ = width;
    maze_size_ = height*width;

    // clear all previous walls and iterations 
    maze_walls.clear();

    // now we create our set of the maze size 
    corridor_.addelements(maze_size_);
    
    //resize wall vector width
    maze_walls.resize(maze_width_);

    // resize height 
    for(int i = 0; i < maze_width_; i++) {
        maze_walls[i].resize(height);
    }
    //now update all valuues in wall to be true as we make a maze of complete walls and then randomly delete it
    for(int i = 0 ; i < maze_width_; i++){
        for(int j = 0; j < maze_height_; j++) {
            // first value signifies the right direction 
            maze_walls[i][j].first = true; 
            // second value signifies the down direction
            maze_walls[i][j].second = true;
        }
    }
    

    // randomly select walls to delete  depending on direction 
    while (corridor_.size(0) < maze_size_){
        int direction = rand() % 2;
        int x_coord = rand() % maze_width_;
        int y_coord = rand() % maze_height_;
        // x+ y*width we use this to navigate the disjoint set while 
        int curr_rand = x_coord + y_coord*maze_width_;
    // boundary condition
    if (x_coord == maze_width_ - 1 && y_coord == maze_height_ - 1) {
        continue;
    }
        // depending on whether we want to go right or down we choose 
    if(x_coord < maze_width_ -1  && y_coord < maze_height_ - 1) {
       // if(direction == 0) {
            //we check if wall is not on boundary and is a wall 
            if(x_coord < maze_width_ - 1 && maze_walls[x_coord][y_coord].first) {
                // now we check if they belong in the same set of corridors or no , to prevent cycles
                if(corridor_.find(curr_rand) != corridor_.find(curr_rand + 1)) {
                // since we are not creating cycles nad in boundary we can delete the wall 
                maze_walls[x_coord][y_coord].first = false;
                // now we set the corridor 
                corridor_.setunion(curr_rand, curr_rand + 1);
                }
            }
       // } else {
            if(y_coord < maze_height_ - 1 && maze_walls[x_coord][y_coord].second) {
                            // now we check if they belong in the same set of corridors or no , to prevent cycles
                            if(corridor_.find(curr_rand) != corridor_.find(curr_rand + maze_width_)) {
                            // since we are not creating cycles nad in boundary we can delete the wall 
                            maze_walls[x_coord][y_coord].second = false;
                            // now we set the corridor 
                            corridor_.setunion(curr_rand, curr_rand + maze_width_);
                            }
            }
       // }
    }  else if (x_coord == maze_width_ -1 && y_coord < maze_height_ - 1) {
        // now we check if they belong in the same set of corridors or no , to prevent cycles
        if(corridor_.find(curr_rand) != corridor_.find(curr_rand + maze_width_)) {
        // since we are not creating cycles nad in boundary we can delete the wall 
        maze_walls[x_coord][y_coord].second = false;
        // now we set the corridor 
        corridor_.setunion(curr_rand, curr_rand + maze_width_);
        }
    } else if (x_coord < maze_width_ -1 && y_coord == maze_height_ -1) {
        // now we check if they belong in the same set of corridors or no , to prevent cycles
                if(corridor_.find(curr_rand) != corridor_.find(curr_rand + 1)) {
                // since we are not creating cycles nad in boundary we can delete the wall 
                maze_walls[x_coord][y_coord].first = false;
                // now we set the corridor 
                corridor_.setunion(curr_rand, curr_rand + 1);
                }
    }
    }
}
bool SquareMaze:: canTravel(int x, int y, Direction dir) const {
    // we first check which direction we are trying to step into 
    if (dir == RIGHT ){
        // now we check if it is within bounds and is not a wall on right 
        if(x < maze_width_ -1 && !maze_walls[x][y].first) {
            return true;
        }

    } else if ( dir == DOWN) {
        // now we check if it is within bounds and is not a wall down
        if(y < maze_height_-1 && !maze_walls[x][y].second) {
            return true;
        }

    } else if (dir == LEFT) {
        // now we check if it is within bounds and is not a wall on left
        if(x > 0  && !maze_walls[x-1][y].first) {
            return true;
        }

    } else if (dir == UP) {
        // now we check if it is within bounds and is not a wall on top
        if(y > 0 && !maze_walls[x][y-1].second) {
            return true;
        }

    }
    return false;
}

void SquareMaze:: setWall(int x, int y, Direction dir, bool exists) {
    if (dir == RIGHT ){
        // now we check if it is within bounds and set wall
        if(x < maze_width_ -1 ) {
            maze_walls[x][y].first = exists;
        }

    } else if ( dir == DOWN) {
        // now we check if it is within bounds and set wall
        if(y < maze_height_ - 1 ) {
            maze_walls[x][y].second = exists;
        }

    } else if (dir == LEFT) {
        // now we check if it is within bounds and set wall
        if(x > 0  ) {
            maze_walls[x-1][y].first = exists;
        }

    } else if (dir == UP) {
        // now we check if it is within bounds and set wall
        if(y > 0 ) {
            maze_walls[x][y-1].second= exists;
        }

    }
}

std::vector<Direction> SquareMaze::solveMaze(int startX) {
    // make variable to store path 
    std::vector<int> path(maze_size_,-1); 
    // make variable to store if a specified square has been visited 
    std::vector<std::vector<bool>> visited(maze_width_,std::vector<bool>(maze_height_,false)); 
    //  we can use a queue implementation to visit x and y coordinates 
    std::queue<int> x_coord;
    std::queue<int> y_coord;
    // We make a vector to store our distance 
    std::vector<std::vector<int>> distance(maze_width_,std::vector<int>(maze_height_,0));
    // we start from our start condition at the top as specified 
    int start_x = startX; 
    int start_y = 0 ;

    // push the coordinates in the queue
    x_coord.push(start_x);
    y_coord.push(start_y);
    // mark it as visited
    visited[start_x][start_y] = true; 
    // now start our bfs search for the longest path down the maze from the specified start position
    while(!x_coord.empty() && !y_coord.empty()){
        int x = x_coord.front();
        int y = y_coord.front();
        // convert from 2D coordinates to single to store our path in a 1D vector 
        int pathIdx = x + y*maze_width_;
        //std:: cout << "x:" << x  << " y:" << y << std::endl;
        //now we pop the elements once we have them 
        x_coord.pop();
        y_coord.pop();

        // check if you can travel and the cooridnate is not visited , add that to the queue S
        if(canTravel(x,y,RIGHT) && !visited[x +1][y] ) {
            //std::cout << "enter right " << std::endl;
            // mark it as visited 
            visited[x+1][y] = true;
            // add element to queue 
            x_coord.push(x+1);
            y_coord.push(y);
            // add path to the vector 
            path[pathIdx+1] = pathIdx;
            // add distance 
            distance[x +1][y] = distance[x][y] + 1;

        }
        //Check for DOWN
        if(canTravel(x,y,DOWN) && !visited[x][y + 1] ) {
            //std::cout << "visited value "<<!visited[x][y + 1] << std::endl;
            // mark it as visited 
            visited[x][y+1] = true;
            // add element to queue 
            x_coord.push(x);
            y_coord.push(y+1);
            // add path to the vector 
            path[pathIdx+maze_width_] = pathIdx;
            // add distance 
            distance[x][y+1] = distance[x][y] + 1;

        }
        //Check for LEFT 
        if(canTravel(x,y,LEFT) && !visited[x -1][y]) {
            //std::cout << "enter left " << std::endl;
            // mark it as visited 
            visited[x-1][y] = true;
            // add element to queue 
            x_coord.push(x-1);
            y_coord.push(y);
            // add path to the vector 
            path[pathIdx-1] = pathIdx;
            // add distance 
            distance[x-1][y] = distance[x][y] + 1;
        }
        // Check for UP 
        if(canTravel(x,y,UP) && !visited[x][y-1] ) {
            //std::cout << "enter up " << std::endl;
            // mark it as visited 
            visited[x][y-1] = true;
            // add element to queue 
            x_coord.push(x);
            y_coord.push(y-1);
            // add path to the vector 
            path[pathIdx-maze_width_] = pathIdx;
            // add distance 
            distance[x][y-1] = distance[x][y] + 1;

        }

    }


    //std::cout << "path vect size :"<<path.size() << std::endl ; 
    //std::cout <<"distance vect size :" <<distance.size() << std::endl;

    // now we have all the distances and iterate over it to find the longest path 
    int longest_path = 0 ;
    int pathIdx = 0; 
    // for(int i = 0; i < maze_width_; i++){
    //    std::cout << "dist" << i << ":" << distance[maze_height_-1][i] << std::endl;
    //     if (longest_path < distance[maze_height_ -1 ][i]){
    //         longest_path = distance[maze_height_ - 1][i];
    //         pathIdx = i;
    //     }
    // }
    for (int i = 0; i < maze_width_; ++i) {
        int exitIdx = i + (maze_height_ - 1) * maze_width_; // Bottom row index
        if (distance[i][maze_height_ - 1] > longest_path) {
            longest_path = distance[i][maze_height_ - 1];
            pathIdx = exitIdx;
        }
    }
    std::cout << pathIdx << std::endl;
    // now depending on what direction we are going in I can push back the directions , since we go in reverse we 
    std::vector<Direction> maze_path; 
    
    while(pathIdx != 0) {
        int prev_index = path[pathIdx];
        if(prev_index == pathIdx -1) {
            maze_path.push_back(RIGHT);
        } else if(prev_index== pathIdx - maze_width_) {
            maze_path.push_back(DOWN);
        } else if(prev_index== pathIdx +1) {
            maze_path.push_back(LEFT);
        } else if(prev_index== pathIdx + maze_width_){
            maze_path.push_back(UP);
        }
        pathIdx = prev_index;
    }
    // lastly we just flip the order of the vector to get the result back in the right order 
    std::reverse(maze_path.begin(), maze_path.end());
    //std::cout << "maze size:" << maze_path.size() << std::endl;
    //we return the maze_path
    return maze_path;
}

cs225::PNG* SquareMaze::drawMaze(int start) const {
    // create a new PNG  with new height and base 
    int png_height = maze_height_*10 + 1;
    int png_width = maze_width_*10 + 1; 
    // make maze PNG 
    cs225::PNG* maze = new cs225::PNG(png_width,png_height);
    //std::cout << "here1" << std::endl;
    // blacken topmost row 
    for(int i = 0 ; i < png_width; i++) {
        // we leave the entrance
        if((i >start*10) && (i < ((start+1)*10))) {
            continue;
        } else {
            // we colour everything else in top row black 
            maze -> getPixel(i,0).l = 0;
        }
    }
    //std::cout << "here2" << std::endl;
    // blacken left most columm 
    for(int j = 0 ; j < png_height; j++){
        maze -> getPixel(0,j).l = 0;
    }
    //std::cout << "here3" << std::endl;
    // now we just iterate over our wall vector
    for (int x = 0 ; x < maze_width_;x++) {
        for( int y = 0 ; y < maze_height_;y++){
            //if right wall exists for that coordinate we colour a range of walls black in the PNG 
            if(maze_walls[x][y].first) {
                // colour walls black from ((x+1)*10, y*10 +k)
                for(int k  = 0 ; k <= 10; k++) {
                    maze-> getPixel((x+1)*10, (y*10 + k)).l = 0;
                }
            }
            //std::cout << "here4" <<std::endl;
            // if down wall exists for that coordinate we colour a range of walls black in the PNG 
            if(maze_walls[x][y].second) {
                for(int k  = 0 ; k <= 10; k++) {
                    maze-> getPixel(x*10 + k, (y+1)*10).l = 0;
                }
            }
            //std::cout << "here5" << std::endl;
        }
    }
    return maze; 
}

cs225::PNG* SquareMaze::drawMazeWithSolution(int start) {
    //start position 
    unsigned int start_x = start*10 +5;
    unsigned int start_y = 5;
    // call draw maze 
    cs225::PNG* drawn_maze = drawMaze(start);
    std::cout << "png height" << drawn_maze->height()<< std::endl;
    std::cout << "png width" << drawn_maze->width()<< std::endl;
    // call solve maze 
    std::vector<Direction> solution = solveMaze(start); 

    // iterate over vector and colour solution red 
    // for(auto dir : solution) {
    //     std::cout << dir<< std::endl;
    // }
    // for(size_t s = 0 ; s < solution.size();s++) {
    //     std::cout << "start x:" << start_x << std::endl;
    //     std::cout << "start y:" << start_y << std::endl;
    //     if(solution[s] == RIGHT ) {
    //         for(unsigned int k = 0 ; k <= 10; k++) {
    //             //std::cout <<"pixel access" <<start_x + k << std::endl ; 
    //             cs225::HSLAPixel& pixel = drawn_maze-> getPixel(start_x + k, start_y);
    //             pixel.h = 0;
    //             pixel.s = 1;
    //             pixel.l = 0.5;
    //             pixel.a = 1;

    //         }
    //         // update the x position
    //         start_x += 10;
    //     } else if (solution[s] == DOWN) {
    //         for(unsigned int  k = 0 ; k <= 10; k++) {
    //             cs225::HSLAPixel& pixel = drawn_maze-> getPixel(start_x, start_y + k);
    //             pixel.h = 0;
    //             pixel.s = 1;
    //             pixel.l = 0.5;
    //             pixel.a = 1;

    //         }
    //         // update the y position
    //         start_y += 10;
    //     } else if (solution[s] == UP) {
    //         for(unsigned int k = 0 ; k <= 10; k++) {
    //           cs225::HSLAPixel& pixel = drawn_maze-> getPixel(start_x, start_y - k);  
    //             pixel.h = 0;
    //             pixel.s = 1;
    //             pixel.l = 0.5;
    //             pixel.a = 1;
    //         }
    //         // update the y position
    //         start_y-=10;
    //     } else if (solution[s] == LEFT) {
    //         for(unsigned int  k = 0 ; k <= 10; k++) {
    //          cs225::HSLAPixel& pixel = drawn_maze-> getPixel(start_x-k, start_y);  
    //          pixel.h = 0;
    //         pixel.s = 1;
    //         pixel.l = 0.5;
    //         pixel.a = 1; 
    //         }
    //         // update the x position
    //         start_x -= 10;
    //     }   
    // }

    // for (unsigned int k = 1 ; k < 10 ;k++) {
    //     cs225::HSLAPixel& pixel = drawn_maze-> getPixel(start_x*10 +k,(start_y+1)*10);
    //     pixel.l = 1;
        
    // }
    // // return final maze 
    // return drawn_maze;
unsigned int maze_width = drawn_maze->width();
unsigned int maze_height = drawn_maze->height();

for (size_t s = 0; s < solution.size(); s++) {
    if (solution[s] == RIGHT) {
        for (unsigned int k = 0; k <= 10; k++) {
            if (start_x + k < maze_width) { // Boundary check
                cs225::HSLAPixel& pixel = drawn_maze->getPixel(start_x + k, start_y);
                pixel.h = 0;
                pixel.s = 1;
                pixel.l = 0.5;
                pixel.a = 1;
            }
        }
        start_x += 10; // Update position
    } else if (solution[s] == DOWN) {
        for (unsigned int k = 0; k <= 10; k++) {
            if (start_y + k < maze_height) { // Boundary check
                cs225::HSLAPixel& pixel = drawn_maze->getPixel(start_x, start_y + k);
                pixel.h = 0;
                pixel.s = 1;
                pixel.l = 0.5;
                pixel.a = 1;
            }
        }
        start_y += 10; // Update position
    } else if (solution[s] == UP) {
        for (unsigned int k = 0; k <= 10; k++) {
            if (start_y >= k) { // Boundary check
                cs225::HSLAPixel& pixel = drawn_maze->getPixel(start_x, start_y - k);
                pixel.h = 0;
                pixel.s = 1;
                pixel.l = 0.5;
                pixel.a = 1;
            }
        }
        start_y -= 10; // Update position
    } else if (solution[s] == LEFT) {
        for (unsigned int k = 0; k <= 10; k++) {
            if (start_x >= k) { // Boundary check
                cs225::HSLAPixel& pixel = drawn_maze->getPixel(start_x - k, start_y);
                pixel.h = 0;
                pixel.s = 1;
                pixel.l = 0.5;
                pixel.a = 1;
            }
        }
        start_x -= 10; // Update position
    }
}

// Ensure this part also considers bounds
for (unsigned int k = 1; k < 10; k++) {
        cs225::HSLAPixel& pixel = drawn_maze->getPixel((start_x/10)*10 + k, ((start_y/10) +1)*10);
        pixel.l = 1;
    
}
return drawn_maze; 
}