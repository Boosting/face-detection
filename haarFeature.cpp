// Author: songkey
// Date: 2016-04-26
// Email: songkey@outlook.com

#include "haarFeature.h"

namespace sk{

  haarFeatureCalculator::haarFeatureCalculator(
    int width, int height, const unsigned char* imgdata)
    :original_width_(width), original_height_(height)
  {
    setImage(width, height, imgdata);
  }

  void haarFeatureCalculator::setImage(
    int width, int height, const unsigned char* imgdata)
  {
    side_max_length_ = 1000;
    original_width_ = width;
    original_height_ = height;

    if (!imgdata) { return; }

    release();

    double EX = 0, EX_2 = 0;
    for (int i = 0; i < width * height; i++)
    {
      float pixval = imgdata[i];
      EX += pixval;
      EX_2 += pixval * pixval;
    }
    EX /= (width * height);
    EX_2 /= (width * height);
    double STD = std::sqrt(EX_2 - EX * EX);

    int longer_side = original_width_;
    if (original_height_ > longer_side) { longer_side = original_height_; }

    int exp = 1;
    if (longer_side > side_max_length_){
      while (longer_side / exp > side_max_length_) { exp <<= 1; }
    }

    resied_width_ = original_width_ / exp;
    resied_height_ = original_height_ / exp;

    step_ = resied_width_;

    integral_image_.resize((step_ + 1) * (resied_height_ + 1), 0);

    for (size_t i = 0; i < resied_height_; i++)
    {
      for (size_t j = 0; j < resied_width_; j++)
      {
        double ava;
        if (exp == 1){
          ava = static_cast<double>(imgdata[i* original_width_ + j]);
          //ava = (static_cast<double>(imgdata[i* original_width_ + j]));
        }
        else
        {
          int sum = 0;
          for (size_t m = 0; m < exp; m++)
          {
            for (size_t n = 0; n < exp; n++)
            {
              sum += imgdata[i * exp * original_width_ + j * exp + m * exp + n];
              //sum += (imgdata[i * exp * original_width_ + j * exp + m * exp + n]);
            }
          }
          ava = static_cast<double>(sum) / (exp * exp);
        }

        ava = (ava - EX) / STD;

        //resized_pixels_.at(i * step_ + j) = static_cast<int>(ava);
        if (i == 0 && j == 0) { integral_image_.at(i * step_ + j) = ava; }
        else if (i == 0) { integral_image_.at(j) = ava + integral_image_.at(j - 1); }
        else if (j == 0) { integral_image_.at(i * step_) = ava + integral_image_.at(i * step_ - step_); }
        else {
          integral_image_.at(i * step_ + j) = ava + integral_image_.at(i * step_ + j) +
            integral_image_.at(i * step_ - step_ + j) +
            integral_image_.at(i * step_ + j - 1) -
            integral_image_.at(i * step_ - step_ + j - 1);
        }
      }
    }
  }

  void haarFeatureCalculator::detect(const std::vector< stageClassifier >& _hfc , 
    std::vector< Rect<int> >& _res)
  {
    const std::vector<int> window_sizes_ = { 24, 28, 33, 39,
      46, 55, 66, 79, 94, 112, 134, 160, 192, 230, 276, 331,
      397, 476, 571, 685, 822, 986 };
    
    int small_side = resied_width_;
    if (small_side > resied_height_) { small_side = resied_height_; }

    size_t max_side_idx = 0;
    while (small_side > window_sizes_.at(max_side_idx++));

    if (max_side_idx == 0) { return; }

    for (int idx = 0; idx < max_side_idx; idx++)
    {

        int w_margin = resied_width_ - window_sizes_.at(idx);
        int h_margin = resied_height_ - window_sizes_.at(idx);

        for (int x = 0; x <= w_margin; x += 4)
        {
          for (int y = 0; y <= h_margin; y += 4)
          {
            Rect<int> tmp_wd(x, y, window_sizes_.at(idx), window_sizes_.at(idx));
            if (calcClassifierValue(tmp_wd, _hfc)){
              if (original_width_ != resied_width_){
                tmp_wd *= original_width_;
                tmp_wd /= resied_width_;
              }
              _res.push_back(tmp_wd);
            }
          }
        }
    }
  }

  bool haarFeatureCalculator::calcClassifierValue(
    const Rect<int>& _window,
    const std::vector< stageClassifier >& _hfc)
  {
    for (size_t stage = 0; stage < _hfc.size(); stage++)
    {
      double sum = 0;
      for (size_t f = 0; f < _hfc.at(stage).classifiers.size(); f++)
      {
        Rect<int> detect_wd(_hfc.at(stage).classifiers.at(f).x * _window.width,
          _hfc.at(stage).classifiers.at(f).y * _window.height,
          _hfc.at(stage).classifiers.at(f).width * _window.width,
          _hfc.at(stage).classifiers.at(f).height * _window.height);
        detect_wd.x += _window.x;
        detect_wd.y += _window.y;

        int p = _hfc.at(stage).classifiers.at(f).p;
        int type = _hfc.at(stage).classifiers.at(f).type;
        double fx = p * getFeatureValue(detect_wd, type);
        float th = p * _hfc.at(stage).classifiers.at(f).th;
        float alpha = _hfc.at(stage).classifiers.at(f).weight;
        if (fx > th) { sum += alpha; }
        else { sum -= alpha; }
      }
      if (sum <= _hfc.at(stage).weight) return false;
    }
    return true;
  }

  void haarFeatureCalculator::setImage(
    const unsigned char* _img)
  {
    setImage(original_width_, original_height_, _img);
  }

  float haarFeatureCalculator::getFeatureValue(
    const Rect<int>& _rect, int _method)
  {
    switch (_method)
    {
    case haar_x2:
      return calcFeatureValue(_rect, hf_x2());
    case haar_y2:
      return calcFeatureValue(_rect, hf_y2());
    case haar_x3:
      return calcFeatureValue(_rect, hf_x3());
    case haar_y3:
      return calcFeatureValue(_rect, hf_y3());
    case haar_x4:
      return calcFeatureValue(_rect, hf_x4());
    case haar_y4:
      return calcFeatureValue(_rect, hf_y4());
    case haar_point:
      return calcFeatureValue(_rect, hf_point());
    case haar_x2_y2:
      return calcFeatureValue(_rect, hf_x2_y2());
    default:
      return 0;
    }
  }

  void haarFeatureCalculator::getFeatureValues(
    std::vector<float>& _values, 
    const std::vector< Rect<int> >& _rects, int _haar_type)
  {

    size_t itemCounts = _rects.size();
    _values.resize(itemCounts);

    for (size_t i = 0; i < itemCounts; i++)
    {
      _values.at(i) = getFeatureValue(_rects.at(i), _haar_type);
    }
  }

  double haarFeatureCalculator::calcFeatureValue(
    const Rect<int>& _rect, haarFunction& _func)
  {
    return _func(*this, _rect);
  }

  void haarFeatureCalculator::release(){
    std::vector<double> v1;
    v1.swap(integral_image_);
  }

  haarFeatureCalculator::~haarFeatureCalculator()
  {
  }

}
