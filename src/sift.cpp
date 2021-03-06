#include "sift.h"
#define PI 3.14159265

using std::min;
using std::vector;
using std::cout;
using std::endl;

using Magick::Image;
using Magick::Color;
using Magick::DrawableCircle;
using Magick::DrawableLine;

namespace vito{
namespace features{

bool SiftDescriptor::isActive(){
  return (Parameters::getInstance()->getiParameter("feature_sift") > 0);
}

Descriptor SiftDescriptor::extract_(MyImage *my_image,
			bool save_visual_representation,
			Image *canvas){
  //Arrange structures
  Descriptor descriptor;
  Parameters * parameters = Parameters::getInstance();
  const int kBlurWindow = parameters->getiParameter("sift_blur_window");
  if(parameters->getiParameter("sift_blur_toggle") > 0)
    my_image->getMagickImage()->blur(kBlurWindow, kBlurWindow / 3.0);
  Matrix<float> grayscale = my_image->getGrayscaleMatrix();
  Matrix<Gradient> gradient = imageGradient(grayscale);

  //Configure Parameters
  int orientations = parameters->getiParameter("sift_orientation_directions");
  int kKeyPoints_x = parameters->getiParameter("sift_number_of_keypoints_x");
  int kKeyPoints_y = parameters->getiParameter("sift_number_of_keypoints_x");
  int window_dummy;
  const int auto_window_type = parameters->getiParameter("sift_auto_window_size_type");
  if(parameters->getiParameter("sift_keypoint_pixel_window") < 0){
    if(auto_window_type == 0){
      window_dummy = min((gradient.get_width() -3) / (2 * kKeyPoints_x),
			 (gradient.get_height() -3)  / (2 * kKeyPoints_y));
    }else if(auto_window_type == 1){
      int window_width = gradient.get_width() / kKeyPoints_x;
      int window_height = gradient.get_height() / kKeyPoints_y;
      window_dummy = sqrt(window_width * window_width + window_height * window_height) / 2;
    }
  }  else 
    window_dummy = parameters->getiParameter("sift_keypoint_pixel_window");
  const int window = window_dummy;

  //Spread up Keypoints
  vector<sift::KeyPoint> keypoints;
  keypoints = sift::divideIntoKeypoints(gradient.get_width(), 
					gradient.get_height(), 
					kKeyPoints_x, kKeyPoints_y);

  // prepare visual represenation if it'll be drawn
  if(save_visual_representation){
    *canvas = *my_image->getMagickImage();
    canvas->fillColor(Color());
  }

  //gather descriptors
  for(vector<sift::KeyPoint>::iterator keypoint = keypoints.begin();
      keypoint != keypoints.end(); ++keypoint){ 
    Descriptor new_descriptor;
    int sub_hists = parameters->getiParameter("sift_sub_hists");

    if(sub_hists > 1) 
      new_descriptor = 
	getKeyPointDescriptor( &gradient, *keypoint,  window, orientations, sub_hists );
    else
      new_descriptor = 
	getKeyPointDescriptor( &gradient, *keypoint,  window, orientations);

    descriptor = descriptor + new_descriptor;
    if(save_visual_representation){
      this->drawKeyPoint(*canvas, orientations, *keypoint, new_descriptor, window);
    }
  }
  descriptor.normalize();
  return descriptor;
}



void SiftDescriptor::drawKeyPoint(Image &draw_me, const int &orientations,
				  const sift::KeyPoint &keypoint, 
				  Descriptor orig_descriptor, 
				  const int &window){
  //orig_descriptor.normalize();
  Parameters *params = Parameters::getInstance();
  const int kHistograms = params->getiParameter("sift_histograms_per_keypoint");
  const int kDescriptors = orig_descriptor.size() / orientations;
  assert(kHistograms * kHistograms == kDescriptors);
  int sub_window_size = (window * 2) / kHistograms;
  draw_me.strokeColor("green");
  /*
  draw_me.draw( DrawableCircle(keypoint.get_center_x(),
			       keypoint.get_center_y(),
			       keypoint.get_center_x() + window, 
			       keypoint.get_center_y() ));
  */
  int offset = (sub_window_size / 2) - window;
  for(size_t x = 0; x < (size_t) kHistograms; ++x){
    for(size_t y = 0; y < (size_t) kHistograms; ++y){
      Descriptor descriptor;
      int index = (x + y * kHistograms);
      descriptor.insert(descriptor.end(),
			orig_descriptor.begin() +  index * orientations,
			orig_descriptor.begin() + (index + 1) * orientations);
      int origin_x = keypoint.get_center_x() + offset + sub_window_size * x;
      int origin_y = keypoint.get_center_y() + offset + sub_window_size * y;
      float bin_size = ((2 * PI) / orientations);
      draw_me.strokeColor("red");
      for(int ori = 0; ori < orientations; ++ori){
	float angle = (bin_size * (ori + 1)) - (0.5 * bin_size);
	if(params->getiParameter("sift_phase_shift_ori_bins"))
	  angle -= (1.0 / orientations) * PI;
	float length = 15.0;
	int end_x = origin_x + descriptor.at(ori) * window * length  * sin(angle);
	int end_y = origin_y - descriptor.at(ori) * window * length * cos(angle);
	draw_me.draw( DrawableLine( origin_x, origin_y, end_x, end_y));
      }
    }
  }
}

Descriptor SiftDescriptor::getKeyPointDescriptor(Matrix<Gradient> * gradient,
						 const sift::KeyPoint &keypoint,
						 const size_t window_size,
						 int kOrientations){
  Parameters * params = Parameters::getInstance();
  bool phase_shift = params->getiParameter("sift_phase_shift_ori_bins") > 0;
  int
    window_left  = keypoint.get_center_x() - window_size,
    window_up    = keypoint.get_center_y() - window_size,
    kHistograms = params->getiParameter("sift_histograms_per_keypoint"),
    histogram_range = (2 * window_size) / (kHistograms);
  Descriptor total_bins;
  for(size_t hist_y = 0; hist_y < (size_t) kHistograms; ++hist_y){
    for(size_t hist_x = 0; hist_x < (size_t) kHistograms; ++hist_x){
      int
	left  = window_left + histogram_range * hist_x,
	right = left + histogram_range,
	up = window_up + histogram_range * hist_y,
	down  = up + histogram_range;
      Descriptor bins( vector<float>(kOrientations, 0.0f));
      if(phase_shift)
	bins.resize(2 * kOrientations);
      for(size_t i = 0; i < bins.size(); ++i)
	bins[i] = 0.0;
      int pixels_used = 0;
      for(int x = left; x < right; ++x){
	for(int y = up; y < down; ++y){
	  if(x < 0 || x >= (int) gradient->get_width() ||
	     y < 0 || y >= (int) gradient->get_height()){
	    //pixel is outside of bounds
	  } else{
	    pixels_used++;
	    bin(gradient->at(x,y), 
			  bins, 1.0);
	  }
	}
      }
      for(size_t i = 0; i < bins.size(); ++i){
	if(pixels_used > 0)
	  bins[i] =  bins[i] / (float) pixels_used;
	else
	  bins[i] = 0.0;
      }
      if(phase_shift){
	Descriptor real_bins (kOrientations);
	for(size_t i = 0; i < bins.size(); i++)
	  real_bins[((i + 1) / 2) % kOrientations] = bins[i];
	bins = real_bins;
      }
      total_bins = total_bins + bins;
    }
  }
  return total_bins;
}

Descriptor SiftDescriptor::getKeyPointDescriptor(Matrix<Gradient> * gradient,
						 const sift::KeyPoint &keypoint,
						 const size_t window_size,
						 int kOrientations,
						 const int hists){
  Descriptor total;
  const size_t new_window_size = window_size / hists;
  cout << "keyp: x: " << keypoint.get_center_x() << " y: " 
	 << keypoint.get_center_y() << " at: " << window_size << endl
	 << " wordt..." << endl;
  for(int y_hist = 0; y_hist < hists; y_hist++){
    for(int x_hist = 0; x_hist < hists; x_hist++){
      size_t new_center_x = keypoint.get_center_x() - (window_size * 0.5) 
	+ ((x_hist + 1) * new_window_size) - (new_window_size / 2);
      size_t new_center_y = keypoint.get_center_y() - (window_size * 0.5) 
	+ ((y_hist + 1) * new_window_size) - (new_window_size / 2);
      cout << "hist: y:" << y_hist << " x: " << x_hist  << " x: " << new_center_x 
	   << " y: " <<  new_center_y << " at " << new_window_size << endl;
      total = total + 
	getKeyPointDescriptor(gradient,
			      sift::KeyPoint(new_center_x, new_center_y, 1.0), 
			      new_window_size, hists);
    }
  }
  return total;
}

}}
