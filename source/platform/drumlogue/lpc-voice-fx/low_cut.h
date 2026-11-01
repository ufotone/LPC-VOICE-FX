#pragma once
#include <algorithm>
#include <cmath>

// One-pole high-pass on the processed voice, with a smoothed cutoff.
class VoiceLowCut {
 public:
  void reset() {previous_=output_=0;}
  void setCutoff(int hz) {
    hz=std::max(20,std::min(hz,1000));
    target_=std::exp(-6.28318530718f*hz/48000.f);
  }
  float process(float input) {
    coefficient_+=(target_-coefficient_)*(1.f/480.f);
    output_=coefficient_*output_+.5f*(1.f+coefficient_)*(input-previous_);
    previous_=input;
    return output_;
  }
 private:
  float previous_=0,output_=0,coefficient_=.98441476f,target_=.98441476f;
};
