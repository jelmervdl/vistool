#ifndef CATEGORY_H
#define CATEGORY_H

/* Contains all relevant information regarding a single class/category
   of images. This include the class name, it's integer label, the
   location of the images on disk and a vector of these images
   themselves. To be used mainly by Dataset.*/

#include <algorithm>
#include"dataPoint.h"

namespace vito{ 

class Category{
 private:
  std::string            name;
  std::string            root; 
  std::vector<DataPoint> data_points;
  int                    enabled; 
  size_t                 label;

 public:
  Category(std::string give_name,
	   std::string root, 
	   size_t lab,
	   std::vector<DataPoint> dps);
  size_t                   size()      const;
  size_t                   get_label() const;
  std::string              get_name()  const;
  std::string              get_root()  const;

  void                     shuffle();

  // return the live variable for the gui for this class' activation
  int *                    enabledLiveVar();
  std::vector<DataPoint> * get_data_points();
  std::vector<DataPoint>   get_data_points_();

  void                     addDataPoint(DataPoint dp);
};

}

#endif
