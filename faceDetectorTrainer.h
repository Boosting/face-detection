// Author: songkey
// Date: 2016-04-29
// Email: songkey@outlook.com

#pragma once
#include "haarFeature.h"
#include "basetype.h"

namespace sk{

  class trainParameter{
  public:
    std::string imageListPath;
    std::string imageDir;
    std::string modelPath;
    int stage;
    int iteration;
  };

  class faceDetectorTrainer
  {
  public:
    faceDetectorTrainer();
    ~faceDetectorTrainer();

    void setParameter(const trainParameter& _param);
    void train();
    void saveModel();

  private:

    struct{ 
      int width, height;
      int w_init, h_init;
      int w_step, h_step;
    } rect_parameter_;

    trainParameter param_;

    std::vector< std::vector< int > > y_;
    std::vector< Rect<int> > rects_;

    typedef struct { int type; size_t rect_idx; } m_label_;
    std::vector< std::vector< std::pair< m_label_, std::vector<float> > > > feature_values_;
    std::vector< std::vector< haarClassifier > > learned_classifiers_;

    void generateTrainRects();
    void loadImageAndExtractFeatures();
    void calcFeaturesOfOneImage(int _stage, const unsigned char* _img);

  };

}

