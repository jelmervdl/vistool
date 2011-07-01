#ifndef DATASET_H
#define DATASET_H

#include <map>
#include "core.h"
#include "category.h"
#include "fileManagement.h"
#include "singleton.h"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>


namespace vito{

struct DatasetSpecification{
  std::string name;
  std::string root;
  std::vector<std::string> categories;
};

class Specifications : public Singleton<Specifications>{
private:
  std::map<std::string, DatasetSpecification> readDatasetIndex(std::string str);
public:
  Specifications() :
    datasets(readDatasetIndex("datasets/index.xml")){}
  const std::map<std::string, DatasetSpecification> datasets;
};



class Dataset{
 private:
  std::map<size_t, std::string> category_names;
  std::vector<Category> categories;
  std::string root;

 public:
  //constructor:
  Dataset();
  Dataset(std::string rt);

  // getters:
  std::vector<Category>  *getCategories();
  std::vector<Category*>  getEnabled();

  std::string             getCatName(size_t cat) ;
  std::string             get_root() const ;
  size_t                  smallestCategory() ;

  //setters

  void                    setRoot(std::string str);
  void                    addCategory(Category cat);
  void                    enableCategory(size_t i);
  void                    enableCategory(std::string str);
  void                    disableCategory(size_t i);
  void                    disableCategory(std::string str);
  void                    enableRandom(const int number);
  void                    print();
  void                    shuffle();
  std::vector<Dataset>         split(float ratio = 0.5);
  void                    cut(float ratio, bool second);

  DataPointCollection     enabledPoints(bool eqrep = true) ;
  std::vector<DataPoint*> enabledDataPoints(bool eqrep= true);

  void                    randomDataSplit(std::vector<DataPoint> * train, 
					  std::vector<DataPoint> * test, 
					  float cut = 0.5, 
					  bool eqrep = true,
					  int max = -1);
  static Dataset          createDatasetByName(std::string str);
  static Dataset          createDatasetBySpecification(DatasetSpecification dss);
};
}

#endif
