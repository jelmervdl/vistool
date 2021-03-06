#ifndef SIFT_H
#define SIFT_H

#include <assert.h>
#include <math.h>
#include <iostream>
#include "keypoint.h"
#include "gradient.h"

namespace vito{
namespace features{

class SiftDescriptor: public Feature, public Singleton<SiftDescriptor> {
  using Feature::extract;
private:
 
  virtual bool isActive();

  // get the descriptor belonging to a single keypoint
  Descriptor   getKeyPointDescriptor(Matrix<Gradient> * gradient,
				     const sift::KeyPoint &keypoint,
				     const size_t window_size,
				     int kOrientations);

  Descriptor   getKeyPointDescriptor(Matrix<Gradient> * gradient,
				     const sift::KeyPoint &keypoint,
				     const size_t window_size,
				     int kOrientations,
				     const int hists);


  // draw a single keypoint onto a canvas to construct visualisation
  void         drawKeyPoint(Magick::Image &draw_me, 
			  const int &orientations,
			  const sift::KeyPoint &keypoint, 
			  Descriptor descriptor, const int &window);
public: 

  // extract sift feature from an image, saving visual representation
  // is optional
  Descriptor   extract_(MyImage *Image,
		      bool save_visual_representation,
		      Magick::Image *canvas);

  virtual std::string getParameterName(){ return "sift";}
};

}}

#endif
