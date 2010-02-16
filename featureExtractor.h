#ifndef FEATUREEXTRACTOR_H
#define FEATUREEXTRACTOR_H

#ifndef DESCRIPTOR_LOCATION
#define DESCRIPTOR_LOCATION "desc/"
#endif

#include <Magick++.h>
#include <vector>
#include "parameters.h"
#include "cv.h"
#include "highgui.h"
#include "category.h"
#include "myImage.h"
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include "descriptorWriter.h"
#include "dataset.h"
#include "feature.h"
#include "histogram.h"


using namespace cv;
using namespace Magick;

class FeatureExtractor{
 private:
  vector<Feature*> features;
 public:
  FeatureExtractor();
  vector< vector<float> >  collectDescriptors(vector<DataPoint> points);
  void saveDescriptorsToFile(Dataset * ds);
  vector<string> createAndSaveDescriptors(vector<Category> * particip);
  void renewDescriptor(DataPoint * db);
};

#endif
