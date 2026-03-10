/**
 * @file kdtree.cpp
 * Implementation of KDTree class.
 */

#include <utility>
#include <algorithm>
#include <deque>
#include <cmath>
using namespace std;

template <int Dim>
bool smallerDimVal(const Point<Dim>& first,
                                const Point<Dim>& second, int curDim)
{
    /**
     * @todo Implement this function!
     */
    if(first[curDim] < second[curDim]) {
      return true;
    }
    if(first[curDim] == second[curDim]) {
      return (first < second);
    }
    return false;
}

template <int Dim>
bool shouldReplace(const Point<Dim>& target,
                                const Point<Dim>& currentBest,
                                const Point<Dim>& potential)
{
    /**
     * @todo Implement this function!
     */
      double currbest_target_distance = 0;
      double potential_target_distance = 0; 
      for(int i = 0 ; i < Dim ; i++) {
        currbest_target_distance += (target[i] - currentBest[i])*(target[i] - currentBest[i]);
      }
      for(int i = 0 ; i < Dim ; i++) {
        potential_target_distance += (target[i] - potential[i])*(target[i] - potential[i]);
      }
      if(currbest_target_distance > potential_target_distance) {
        return true;
      }
      if(currbest_target_distance == potential_target_distance) {
        return potential < currentBest;
      }
     return false;
}

template <int Dim>
KDTree<Dim>::KDTree(const vector<Point<Dim>>& newPoints)
{
    /**
     * @todo Implement this function!
     */
    if(newPoints.empty()) {
      return;
    } else {
      std::vector<Point<Dim>> tree_vec; // create vector to store points and modify 
      for(auto i : newPoints) {
        tree_vec.push_back(i);
      }
      tree_creator(tree_vec,0,0,tree_vec.size()-1,root);
      
    }
}
template <int Dim>
void KDTree<Dim>::tree_creator(std::vector<Point<Dim>>& tree_points, int dimension, int left, int right, KDTreeNode*& root) {
  if(left <= right) {
  int middle = floor((left + right)/2);
  auto middle_iter = tree_points.begin() + middle;
  auto cmp = [dimension] (const Point<Dim>& p1, const Point<Dim>& p2) {
    return smallerDimVal(p1,p2,dimension);
  };
  select(tree_points.begin() + left, tree_points.begin() + right + 1, middle_iter, cmp);
  root = new KDTreeNode(tree_points[middle]);
// create nodes for the left subtree
  tree_creator(tree_points, (dimension+1)%Dim, left, middle - 1,root->left);
// create nodes for the right subtree
  tree_creator(tree_points,(dimension+1)%Dim,middle+1, right, root->right);

}

}
template <int Dim>
KDTree<Dim>::KDTree(const KDTree<Dim>& other) {
  /**
   * @todo Implement this function!
   * 
   */
  if(other == NULL) {
    return;
  }
  copy(root,other);
}
template <int Dim>
void KDTree<Dim>:: copy(KDTreeNode*& copy, KDTreeNode*& original) {
  if(original == NULL) {
    return;
  } else {
    copy = new KDTreeNode(original);
    copy(copy->right, original->right);
    copy(copy->left, original->left);
  }

}

template <int Dim>
const KDTree<Dim>& KDTree<Dim>::operator=(const KDTree<Dim>& rhs) {
  /**
   * @todo Implement this function!
   */
 if(root == NULL){
    return;
  }
  delete(root->right);
  delete(root->left);
  delete root;
  copy(root, rhs->root);
  return *this;
}

template <int Dim>
KDTree<Dim>::~KDTree() {
  /**
   * @todo Implement this function!
   */
  if(root == NULL){
    return;
  }
  delete(root->right);
  delete(root->left);
  delete root;
}

template <int Dim>
Point<Dim> KDTree<Dim>::findNearestNeighbor(const Point<Dim>& query) const
{
    /**
     * @todo Implement this function!
     */
      // naive implementation of the algorithm 
      // we need helper function which takes in root  query and dimension 

    return findingNearestNeighbour(query, root, 0);
    //return Point<Dim>();
}


// template <int Dim>
// Point<Dim> KDTree<Dim>::findingNearestNeighbour(const Point<Dim>& query,KDTreeNode* current, int dimension) const {
// // first step is to find the smaller value in the current dimension to choose a way to procced 
// Point<Dim> curr_point = current -> point;
// // finds the smallest point in the dimension we need 
// //targetCoords[2] = {45, 59};
// bool go_left = smallerDimVal(query,curr_point, dimension);
// bool go_right = !go_left;
// // now choose left or right and set nearest 
// Point<Dim> nearest;
// if(go_left && !go_right  && current -> left != nullptr) {
//   // find query in the left side tree
//     nearest = findingNearestNeighbour(query,current->left,(dimension+1)%Dim);
// } else if (!go_left  && go_right && current->right != nullptr) {
//   // find query in the right side of tree
//   nearest = findingNearestNeighbour(query,current->right,(dimension+1)%Dim);
// } else {
//   nearest = current->point;
// }
// // check for replacing 
// if (shouldReplace(query,curr_point, nearest)) {
//   curr_point = nearest;
// }
// std::cout <<"(" <<nearest[0] << "," << nearest[1] << ") ("<<  curr_point[0] << "," << curr_point[1]<<")" <<std::endl;
// // the distance between query and current  
// double square_dist = (query[dimension] - curr_point[dimension])*(query[dimension] - curr_point[dimension]);

// //the distance between query and nearest 
// double radius = 0.0;
// for (int i = 0 ; i < Dim ; i++) {
//         radius += ((query[i] - nearest[i])*(query[i] - nearest[i]));
// }
// std::cout << radius  << ":" << square_dist << std::endl;
// // back traversal and chcek other nodes 

// if(radius >= square_dist) {
//   Point<Dim> close_temp;
//  if(go_left && !go_right && current->right !=nullptr) {
//   close_temp = findingNearestNeighbour(query,current->right,(dimension+1)%Dim);
//  } else if (!go_left && go_right && current->left != nullptr) {
//   close_temp = findingNearestNeighbour(query,current->left,(dimension+1)%Dim);
//  } else  {
//   close_temp = curr_point;
//  }
//  if(shouldReplace(query,curr_point,close_temp)) {
//   curr_point = close_temp;
//  }
// }
 
// //::cout << "\n" << std::endl;
// return curr_point;

// }
template <int Dim>
Point<Dim> KDTree<Dim>::findingNearestNeighbour(const Point<Dim>& query, KDTreeNode* current, int dimension) const {
    if (current == nullptr || (current->left == nullptr && current->right == nullptr)) return current->point; //return Point<Dim>();
  // testing the current point 
  Point<Dim> curr_point = current->point;
  // variable to store the nearest point we find 
  Point<Dim> nearest;

  // set whether we are going left or right 
  bool go_left = smallerDimVal(query, current->point, dimension);
  bool go_right = !go_left;
  

  if (go_left && !go_right && current->left != nullptr) { 
    // go left and search subtree
    nearest = findingNearestNeighbour(query, current->left, (dimension + 1) % Dim);
    
  } else if (!go_left && go_right && current->right != nullptr) { 
    // go right and search subtree
    nearest = findingNearestNeighbour(query, current->right, (dimension + 1) % Dim);
  } else {
    // set our nearest as the current point
    nearest = current->point;
  }
  // now we replace if our curr point is nearer than our nearest point 
  if (shouldReplace(query, curr_point, nearest)) {
    curr_point = nearest;
  }
  
  // finding the radius we search within to find a point in another hyper rectangle 
  double radius = 0.0;
  for (int i = 0 ; i < Dim ; i++) {
    radius += ((query[i] - nearest[i])*(query[i] - nearest[i]));
  }
  double square_dist = (query[dimension] - current->point[dimension]) * (query[dimension] - current->point[dimension]);

  // if our curr sqaure distance is less than the radius we back track and search other parts of the tree 
  if (square_dist <= radius) {
    Point<Dim> temp_close;

    if (!go_left && go_right && current->left != nullptr) {
      // if we went right before then we need to go to the left tree 
      temp_close = findingNearestNeighbour(query, current->left, (dimension + 1) % Dim);
      // we replace current point as the closest point 
      if (shouldReplace(query, curr_point, temp_close)) {
      curr_point = temp_close;
    }
    } else if (go_left && !go_right && current->right != nullptr) {
      // if we went to the left tree before we need to go the right 
      temp_close = findingNearestNeighbour(query, current->right, (dimension + 1) % Dim);
      // we replace our current point as the closest point 
      if (shouldReplace(query, curr_point, temp_close)) {
      curr_point = temp_close;
    }
    } else {
      // otherwise set closest as the current point 
      temp_close = current->point;
    }
  }
  return curr_point;
}
template <typename RandIter, typename Comparator>
void select(RandIter start, RandIter end, RandIter k, Comparator cmp)
{
    /**
     * @todo Implement this function!
     */    
  // std::cout << "enter" << std::endl;
  RandIter last = end -1;
  if(start == last) {
    return;
  }  
  RandIter pivot_index  = start + floor(std::rand() % (last - start + 1)); 
  pivot_index = Partition(start,last,pivot_index,cmp);
  if(pivot_index == k) {
    return;
  } else if  (k < pivot_index) {
    // std::cout << "last here" << std::endl;
    select(start,pivot_index,k,cmp);
  } else {
    // std::cout << "last here 2" << std::endl;
    select(pivot_index+1,end,k,cmp);
  }
}
template <typename RandIter, typename Comparator>
RandIter Partition(RandIter left, RandIter right, RandIter pivot, Comparator cmp) {
auto pivotValue = *pivot;
std::iter_swap(pivot,right);
RandIter storeIndex = left;
for(auto i = left; i != right; i++) {
  if(cmp(*i,pivotValue)) {
    std::iter_swap(storeIndex,i);
    storeIndex++;
  }
}
std::iter_swap(right,storeIndex);
return storeIndex;
}


