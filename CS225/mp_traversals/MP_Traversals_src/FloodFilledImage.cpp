#include "cs225/PNG.h"
#include <list>
#include <iostream>

#include "colorPicker/ColorPicker.h"
#include "ImageTraversal.h"

#include "Point.h"
#include "Animation.h"
#include "FloodFilledImage.h"

using namespace cs225;

/**
 * Constructs a new instance of a FloodFilledImage with a image `png`.
 * 
 * @param png The starting image of a FloodFilledImage
 */
FloodFilledImage::FloodFilledImage(const PNG & png) {
  /** @todo [Part 2] */
  png_ = png;
  frame_interval_ = 0;
}

/**
 * Adds a FloodFill operation to the FloodFillImage.  This function must store the operation,
 * which will be used by `animate`.
 * 
 * @param traversal ImageTraversal used for this FloodFill operation.
 * @param colorPicker ColorPicker used for this FloodFill operation.
 */
void FloodFilledImage::addFloodFill(Traversals::ImageTraversal & traversal, ColorPicker & colorPicker) {
  /** @todo [Part 2] */
  traversal_.push_back(traversal);
  color_picker_.push_back(&colorPicker);

}

/**
 * Creates an Animation of frames from the FloodFill operations added to this object.
 * 
 * Each FloodFill operation added by `addFloodFill` is executed based on the order
 * the operation was added.  This is done by:
 * 1. Visiting pixels within the image based on the order provided by the ImageTraversal iterator and
 * 2. Updating each pixel to a new color based on the ColorPicker
 * 
 * While applying the FloodFill to the image, an Animation is created by saving the image
 * after every `frameInterval` pixels are filled.  To ensure a smooth Animation, the first
 * frame is always the starting image and the final frame is always the finished image.
 * 
 * (For example, if `frameInterval` is `4` the frames are:
 *   - The initial frame
 *   - Then after the 4th pixel has been filled
 *   - Then after the 8th pixel has been filled
 *   - ...
 *   - The final frame, after all pixels have been filed)
 * @param frameInterval how often to save frames of the animation
 */ 
Animation FloodFilledImage::animate(unsigned frameInterval) const {
  /** @todo [Part 2] */
  Animation animation;
  PNG frame_ = png_;
  // add the first frame to the animation
  animation.addFrame(frame_);
  // start the count for frame intervals and colour picker vector 
  unsigned int count_frame_interval = frame_interval_;
  // start iterating through the traversals 
  for(unsigned long i = 0; i < traversal_.size(); i++) {
    //select a traversals {note advamced for loop creates some kind of out of bounds error for god knows what reason and wasted 2 hours }
    auto tra_select = traversal_[i];
    // iterate over the traversal operations on pixels
    for(auto pixel_operation: tra_select) {
      // if our frameinterval reaches what we want we can add a new frame 
        if(count_frame_interval == frameInterval) {
          //set count back to 0
          count_frame_interval = 0;
          // add frame
          animation.addFrame(frame_);
        }
      // if our frame interval is not what we want we just colour the pixel 
      auto pixel_x = pixel_operation.x;
      auto pixel_y = pixel_operation.y;
      frame_.getPixel(pixel_x, pixel_y) = color_picker_.at(i)->getColor(pixel_x,pixel_y);
       // increase frame interval to keep count
      count_frame_interval++;
    }
  }
  //lastly add the frame after all traversals are done 
  animation.addFrame(frame_);
// return final animation and hopefully it works 
  return animation;
}
