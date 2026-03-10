#include <cmath>
#include <iterator>
#include <iostream>
#include <vector>
#include "cs225/HSLAPixel.h"
#include "cs225/PNG.h"
#include "Point.h"

#include "ImageTraversal.h"

namespace Traversals {
  /**
  * Calculates a metric for the difference between two pixels, used to
  * calculate if a pixel is within a tolerance.
  *
  * @param p1 First pixel
  * @param p2 Second pixel
  * @return the difference between two HSLAPixels
  */
  double calculateDelta(const HSLAPixel & p1, const HSLAPixel & p2) {
    double h = fabs(p1.h - p2.h);
    double s = p1.s - p2.s;
    double l = p1.l - p2.l;

    // Handle the case where we found the bigger angle between two hues:
    if (h > 180) { h = 360 - h; }
    h /= 360;

    return sqrt((h*h) + (s*s) + (l*l));
  }
  
  /**
  * Adds a Point for the bfs traversal to visit at some point in the future.
  * @param work_list the deque storing a list of points to be processed
  * @param point the point to be added
  */
  void bfs_add(std::deque<Point> & work_list, const Point & point) {
    /** @todo [Part 1] */
    // just push back the points 
    work_list.push_back(point);
  }

  /**
  * Adds a Point for the dfs traversal to visit at some point in the future.
  * @param work_list the deque storing a list of points to be processed
  * @param point the point to be added
  */
  void dfs_add(std::deque<Point> & work_list, const Point & point) {
    /** @todo [Part 1] */
    // just pushback the point
    work_list.push_back(point);
  }

  /**
  * Removes the current Point in the bfs traversal
  * @param work_list the deque storing a list of points to be processed
  */
  void bfs_pop(std::deque<Point> & work_list) {
    /** @todo [Part 1] */
    // we remove the first point in the list 
    work_list.pop_front();
  }

  /**
  * Removes the current Point in the dfs traversal
  * @param work_list the deque storing a list of points to be processed
  */
  void dfs_pop(std::deque<Point> & work_list) {
    /** @todo [Part 1] */
    // we remove the first point in the list 
    work_list.pop_back();
  }

  /**
   * Returns the current Point in the bfs traversal
   * @param work_list the deque storing a list of points to be processed
   * @return the current Point 
   */
  Point bfs_peek(std::deque<Point> & work_list) {
    /** @todo [Part 1] */
    // returning the point at the front of the list and the popping it 
    Point current_point = work_list.front();
    return current_point;
  }

  /**
   * Returns the current Point in the dfs traversal
   * @param work_list the deque storing a list of points to be processed
   * @return the current Point 
   */
  Point dfs_peek(std::deque<Point> & work_list) {
    /** @todo [Part 1] */
    // returning the point at the front of the list and the popping it 
    Point current_point = work_list.back();
    return current_point;
  }

  /**
  * Initializes a ImageTraversal on a given `png` image,
  * starting at `start`, and with a given `tolerance`.
  * @param png The image this traversal is going to traverse
  * @param start The start point of this traversal
  * @param tolerance If the current point is too different (difference larger than tolerance) with the start point,
  * it will not be included in this traversal
  * @param fns the set of functions describing a traversal's operation
  */
  ImageTraversal::ImageTraversal(const PNG & png, const Point & start, double tolerance, TraversalFunctions fns) {  
    /** @todo [Part 1] */
    // initialize all the parameters for the img traversal 
    png_ = png;
    start_ = start;
    tolerance_ = tolerance;
    fns_ = fns;

  }

  /**
  * Returns an iterator for the traversal starting at the first point.
  */
  ImageTraversal::Iterator ImageTraversal::begin() {
    /** @todo [Part 1] */
    return ImageTraversal::Iterator(png_,start_,tolerance_,fns_);
  }

  /**
  * Returns an iterator for the traversal one past the end of the traversal.
  */
  ImageTraversal::Iterator ImageTraversal::end() {
    /** @todo [Part 1] */
    // google doc told to think about the end as when the traversal is done
    // if the traversal is done tecnically the iterator is empty and can return anything/ nothing 
    // hence we do not initialize it to anything 
    // still ask in OH ??
    return ImageTraversal::Iterator();
  }

  /**
  * Default iterator constructor.
  */
  ImageTraversal::Iterator::Iterator() {
    /** @todo [Part 1] */
  }
  // creating a new iterator constructor to provide necessary information in order to iterate throught the png image
  // as hinted to do in the google doc
  ImageTraversal::Iterator::Iterator(const PNG & png, const Point & start, double tolerance, TraversalFunctions fns) {
    png_ = png;
    start_ = start;
    tolerance_ = tolerance; 
    fns_ = fns; 
    // now we populate the worklist with our starting point of the iterator 
    fns_.add(work_list_,start_);
    //now we define what our iterator is pointing to currently 
    current = fns_.peek(work_list_);
    // create an vector to store whether any point has been traversed or not 
    traversed_ = std::vector<std::vector<bool>>(png_.width(), std::vector<bool>(png_.height(), false));
  }

  /**
  * Iterator increment operator.
  *
  * Advances the traversal of the image.
  */
  ImageTraversal::Iterator & ImageTraversal::Iterator::operator++() {
    /** @todo [Part 1] */
    //first we call all the points we need 
    Point curr_iter = current;
    Point up = Point(curr_iter.x,curr_iter.y-1);
    Point down = Point(curr_iter.x, curr_iter.y+1);
    Point right = Point(curr_iter.x+1, curr_iter.y);
    Point left = Point(curr_iter.x-1, curr_iter.y);

    // now we mark that we visited this point 

    traversed_[curr_iter.x][curr_iter.y] = true;

    //now we remove the point from the worklist 
    if(!work_list_.empty()){
      fns_.pop(work_list_);
    }
    

    // now we check for the tolerance of the pixels and whether they need to be added to the worklist 
    // we start traversing and checking with the starting pixel
    HSLAPixel start_pixel = png_.getPixel(start_.x, start_.y);
    if((right.x < png_.width()) && !(traversed_[right.x][right.y])) {
        HSLAPixel right_pixel = png_.getPixel(right.x,right.y);
        if((calculateDelta(start_pixel,right_pixel) < tolerance_)) {
          // if radius less than tolenrance we add it to the work list to traverse further 
          fns_.add(work_list_,right);
        }
    }
    if((down.y < png_.height()) && !(traversed_[down.x][down.y])) {
        HSLAPixel down_pixel = png_.getPixel(down.x,down.y);
        // if radius less than tolenrance we add it to the work list to traverse further 
        if((calculateDelta(start_pixel,down_pixel) < tolerance_)) {
          fns_.add(work_list_,down);
        }
    }
    if((left.x < png_.width()) && !(traversed_[left.x][left.y])) {
        HSLAPixel left_pixel = png_.getPixel(left.x,left.y);
        // if radius less than tolenrance we add it to the work list to traverse further 
        if((calculateDelta(start_pixel,left_pixel) < tolerance_)) {
          fns_.add(work_list_,left);
        }

    }
    if((up.y < png_.height()) && !(traversed_[up.x][up.y])) {
        HSLAPixel up_pixel = png_.getPixel(up.x,up.y);
        // if radius less than tolenrance we add it to the work list to traverse further 
        if((calculateDelta(start_pixel,up_pixel) < tolerance_)) {
          fns_.add(work_list_,up);
        }
    }
    

    while(!work_list_.empty()){
      curr_iter =fns_.peek(work_list_);
      current = curr_iter;
      if(traversed_[current.x][current.y]) {
        fns_.pop(work_list_);
      } else {
        break;
      }

    }
    return *this;
  }
  /**
  * Iterator accessor operator.
  *
  * Accesses the current Point in the ImageTraversal.
  */
  Point ImageTraversal::Iterator::operator*() {
    /** @todo [Part 1] */
    return current;
  }

  /**
  * Iterator inequality operator.
  *
  * Determines if two iterators are not equal.
  */
  bool ImageTraversal::Iterator::operator!=(const ImageTraversal::Iterator &other) {
    /** @todo [Part 1] */
    // the google doc says to think about what the iterator will look like when its at the end 
    // basically our list will be over/empty 
    // hence we check for empty 
    bool equal = !(work_list_.empty() && other.work_list_.empty());
    return equal;
  }

  /**
   * Iterator size function.
   *
   * @return size_t the size of the iterator work queue.
   */
  size_t ImageTraversal::Iterator::size() const {
    return work_list_.size();
  }

  /**
   * Iterator empty function.
   *
   * @return bool whether the iterator work queue is empty.
   */
  bool ImageTraversal::Iterator::empty() const {
    return work_list_.empty();
  }


}