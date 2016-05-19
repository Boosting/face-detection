// Author: songkey
// Date: 2016-05-01
// Email: songkey@outlook.com

#pragma once
#include "basetype.h"
#include "haarFeature.h"
#include <string>
#include <vector>

namespace sk{

  class detectParameter{
  public:

    int width;
    int height;
    std::string modelPath;
  };

  class faceDetector
  {
  public:

    faceDetector();
    ~faceDetector();

    void setParameter(const detectParameter& _param);
    void detect(const unsigned char* _img, std::vector< Rect<int> >& _face_rects);

  private:

    int width_, height_;
    int slid_step_;
    std::string model_path_;
    haarFeatureCalculator hfc_;

    void loadModel();
    void clusterRects(const std::vector< Rect<int> >& _src, std::vector< Rect<int> >& _dst);

    std::vector< stageClassifier > haarclassifiers_;

  };
}
