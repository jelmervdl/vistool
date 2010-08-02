#include "classifier.h"

using std::vector;
using std::cout;
using std::endl;

namespace vito{
namespace classification{

LabelCollection Classifier::classify(const DescriptorCollection &descriptors){
  cout << "attempting to create a label collection" << endl;
  LabelCollection labels;
  cout << descriptors[0].size() << "is the descriptor coll size " << endl;
  labels.resize(descriptors.size());
  cout << "created a label collection" << endl;
  for(size_t i = 0; i < descriptors.size(); ++i){
    cout << "at i: " << i << endl;
    labels[i] = classify(descriptors[i]);
  }
  return labels;
}


LabelCollection Classifier::crossvalidation(const ExampleCollection &files){
  // ready CrossValidation Parameters
  const int kFolds = 
    Parameters::getInstance()->getiParameter("cross_validation_k");
  const int kFiles = files.size();
  assert(kFolds > 1);

  // fill in results of training/classification on k subgroups
  LabelCollection results(kFiles);
  const float bin_size = kFiles / (float) kFolds;
  for(int subgroup = 0; subgroup < kFolds; ++subgroup){
    cout << "at subgroup " << subgroup << endl;
    ExampleCollection training_set;
    ExampleCollection test_set;
    const int bin_min = (int) ((bin_size * subgroup) + 0.5);
    const int bin_max = (int) ((bin_size * (subgroup + 1) ) + 0.5);

    //fill training and test sets:
    cout << "before filling" << endl;
    for(int i = 0; i < kFiles; ++i)
      if(i >= bin_min && i < bin_max )
	test_set.push_back(files[i]);
      else
	training_set.push_back(files[i]);

    // train on subgroup
    cout << "crossvalidation is training on " << training_set.size() 
	 << " items"  << endl;
    
    train(training_set); 
    DescriptorCollection test_set_descriptor = (DescriptorCollection) test_set;
    // classify test set
    cout << "before classifying" << endl;
    LabelCollection bin_results = classify(test_set_descriptor); 

    // fill in results of last classification
    for(int i = bin_min; i < bin_max; ++i)
      results[i] = bin_results[i - bin_min];
  }
  return results;
}

bool Classifier::single_class(){
  return false;
}

}}
