// Author: songkey
// Date: 2016-04-19
// Email: songkey@outlook.com

#pragma once
#include <string>
#include <fstream>
#include <vector>

namespace sk{

  class binFR
  {
  public:

    explicit binFR(const std::string&);
    ~binFR();

    void close();

    template<typename Dtype>
    binFR& operator>>(std::vector<Dtype> &vec){
      size_t vec_len;
      fr_.read((char*)&vec_len, sizeof(size_t));
      vec.resize(vec_len);
      fr_.read((char*)vec.data(), sizeof(Dtype)* vec_len);

      return *this;
    }

    template<typename Dtype>
    binFR& operator>>(Dtype &item){
      fr_.read((char*)&item, sizeof(Dtype));
      return *this;
    }

    binFR& operator>>(std::string&);

  private:

    binFR();
    std::ifstream fr_;

  };

  class binFW
  {
  public:

    explicit binFW(const std::string&);
    ~binFW();

    void close();
    void flush();

    template<typename Dtype>
    binFW& operator<<(const std::vector<Dtype> &vec){
      size_t vec_len = vec.size();
      fw_.write((char*)&vec_len, sizeof(size_t));
      fw_.write((char*)vec.data(), sizeof(Dtype) * vec_len);

      return *this;
    }

    template<typename Dtype>
    binFW& operator<<(Dtype item){
      fw_.write((char*)&item, sizeof(item));

      return *this;
    }

    binFW& operator<<(const std::string&);
    binFW& operator<<(const char*);

  private:

    binFW();
    std::ofstream fw_;

  };
}
