// Author: songkey
// Date: 2016-04-29
// Email: songkey@outlook.com

#include "faceDetectorTrainer.h"
#include "binfile.h"
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>

namespace sk{

  faceDetectorTrainer::faceDetectorTrainer()
  {
    rect_parameter_.width = 72;
    rect_parameter_.height = 72;
    rect_parameter_.w_init = 6;
    rect_parameter_.w_step = 6;
    rect_parameter_.h_init = 6;
    rect_parameter_.h_step = 6;

    generateTrainRects();
  }

  faceDetectorTrainer::~faceDetectorTrainer()
  {
  }

  void faceDetectorTrainer::setParameter(const trainParameter& _param)
  {
    param_ = _param;
  }

  void faceDetectorTrainer::saveModel()
  {
    if (learned_classifiers_.size() == 0)
    {
      return;
    }

    binFW bfw(param_.modelPath);
    size_t stage_num = learned_classifiers_.size();
    bfw << stage_num;
    for (size_t i = 0; i < stage_num; i++)
    {
      bfw << learned_classifiers_.at(i).weight;
      bfw << learned_classifiers_.at(i).classifiers;
    }
  }
  
  typedef struct{ size_t idx; float val; } idx_fval;
  bool comp(const idx_fval& lhs, const idx_fval& rhs)
  {
    return lhs.val < rhs.val;
  }

  void faceDetectorTrainer::train()
  {
    loadImageAndExtractFeatures();

    std::cout << "\n training\n**************\n" << std::endl;

    std::set<size_t> learned_f;

    for (int stage = 0; stage < param_.stage; stage++)
    {
      size_t feature_num = feature_values_.at(stage).size();
      size_t sample_num = y_.at(stage).size();

      typedef struct { double Sp, Sn; } Sp_Sn;

      stageClassifier stage_classifies;
      std::vector< int > selected_funcs;
      std::vector< double > W(sample_num, 1. / sample_num);

      int iteration = param_.iteration;
      iteration = iteration * (stage + 1) / param_.stage;
      if (iteration < 1) { iteration = 1; }
      for (int iter = 0; iter < iteration; iter++)
      {
        class weekClassifierParam { public: float e; float th; int p; size_t f; };
        weekClassifierParam best_param = { 10., 10, 0, 0 };
        for (size_t f = 0; f < feature_num; f++)
        {
          if (learned_f.find(f) != learned_f.end()){ continue; }
          std::vector< idx_fval > sorted_val(sample_num);
          for (size_t i = 0; i < sample_num; i++)
          {
            sorted_val.at(i).idx = i;
            sorted_val.at(i).val = feature_values_.at(stage).at(f).second.at(i);
          }

          // Ascending sort
          std::sort(sorted_val.begin(), sorted_val.end(), comp);

          // calc S+ list and S- list

          std::vector< Sp_Sn > Sp_Sn_list(sample_num);
          if (y_.at(stage).at(sorted_val.at(0).idx) > 0) {
            Sp_Sn_list.at(0).Sp = W.at(sorted_val.at(0).idx);
            Sp_Sn_list.at(0).Sn = 0;
          }
          else
          {
            Sp_Sn_list.at(0).Sp = 0;
            Sp_Sn_list.at(0).Sn = W.at(sorted_val.at(0).idx);
          }
          for (size_t i = 1; i < sample_num; i++) {
            if (y_.at(stage).at(sorted_val.at(i).idx) > 0) {
              Sp_Sn_list.at(i).Sp = W.at(sorted_val.at(i).idx) + Sp_Sn_list.at(i - 1).Sp;
              Sp_Sn_list.at(i).Sn = Sp_Sn_list.at(i - 1).Sn;
            }
            else
            {
              Sp_Sn_list.at(i).Sp = Sp_Sn_list.at(i - 1).Sp;
              Sp_Sn_list.at(i).Sn = W.at(sorted_val.at(i).idx) + Sp_Sn_list.at(i - 1).Sn;
            }
          }
          // T+ and T-
          double Tp = Sp_Sn_list.back().Sp;
          double Tn = Sp_Sn_list.back().Sn;

          // calc the best threshold of the (f)th classifier

          weekClassifierParam min_param = {100., 0, 0, 0};
          for (size_t i = 1; i < sample_num; i++) {

            // left positive and right negative
            float tmp_e1 = Sp_Sn_list.at(i).Sp + Tn - Sp_Sn_list.at(i).Sn;

            // left negative and right positive
            float tmp_e2 = Sp_Sn_list.at(i).Sn + Tp - Sp_Sn_list.at(i).Sp;

            if (std::min(tmp_e1, tmp_e2) < min_param.e){
              if (tmp_e1 > tmp_e2){
                min_param.e = tmp_e2;
                min_param.p = -1;
                min_param.th = sorted_val.at(i).val;
              }
              else {
                min_param.e = tmp_e1;
                min_param.p = 1;
                min_param.th = sorted_val.at(i).val;
              }
            }
          } // i
          if (min_param.e < best_param.e) { 
            best_param = min_param;
            best_param.f = f;

            std::cout << "-> " << "(s" << stage << "|i" << iter << "|f" << f << ") p: "
              << best_param.p << " th: " << best_param.th << " e: " << best_param.e
              << " type: " << feature_values_.at(stage).at(f).first.type << std::endl;
          }

        } // feature

        float alpha = std::log((1 - best_param.e) / best_param.e) * 0.5;

        // get a classifier

        learned_f.insert(best_param.f);

        haarClassifier tmp_classifier;
        size_t rect_index = feature_values_.at(stage).at(best_param.f).first.rect_idx;
        tmp_classifier.x = rects_.at(rect_index).x / static_cast<float>(rect_parameter_.width);
        tmp_classifier.y = rects_.at(rect_index).y / static_cast<float>(rect_parameter_.height);
        tmp_classifier.width = rects_.at(rect_index).width / static_cast<float>(rect_parameter_.width);
        tmp_classifier.height = rects_.at(rect_index).height / static_cast<float>(rect_parameter_.height);
        tmp_classifier.p = best_param.p;
        tmp_classifier.th = best_param.th;
        tmp_classifier.type = feature_values_.at(stage).at(best_param.f).first.type;
        tmp_classifier.weight = alpha;
        stage_classifies.classifiers.push_back(tmp_classifier);
        selected_funcs.push_back(best_param.f);

        // update weights

        double Zm = 0;

        for (size_t i = 0; i < sample_num; i++)
        {
          int Gx = 1;
          if (feature_values_.at(stage).at(best_param.f).second.at(i) <= best_param.th) { Gx = -1; }
          W.at(i) *= std::exp(-alpha * y_.at(stage).at(i) * Gx * best_param.p);
          Zm += W.at(i);
        }
        for (size_t i = 0; i < sample_num; i++) { W.at(i) /= Zm; }

        std::cout << "---> " << "(s" << stage << "|i" << iter << ") f: "  << best_param.f 
          << " p: " << best_param.p << " th: " << best_param.th << " e: " << best_param.e 
          << " type: " << tmp_classifier.type << " alpha: " << tmp_classifier.weight << std::endl;
      } // iteration

      learned_classifiers_.push_back(stage_classifies);
      std::vector<idx_fval> tmp_val(sample_num, { 0, 0 });
      for (size_t i = 0; i < sample_num; i++)
      {
        tmp_val.at(i).idx = i;
      }
      for (size_t c = 0; c < selected_funcs.size(); c++)
      {
        for (size_t sn = 0; sn < sample_num; sn++)
        {
          double fx = feature_values_.at(stage).at(selected_funcs[c]).second.at(sn) *
            stage_classifies.classifiers.at(c).p;
          double th = stage_classifies.classifiers.at(c).p * stage_classifies.classifiers.at(c).th;
          if (fx > th){
            tmp_val.at(sn).val += stage_classifies.classifiers.at(c).weight;
          }
          else {
            tmp_val.at(sn).val -= stage_classifies.classifiers.at(c).weight;
          }
        }
      }
      std::sort(tmp_val.begin(), tmp_val.end(), comp);
      std::vector<Sp_Sn> spsn(sample_num, {0, 0});
      if (y_.at(stage).at(tmp_val.front().idx) > 0){
        spsn.front().Sp = 1;
      }
      else
      {
        spsn.front().Sn = 1;
      }
      for (size_t sn = 0; sn < sample_num; sn++)
      {
        if (y_.at(stage).at(tmp_val.at(sn).idx) > 0){
          spsn.at(sn).Sp = spsn.at(sn - 1).Sp + 1;
          spsn.at(sn).Sn = spsn.at(sn - 1).Sn;
        }
        else
        {
          spsn.at(sn).Sp = spsn.at(sn - 1).Sp;
          spsn.at(sn).Sn = spsn.at(sn - 1).Sn + 1;
        }
      }
      int Tp = spsn.back().Sp;
      int Tn = spsn.back().Sn;
      int best_sn = 0;
      for (size_t sn = 0; sn < sample_num; sn++)
      {
        if (spsn.at(sn).Sp + Tn - spsn.at(sn).Sn <
          spsn.at(best_sn).Sp + Tn - spsn.at(best_sn).Sn)
        {
          best_sn = sn;
        }
      }
      stage_classifies.weight = tmp_val.at(best_sn).val;

    } // stage
    std::cout << std::endl;

  }

  void faceDetectorTrainer::loadImageAndExtractFeatures()
  {

    typedef struct{ std::string filename; int label; } data_elem;
    std::vector< data_elem > data_list;

    std::ifstream fr(param_.imageListPath.c_str());
    if (!fr.is_open()){
      std::cout << "\nERROR: image list error!\n---> " <<
        param_.imageListPath << std::endl;
      exit(0);
    }

    std::string imageName;
    int label;
    while (fr >> imageName >> label)
    {
      if (label > 0) { label = 1; }
      else { label = -1; }
      data_list.push_back({ imageName, label });
    }
    fr.close();

    std::random_shuffle(data_list.begin(), data_list.end());
    std::random_shuffle(data_list.begin(), data_list.end());
    std::random_shuffle(data_list.begin(), data_list.end());

    feature_values_.resize(param_.stage);
    y_.resize(param_.stage);

    int sample_num = data_list.size();
    int stage_num = param_.stage;
    int stage_sample_num = sample_num / std::pow(2, stage_num);
    int stage_sample_start = 0;
    for (int j = 0; j < stage_num; j++)
    {
      feature_values_.at(j).resize(8 * rects_.size());
      int end = stage_sample_start + stage_sample_num;
      if (j == stage_num - 1) { end = sample_num; }

      for (size_t i = stage_sample_start; i < end; i++)
      {
        //std::string imagePath = param_.imageDir + "/" + data_list.at(i).filename;
        std::string imagePath = data_list.at(i).filename;
        cv::Mat img = cv::imread(imagePath.c_str());
        if (img.empty()) {
          std::cout << "\nWARRING: image path error!\n---> " <<
            imagePath << "\n";
          continue;
        }

        if (img.channels() == 3){
          cv::cvtColor(img, img, CV_BGR2GRAY);
        }
        if (img.cols != rect_parameter_.width || img.rows != rect_parameter_.height) {
          cv::resize(img, img, cv::Size(rect_parameter_.width, rect_parameter_.height));
        }

        std::cout << "---> (s" << j << "|" << i << ")" << data_list.at(i).filename << " " 
          << data_list.at(i).label << std::endl;
        calcFeaturesOfOneImage(j, img.data);

        y_.at(j).push_back(data_list.at(i).label);

        if (y_.at(j).back() > 0){
          cv::imshow("pos", img);
        }
        else{
          cv::imshow("neg", img);
        }
        cv::waitKey(10);
      } // for i
      stage_sample_start += stage_sample_num;
      stage_sample_num *= 2;
    }
  }

  void faceDetectorTrainer::calcFeaturesOfOneImage(
    int _stage, const unsigned char* _img)
  {
    haarFeatureCalculator hfc(rect_parameter_.height, rect_parameter_.width, _img);
    for (int type = haar_x2; type <= haar_x2_y2; type++)
    {
      std::vector<float> tmpFeatVal;
      hfc.getFeatureValues(tmpFeatVal, rects_, type);

      for (size_t i = 0; i < rects_.size(); i++)
      {
        feature_values_.at(_stage).at(type * rects_.size() + i).first.type = type;
        feature_values_.at(_stage).at(type * rects_.size() + i).first.rect_idx = i;
        feature_values_.at(_stage).at(type * rects_.size() + i).second.push_back(tmpFeatVal.at(i));
      }
    }
  }

  void faceDetectorTrainer::generateTrainRects()
  {
    rects_.resize(0);

    int width = rect_parameter_.width;
    int height = rect_parameter_.height;
    int w_init = rect_parameter_.w_init;
    int w_step = rect_parameter_.w_step;
    int h_init = rect_parameter_.h_init;
    int h_step = rect_parameter_.h_step;

    for (int w = w_init; w <= width; w += w_step){
      for (int h = h_init; h <= height; h += h_step){
        int x_margin = width - w;
        int y_margin = height - h;
        for (int x = 0; x <= x_margin; x += w_step){
          for (int y = 0; y <= y_margin; y += h_step)
          {
            rects_.push_back(Rect<int>(x, y, w, h));
          }
        }
      }
    }
  }

}
