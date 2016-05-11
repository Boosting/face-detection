// Author: songkey
// Date: 2016-04-26
// Email: songkey@outlook.com

#pragma once
#include "basetype.h"
#include <vector>

namespace sk{

  enum haarType
  {
    haar_x2,
    haar_y2,
    haar_x3,
    haar_y3,
    haar_x4,
    haar_y4,
    haar_point,
    haar_x2_y2
  };

  typedef struct{
    int type;
    int p;
    float th;
    float weight;
    float x, y;
    float width;
    float height;
  } haarClassifier;

  class haarFeatureCalculator
  {
  public:
    haarFeatureCalculator(int width = 0, int height = 0, const unsigned char* imgdata = NULL);
    ~haarFeatureCalculator();

    void setSize(const Size<int>& _sz);
    void setImage(int width, int height, const unsigned char* imgdata);
    void setImage(const unsigned char* imgdata);
    void getFeatureValues(std::vector<float>& _values, const std::vector< Rect<int> >& _rects, int _haar_type);
    void haarFeatureCalculator::detect(const std::vector< std::vector< haarClassifier > >& _hfc, std::vector< Rect<int> >& _res);


    void release();

    friend class haarFunction;


  private:

    int side_max_length_;
    int original_width_, original_height_;
    int resied_width_, resied_height_;
    int step_;

    double calcFeatureValue(const Rect<int>& rect, haarFunction& func);
    float getFeatureValue(const Rect<int>& _rects, int _method);
    bool calcClassifierValue(const Rect<int>& _window,
      const std::vector< std::vector< haarClassifier > >& _hfc);
    std::vector< double > integral_image_;
  };

  class haarFunction
  {
  public:
    virtual double operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect) = 0;
  protected:
    double getval(const haarFeatureCalculator& hfeat, const Point<int>& pt);
  };

  class hf_x2 : public haarFunction
  {
  public:
    double operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect);
  };

  class hf_y2 : public haarFunction
  {
  public:
    double operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect);
  };

  class hf_x3 : public haarFunction
  {
  public:
    double operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect);
  };

  class hf_y3 : public haarFunction
  {
  public:
    double operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect);
  };

  class hf_x4 : public haarFunction
  {
  public:
    double operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect);
  };

  class hf_y4 : public haarFunction
  {
  public:
    double operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect);
  };

  class hf_point : public haarFunction
  {
  public:
    double operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect);
  };

  class hf_x2_y2 : public haarFunction
  {
  public:
    double operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect);
  };
}
