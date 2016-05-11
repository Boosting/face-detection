// Author: songkey
// Date: 2016-05-01
// Email: songkey@outlook.com

#include "faceDetector.h"
#include "binfile.h"

namespace sk{
  faceDetector::faceDetector()
  {
    slid_step_ = 5;
  }

  faceDetector::~faceDetector()
  {
  }

  void faceDetector::loadModel(){
    binFR bfr(model_path_);
    size_t stage_num;
    bfr >> stage_num;
    for (size_t i = 0; i < stage_num; i++)
    {
      std::vector< haarClassifier > tmp_classifier;
      bfr >> tmp_classifier;
      haarclassifiers_.push_back(tmp_classifier);
    }
  }

  void faceDetector::setParameter(const detectParameter& _param)
  {
    if (_param.width <= 24 || _param.height <= 24){
      width_ = 0;
      height_ = 0;
    }
    if (_param.width != width_ || _param.height != height_){
      width_ = _param.width;
      height_ = _param.height;
    }
    if (model_path_.compare(_param.modelPath)){
      model_path_ = _param.modelPath;
      loadModel();
    }
  }

  void faceDetector::detect(const unsigned char* _img, 
    std::vector< Rect<int> >& _face_rects)
  {
    if (haarclassifiers_.size() == 0) {
      return; 
    }
    hfc_.setImage(width_, height_, _img);

    std::vector< Rect<int> > tmp_rects;
    //hfc_.detect(haarclassifiers_, _face_rects);
    hfc_.detect(haarclassifiers_, tmp_rects);
    clusterRects(tmp_rects, _face_rects);
  }

  void faceDetector::clusterRects(const std::vector< Rect<int> >& _src, 
    std::vector< Rect<int> >& _dst)
  {
    if (_src.empty()){ return; }

    // The algorithm is described in "Introduction to Algorithms"
    // by Cormen, Leiserson and Rivest, the chapter "Data structures for disjoint sets"

    size_t rects_num = _src.size();

    typedef struct { int rank, parent; } node;
    std::vector<node> nodes(rects_num, { 0, -1 });

    // The main O(N^2) pass: merge connected components
    for (size_t i = 0; i < rects_num; i++)
    {
      // find root
      int root1 = i;
      while (nodes.at(root1).parent >= 0) { root1 = nodes.at(root1).parent; }
      
      for (size_t j = 0; j < rects_num; j++)
      {
        if (i == j || !Rect<int>::isSimilar(_src.at(i), _src.at(j), 0.2)) { continue; }

        int root2 = j;
        while (nodes.at(root2).parent >= 0) { root2 = nodes.at(root2).parent; }

        if (root1 == root2) { continue; }

        // unite both trees
        int rank1 = nodes.at(root1).rank;
        int rank2 = nodes.at(root2).rank;
        if (rank1 > rank2) { nodes.at(root2).parent = root1; }
        else {
          nodes.at(root1).parent = root2;
          nodes.at(root2).rank += (rank1 == rank2);
          root1 = root2;
        }
        //if (nodes.at(root).parent >= 0) { exit(0); }

        // compress the path from node to root
        int k = j, k_parent;
        while ((k_parent = nodes.at(k).parent) >= 0)
        {
          nodes.at(k).parent = root1;
          k = k_parent;
        }

        k = i;
        while ((k_parent = nodes.at(k).parent) >= 0)
        {
          nodes.at(k).parent = root1;
          k = k_parent;
        }

      } // for j

    } // for i

    // Final O(N) pass: enumerate classes
    std::vector<int> labels(rects_num, 0);
    int class_num = 0;

    for (size_t i = 0; i < rects_num; i++)
    {
      int root = i;
      while (nodes.at(root).parent >= 0) { root = nodes.at(root).parent; }
      // re-use the rank as the class label
      if (nodes.at(root).rank >= 0) { nodes.at(root).rank = ~class_num++; }
      labels.at(i) = ~nodes.at(root).rank;
    }

    std::vector< Rect<int> > tmp_rects(class_num, { 0, 0, 0, 0 });
    std::vector< int > tmp_weights(class_num, 0);
    for (size_t i = 0; i < rects_num; i++)
    {
      tmp_rects.at(labels.at(i)) += _src.at(i);
      tmp_weights.at(labels.at(i))++;
    }

    for (int i = 0; i < class_num; i++)
    {
      tmp_rects.at(i) /= tmp_weights.at(i);
    }

    for (int i = 0; i < class_num; i++)
    {
      int j;
      for (j = 0; j < class_num; j++)
      {
        if (j != i && tmp_weights.at(j) < 4 && tmp_rects.at(i).contain(tmp_rects.at(j), 0.2))
        {
          break;
        }
      }
      if (j == class_num && tmp_weights.at(i) > 1){
        _dst.push_back(tmp_rects.at(i));
      }
    }

  }
}
