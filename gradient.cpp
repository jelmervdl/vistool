#include "gradient.h"

#define PI 3.14159265

Gradient::Gradient(float magnitude_, float orientation_)
  :magnitude(magnitude_), orientation(orientation_){
}

Gradient::Gradient(){
};


float Gradient::get_magnitude(){
  return magnitude;
}

float Gradient::get_orientation(){
  return orientation;
}

Gradient singleGradient(Matrix<float> * image, 
		    size_t center_pixel_x, size_t center_pixel_y){
  //Parameters * parameters = Parameters::getInstance();
  size_t width = image->get_width(), height = image->get_height();

  if(center_pixel_x > width - 1 || center_pixel_x < 1 
     || center_pixel_y > height - 1 || center_pixel_y < 1){
    cout << 
      "requesting gradient of out of bound pixel. " <<
      "pixels are out of bounds when they are not within image " << 
      "or within the outermost pixel layer" <<  endl;
    return Gradient(0,0);
  }

  //size_t sift_orientation_directions = parameters->getiParameter("sift_orientation_directions");

  float dx,dy; // dx is the gradient to the right, dy is the gradient to the bottom.
  dx = *image->at(center_pixel_x+1, center_pixel_y) - 
       *image->at(center_pixel_x-1, center_pixel_y);
  dy = *image->at(center_pixel_x, center_pixel_y-1) - 
       *image->at(center_pixel_x, center_pixel_y+1);
  return Gradient (sqrt(dx * dx + dy * dy), atan2( dy, dx));
}

Matrix<Gradient> imageGradient(Matrix<float> * image ){
  cout << "hello" << endl;
  size_t width = image->get_width(), height = image->get_height();
  cout << "width " << width << " height " << height << endl;
  Matrix<Gradient> gradient_image(width-2, height-2);
  for(size_t x = 1; x < image->get_width() - 1; ++x)
    for(size_t y = 1; y < image->get_height() - 1; ++y)
      *gradient_image.at(x - 1, y - 1) = singleGradient(image, x, y);
  return gradient_image;
}

void bin(Gradient * gradient, vector<float> * bin_values){
  float angle = gradient->get_orientation() / PI;
  size_t bins = bin_values->size();
  float bin_size = 2.0 / bins;
  float angle_min = angle - (0.5 * bin_size);
  float angle_max = angle + (0.5 * bin_size);

  if(angle_min > 1.0)
    angle_min -= 2.0;
 if(angle_min < -1.0)
   angle_min += 2.0;

 if(angle_max > 1.0)
   angle_max -= 2.0;
 if(angle_max < -1.0)
   angle_max += 2.0;

 for(size_t bin = 0; bin < bins; ++bin){
   float bin_max = 1.0 - bin * bin_size;
   float bin_min = 1.0 - (bin + 1) * bin_size;
   float share = 0.0;
   if(angle_min >= bin_min && angle_min <= bin_max)
     share = ((bin_max - angle_min) / bin_size) * gradient->get_magnitude();
   else if(angle_max >= bin_min && angle_max <= bin_max)
     share = ((angle_max - bin_min) / bin_size) * gradient->get_magnitude();
   bin_values->at(bin) += share;   
 }
}
