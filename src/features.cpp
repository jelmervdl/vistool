#include "features.h"

using std::vector;

namespace vito{
namespace features{

using mpeg7::EdgeHistogram;
using mpeg7::ScalableColor;
using mpeg7::ColorLayout;

vector<Feature*> getExistingFeatures(){
  vector<Feature*> features;
  features.push_back(SiftDescriptor::getInstance());
  features.push_back(Histogram::getInstance());
  features.push_back(EdgeHistogram::getInstance());
  features.push_back(ScalableColor::getInstance());
  features.push_back(ColorLayout::getInstance());
  return features;
}

vector<Feature*> getActiveFeatures(){
  vector<Feature*> features = getExistingFeatures();
  vector<Feature*> active_features;
  for(int i = 0; i < (int) features.size(); ++i){
    if(features[i]->isActive())
      active_features.push_back(features[i]);
  }
  return active_features;
}

}}