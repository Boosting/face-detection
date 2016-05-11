// Author: songkey
// Date: 2016-04-26
// Email: songkey@outlook.com

#pragma once
#include <algorithm>

namespace sk{


  template <typename Dtype>
  class Point{
  public:
    Dtype x, y;
    Point();
    Point(Dtype _x = 0, Dtype _y = 0) : x(_x), y(_y) {}
  };

  template <typename Dtype>
  class Size{
  public:
    Dtype width, height;
    Size(Dtype _w = 0, Dtype _h = 0) : width(_w), height(_h) {}
  };

  template <typename Dtype>
  class Rect{
  public:
    Dtype x, y;
    Dtype width, height;

    Rect(Dtype _x = 0, Dtype _y = 0, Dtype _w = 0, Dtype _h = 0) 
      : x(_x), y(_y), width(_w), height(_h) {}

    Dtype area() const
    {
      return width * height;
    }

    bool contain(const Rect& _rhs, float _margin) const
    {
      Dtype dx = width * _margin, dy = height * _margin;
      return (_rhs.x + dx >= x) && (_rhs.y + dy >= y) &&
        (_rhs.x + _rhs.width <= x + width + dx) &&
        (_rhs.y + _rhs.height <= y + height + dy);
    }


    static bool isOverlap(const Rect& _lhs, const Rect& _rhs)
    {
      return !(_lhs.x > (_rhs.x + _rhs.width) || _lhs.y > (_rhs.y + _rhs.height) ||
        _rhs.x > (_lhs.x + _lhs.width) || _rhs.y > (_lhs.y + _lhs.height));
    }

    static bool isSimilar(const Rect& _lhs, const Rect& _rhs, float _th)
    {
      _th *= (std::min(_lhs.width, _rhs.width) + std::min(_lhs.height, _rhs.height)) * 0.5;
      return std::abs(static_cast<float>(_lhs.x - _rhs.x)) <= _th &&
        std::abs(static_cast<float>(_lhs.y - _rhs.y)) <= _th &&
        std::abs(static_cast<float>(_lhs.x + _lhs.width - _rhs.x - _rhs.width)) <= _th &&
        std::abs(static_cast<float>(_lhs.y + _lhs.height - _rhs.y - _rhs.height)) <= _th;
    }

    static Dtype intersectArea(const Rect& _lhs, const Rect& _rhs)
    {
      if (isOverlap(_lhs, _rhs)) {
        Dtype l = _lhs.x > _rhs.x ? _lhs.x : _rhs.x;
        Dtype t = _lhs.y > _rhs.y ? _lhs.y : _rhs.y;
        Dtype r = (_lhs.x + _lhs.width) < (_rhs.x + _rhs.width) ? (_lhs.x + _lhs.width) : (_rhs.x + _rhs.width);
        Dtype b = (_lhs.y + _lhs.height) < (_rhs.y + _rhs.height) ? (_lhs.y + _lhs.height) : (_rhs.y + _rhs.height);

        return(r - l)*(t - b);
      }
      return 0;
    }

    static Dtype commonArea(const Rect& _lhs, const Rect& _rhs)
    {
      if (isOverlap(_lhs, _rhs)){
        return _lhs.area() + rhs.area() - intersectArea(_lhs, _rhs)
      }
      return 0;
    }

    bool operator == (const Rect& _rhs) const
    {
      if (x != _rhs.x || y != _rhs.y || width != _rhs.width || 
        height != _rhs.height)
      {
        return false;
      }
      return true;
    }

    void operator += (const Rect& _rhs)
    {
      x += _rhs.x; y += _rhs.y; width += _rhs.width; height += _rhs.height;
      return;
    }

    void operator /= (Dtype _val)
    {
      if (_val == 0) { exit(0); }
      x /= _val; y /= _val; width /= _val; height /= _val;
      return;
    }

    void operator *= (Dtype _val)
    {
      x *= _val; y *= _val; width *= _val; height *= _val;
      return;
    }

  };
}
