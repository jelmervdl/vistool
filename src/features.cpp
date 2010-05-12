#include "features.h"

using std::vector;

namespace vito{
namespace features{

vector<Feature*> getExistingFeatures(){
  vector<Feature*> features;
  features.push_back(new SiftDescriptor);
  features.push_back(new Histogram);
  return features;
}

vector<Feature*> getActiveFeatures(){
  vector<Feature*> features = getExistingFeatures();
  vector<Feature*> active_features;
  for(int i = 0; i < (int) features.size(); ++i){
    if(features[i]->isActive())
      active_features.push_back(features[i]);
    else
      delete features[i];
  }
  return active_features;
}

}}
