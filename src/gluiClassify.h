#ifndef GLUICLASSIFY_H
#define GLUICLASSIFY_H

#include "toolState.h"
#include "gluiFeatures.h"
#include "nnclassifier.h"
#include "svmclassifier.h"
#include "gluiView.h"
#include "gluiEvaluate.h"
#include "classification.h"

namespace vito{
namespace gui{

void train();
void classify();
void non_set_classify();

}}

#endif
