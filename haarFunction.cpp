// Author: songkey
// Date: 2016-04-26
// Email: songkey@outlook.com

#include "haarFeature.h"

namespace sk{

  double haarFunction::getval(const haarFeatureCalculator& hfeat, const Point<int>& pt)
  {
    int x = pt.x, y = pt.y;
    int step = hfeat.step_;
    int height = hfeat.resied_height_;
    if (x <= 0 || y <= 0) { return 0; }
    if (x >= step && y < height){
      return hfeat.integral_image_.at(y * step + step - 1);
    }
    if (x < step && y >= height){
      return hfeat.integral_image_.at((height - 1) * step + x);
    }
    if (y * step + x >= hfeat.integral_image_.size()) {
      return hfeat.integral_image_.back();
    }
    return hfeat.integral_image_.at(y * step + x);
  }

  double hf_x2::operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect)
  {
    int x = rect.x - 1, y = rect.y - 1;
    int w = rect.width, h = rect.height;
    int h_2 = h / 2;

    double tl = getval(hfeat, Point<int>(x, y));
    double tr = getval(hfeat, Point<int>(x + w, y));
    double ml = getval(hfeat, Point<int>(x, y + h_2));
    double mr = getval(hfeat, Point<int>(x + w, y + h_2));
    double bl = getval(hfeat, Point<int>(x, y + h_2 * 2));
    double br = getval(hfeat, Point<int>(x + w, y + h_2 * 2));

    double s1 = mr + tl - (tr + ml);
    double s2 = br + ml - (mr + bl);

    return (s1 - s2) / (w * h_2);
  }

  double hf_y2::operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect)
  {
    int x = rect.x - 1, y = rect.y - 1;
    int w = rect.width, h = rect.height;
    int w_2 = w / 2;

    double lt = getval(hfeat, Point<int>(x, y));
    double lb = getval(hfeat, Point<int>(x, y + h));
    double mt = getval(hfeat, Point<int>(x + w_2, y));
    double mb = getval(hfeat, Point<int>(x + w_2, y + h));
    double rt = getval(hfeat, Point<int>(x + w_2 * 2, y));
    double rb = getval(hfeat, Point<int>(x + w_2 * 2, y + h));

    double s1 = mb + lt - (mt + lb);
    double s2 = rb + mt - (rt + mb);

    return (s1 - s2) / (w_2 * h);
  }

  double hf_x3::operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect)
  {
    int x = rect.x - 1, y = rect.y - 1;
    int w = rect.width, h = rect.height;
    int h_3 = h / 3;

    double tl = getval(hfeat, Point<int>(x, y));
    double tr = getval(hfeat, Point<int>(x + w, y));
    double mal = getval(hfeat, Point<int>(x, y + h_3));
    double mar = getval(hfeat, Point<int>(x + w, y + h_3));
    double mbl = getval(hfeat, Point<int>(x, y + h_3 * 2));
    double mbr = getval(hfeat, Point<int>(x + w, y + h_3 * 2));
    double bl = getval(hfeat, Point<int>(x, y + h_3 * 3));
    double br = getval(hfeat, Point<int>(x + w, y + h_3 * 3));

    double s1 = mar + tl - (mal + tr);
    double s2 = mbr + mal - (mar + mbl);
    double s3 = br + mbl - (mbr + bl);

    return (s3 + s1 - 2 * s2) / (h_3 * w * 2);
  }

  double hf_y3::operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect)
  {
    int x = rect.x - 1, y = rect.y - 1;
    int w = rect.width, h = rect.height;
    int w_3 = w / 3;

    double lt = getval(hfeat, Point<int>(x, y));
    double lb = getval(hfeat, Point<int>(x, y + h));
    double mat = getval(hfeat, Point<int>(x + w_3, y));
    double mab = getval(hfeat, Point<int>(x + w_3, y + h));
    double mbt = getval(hfeat, Point<int>(x + w_3 * 2, y));
    double mbb = getval(hfeat, Point<int>(x + w_3 * 2, y + h));
    double rt = getval(hfeat, Point<int>(x + w_3 * 3, y));
    double rb = getval(hfeat, Point<int>(x + w_3 * 3, y + h));

    double s1 = mab + lt - (mat + lb);
    double s2 = mbb + mat - (mbt + mab);
    double s3 = rb + mbt - (rt + mbb);

    return (s1 + s3 - 2 * s2) / (h * w_3 * 2);
  }

  double hf_x4::operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect)
  {
    int x = rect.x - 1, y = rect.y - 1;
    int w = rect.width, h = rect.height;
    int h_4 = h / 4;

    double tl = getval(hfeat, Point<int>(x, y));
    double tr = getval(hfeat, Point<int>(x + w, y));
    double mal = getval(hfeat, Point<int>(x, y + h_4));
    double mar = getval(hfeat, Point<int>(x + w, y + h_4));
    double mbl = getval(hfeat, Point<int>(x, y + h_4 * 3));
    double mbr = getval(hfeat, Point<int>(x + w, y + h_4 * 3));
    double bl = getval(hfeat, Point<int>(x, y + h_4 * 4));
    double br = getval(hfeat, Point<int>(x + w, y + h_4 * 4));

    double s1 = mar + tl - (mal + tr);
    double s2 = mbr + mal - (mar + mbl);
    double s3 = br + mbl - (mbr + bl);

    return (s1 + s3 - s2) / (w * h_4 * 2);
  }

  double hf_y4::operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect)
  {
    int x = rect.x - 1, y = rect.y - 1;
    int w = rect.width, h = rect.height;
    int w_4 = w / 4;

    double lt = getval(hfeat, Point<int>(x, y));
    double lb = getval(hfeat, Point<int>(x, y + h));
    double mat = getval(hfeat, Point<int>(x + w_4, y));
    double mab = getval(hfeat, Point<int>(x + w_4, y + h));
    double mbt = getval(hfeat, Point<int>(x + w_4 * 3, y));
    double mbb = getval(hfeat, Point<int>(x + w_4 * 3, y + h));
    double rt = getval(hfeat, Point<int>(x + w_4 * 4, y));
    double rb = getval(hfeat, Point<int>(x + w_4 * 4, y + h));

    double s1 = mab + lt - (mat + lb);
    double s2 = mbb + mat - (mbt + mab);
    double s3 = rb + mbt - (rt + mbb);

    return (s1 + s3 - s2) / ( h * w_4 * 2);
  }

  double hf_point::operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect)
  {
    int x = rect.x - 1, y = rect.y - 1;
    int w = rect.width, h = rect.height;
    int w_3 = w / 3, h_3 = h / 3;

    double tl = getval(hfeat, Point<int>(x, y));
    double tr = getval(hfeat, Point<int>(x + (w_3) * 3, y));
    double bl = getval(hfeat, Point<int>(x, y + (h_3) * 3));
    double br = getval(hfeat, Point<int>(x + (w_3) * 3, y + (h_3) * 3));

    double mtl = getval(hfeat, Point<int>(x + w_3, y + h_3));
    double mtr = getval(hfeat, Point<int>(x + (w_3) * 2, y + h_3));
    double mbl = getval(hfeat, Point<int>(x + w_3, y + (h_3) * 2));
    double mbr = getval(hfeat, Point<int>(x + (w_3) * 2, y + (h_3) * 2));

    double s1 = br + tl - (tr + bl);
    double s2 = mbr + mtl - (mtr + mbl);

    return (s1 - 9 * s2) / (w_3 * h_3 * 9);
  }

  double hf_x2_y2::operator()(const haarFeatureCalculator& hfeat, const Rect<int>& rect)
  {
    int x = rect.x - 1, y = rect.y - 1;
    int w = rect.width, h = rect.height;
    int w_2 = w / 2, h_2 = h / 2;

    double tl = getval(hfeat, Point<int>(x, y));
    double tm = getval(hfeat, Point<int>(x + w_2, y));
    double tr = getval(hfeat, Point<int>(x + (w_2) * 2, y));

    double ml = getval(hfeat, Point<int>(x, y + h / 2));
    double mm = getval(hfeat, Point<int>(x + w_2, y + h / 2));
    double mr = getval(hfeat, Point<int>(x + (w_2) * 2, y + h / 2));

    double bl = getval(hfeat, Point<int>(x, y + (h / 2) * 2));
    double bm = getval(hfeat, Point<int>(x + w_2, y + (h / 2) * 2));
    double br = getval(hfeat, Point<int>(x + (w_2) * 2, y + (h / 2) * 2));

    double s1 = br + tl - (tr + bl);
    double s2 = mm + tl - (tm + ml);
    double s3 = br + mm - (mr + bm);

    return (s1 - 2 * (s3 + s2)) / (w_2 * h_2 * 4);
  }

}
