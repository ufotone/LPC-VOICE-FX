#pragma once
#include <algorithm>
#include <cmath>

// Fixed-memory stereo feedback reverb for the processed voice.
class VoiceReverb {
 public:
  void reset() {
    for(int i=0;i<8;++i){pos_[i]=valid_[i]=0;damped_[i]=0;}
    initialized_=false;
  }
  void controls(int mix,int timeMs) {
    target_mix_=std::max(0,std::min(mix,100))*.01f;
    int boundedTime=std::max(200,std::min(timeMs,4000));
    if(boundedTime!=last_time_) {
      float seconds=boundedTime*.001f;
      for(int i=0;i<8;++i)target_feedback_[i]=std::pow(.001f,lengths_[i]/(48000.f*seconds));
      last_time_=boundedTime;
    }
    if(!initialized_){mix_=target_mix_;for(int i=0;i<8;++i)feedback_[i]=target_feedback_[i];initialized_=true;}
  }
  void process(float input,float& left,float& right) {
    mix_+=(target_mix_-mix_)*(1.f/960.f);
    float sumL=0,sumR=0;
    for(int i=0;i<8;++i) {
      feedback_[i]+=(target_feedback_[i]-feedback_[i])*(1.f/960.f);
      float delayed=valid_[i]>=lengths_[i]?buffer_[i][pos_[i]]:0.f;
      damped_[i]+=(delayed-damped_[i])*.35f;
      buffer_[i][pos_[i]]=input+feedback_[i]*damped_[i];
      if(++pos_[i]>=lengths_[i])pos_[i]=0;
      if(valid_[i]<lengths_[i])++valid_[i];
      if(i<4)sumL+=delayed;else sumR+=delayed;
    }
    left=input*(1.f-mix_)+sumL*(mix_*.25f);
    right=input*(1.f-mix_)+sumR*(mix_*.25f);
  }
 private:
  static constexpr int lengths_[8]={1493,1601,1747,1867,1517,1627,1777,1901};
  float buffer_[8][1901]={},damped_[8]={},feedback_[8]={},target_feedback_[8]={};
  int pos_[8]={},valid_[8]={};
  float mix_=0,target_mix_=0;int last_time_=-1;bool initialized_=false;
};
constexpr int VoiceReverb::lengths_[8];
