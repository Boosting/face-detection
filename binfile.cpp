// Author: songkey
// Date: 2016-04-19
// Email: songkey@outlook.com

#include "binfile.h"

namespace sk{

  binFR::binFR(const std::string& filePath){
    fr_.open(filePath.c_str(), std::ios::binary);
    if (!fr_.is_open()) { exit(0); }
  }

  binFR::~binFR()
  {
    if (fr_.is_open()){
      fr_.close();
    }
  }

  void binFR::close(){
    if (fr_.is_open()){
      fr_.close();
    }
  }

  binFR& binFR::operator>>(std::string &str){
    size_t str_len;
    fr_.read((char*)&str_len, sizeof(size_t));
    str.resize(str_len);
    fr_.read((char*)str.data(), str_len);

    return *this;
  }

  binFW::binFW(const std::string& filePath){
    fw_.open(filePath.c_str(), std::ios::binary);
    if (!fw_.is_open()) { exit(0); }
  }

  binFW::~binFW(){
    if (fw_.is_open()){
      fw_.close();
    }
  }

  void binFW::close(){
    if (fw_.is_open()){
      fw_.close();
    }
  }

  void binFW::flush(){
    if (fw_.is_open())
    {
      fw_.flush();
    }
  }

  binFW& binFW::operator<<(const std::string& str){
    rsize_t str_len = str.length();
    fw_.write((char*)&str_len, sizeof(size_t));
    fw_.write((char*)str.data(), str_len);

    return *this;
  }

  binFW& binFW::operator<<(const char* c_str){
    this->operator<<(std::string(c_str));

    return *this;
  }
}
