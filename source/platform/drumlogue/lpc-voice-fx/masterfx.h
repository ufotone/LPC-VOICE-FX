#pragma once
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include "unit.h"
#include "low_cut.h"
#include "voice_reverb.h"

// Live 8 kHz, order-10 LPC analysis and lattice resynthesis, 48 kHz I/O.
class MasterFX {
 public:
  MasterFX() {const int defaults[]={0,18,0,100,80,120,25,1500};for(int i=0;i<8;++i)p_[i].store(defaults[i]);
    for(int i=0;i<320;++i)window_[i]=.5f-.5f*std::cos(6.28318530718f*i/319.f);}
  int8_t Init(const unit_runtime_desc_t* d) {
    if(d->samplerate!=48000)return k_unit_err_samplerate;
    if(d->input_channels!=4 || d->output_channels!=2)return k_unit_err_geometry;
    clear();return k_unit_err_none;
  }
  void Teardown(){reset_.store(true);} void Reset(){reset_.store(true);}
  void Resume(){reset_.store(true);} void Suspend(){reset_.store(true);}
  void setParameter(uint8_t id,int32_t value) {
    static const int lo[]={0,0,-24,0,0,20,0,200},hi[]={1,60,36,100,100,1000,100,4000};
    if(id<8)p_[id].store(std::max(lo[id],std::min(int(value),hi[id])));
  }
  int32_t getParameterValue(uint8_t id)const{return id<8?p_[id].load():0;}
  const char* getParameterStrValue(uint8_t id,int32_t v)const {
    if(id==0 && v==0)return "SIDECHAIN";
    if(id==0 && v==1)return "MAIN";
    return nullptr;
  }
  const uint8_t* getParameterBmpValue(uint8_t,int32_t)const{return nullptr;}
  void LoadPreset(uint8_t){} uint8_t getPresetIndex()const{return 0;}
  static const char* getPresetName(uint8_t){return nullptr;}
  void Process(const float* in,float* out,size_t frames) {
    if(reset_.exchange(false))clear();
    int v[8];for(int i=0;i<8;++i)v[i]=p_[i].load();
    if(v[0]!=source_){clear();source_=v[0];}
    low_cut_.setCutoff(v[5]);
    reverb_.controls(v[6],v[7]);
    float gain=std::pow(10.f,v[1]/20.f), pitch=130.f*std::pow(2.f,v[2]/12.f);
    float mix=v[3]*.01f,level=v[4]*.01f;
    for(size_t i=0;i<frames;++i) {
      float left=in[i*4],right=in[i*4+1];
      float source=v[0]?(left+right)*.5f:(in[i*4+2]+in[i*4+3])*.5f;
      float voice=low_cut_.process(process(source*gain,pitch)),wetL,wetR;
      reverb_.process(voice,wetL,wetR);wetL*=level;wetR*=level;
      // Sidechain is an additional voice; main input remains the backing mix.
      float l=v[0]?left*(1-mix)+wetL*mix:left+wetL*mix;
      float r=v[0]?right*(1-mix)+wetR*mix:right+wetR*mix;
      out[2*i]=std::max(-1.f,std::min(l,1.f));out[2*i+1]=std::max(-1.f,std::min(r,1.f));
    }
  }
 private:
  void clear() {
    low_cut_.reset();
    reverb_.reset();
    for(float& x:fir_)x=0;
    for(float& x:raw_)x=0;
    for(float& x:pre_)x=0;
    for(float& x:state_)x=0;
    for(float& x:current_)x=0;
    for(float& x:target_)x=0;
    fir_pos_=write_=count_=hop_=sub_=0;prev_=phase_=deemph_=hp_x_=hp_y_=a_=b_=level_=target_level_=0;
    voiced_=false;previous_voiced_=false;noise_=0x51005220u;
  }
  float process(float input,float pitch) {
    static const float fir[31]={.000646920f,-.000106703f,-.001393402f,-.003586869f,-.006578815f,-.009491123f,-.010654594f,-.007927335f,.000697063f,.016360107f,.038725721f,.065712804f,.093710364f,.118266140f,.135085529f,.141068384f,.135085529f,.118266140f,.093710364f,.065712804f,.038725721f,.016360107f,.000697063f,-.007927335f,-.010654594f,-.009491123f,-.006578815f,-.003586869f,-.001393402f,-.000106703f,.000646920f};
    fir_[fir_pos_]=std::max(-2.f,std::min(2.f,input));fir_pos_=(fir_pos_+1)%31;
    if(sub_==0) {
      float raw=0;unsigned j=fir_pos_;for(int k=0;k<31;++k){raw+=fir[k]*fir_[j];j=(j+1)%31;}
      raw_[write_]=raw;pre_[write_]=raw-.85f*prev_;prev_=raw;write_=(write_+1)%320;
      if(count_<320)++count_;
      if(++hop_==80){hop_=0;if(count_==320)analyze();}
      a_=b_;b_=synthesize(pitch);
    }
    float y=a_+(b_-a_)*(sub_/6.f);sub_=(sub_+1)%6;
    float x=y*7.94328235f;float mag=std::fabs(x);
    if(mag>.75f){float e=mag-.75f;x=std::copysign(.75f+.2f*e/(.2f+e),x);}
    return x;
  }
  void analyze() {
    float x[320],r[11]={};float raw[320];float mean=0;
    for(int i=0;i<320;++i){unsigned k=(write_+i)%320;raw[i]=raw_[k];mean+=raw[i];
      x[i]=pre_[k]*window_[i];}
    mean/=320;for(float& z:raw)z-=mean;
    for(int lag=0;lag<=10;++lag)for(int i=lag;i<320;++i)r[lag]+=x[i]*x[i-lag];
    float rms=std::sqrt(std::max(r[0]/320.f,0.f));
    target_level_=rms>.00015f?rms:0.f;
    if(target_level_==0){for(float& k:target_)k=0;voiced_=false;return;}
    r[0]*=1.0005f;float a[11]={1},err=std::max(r[0],1e-10f);
    for(int m=1;m<=10;++m){float sum=r[m];for(int j=1;j<m;++j)sum+=a[j]*r[m-j];
      float k=std::max(-.96f,std::min(.96f,-sum/err));float old[11];for(int j=0;j<=10;++j)old[j]=a[j];
      for(int j=1;j<m;++j)a[j]=old[j]+k*old[m-j];
      a[m]=k;err=std::max(err*(1-k*k),1e-10f);
      int levels=m<=2?31:(m<=7?15:7);float q=std::round((k+1)*levels*.5f)*2/levels-1;
      target_[m-1]=std::max(-.96f,std::min(.96f,q));}
    float best=0;for(int lag=20;lag<=123;++lag){float ac=0,e1=0,e2=0;
      for(int i=lag;i<320;++i){ac+=raw[i]*raw[i-lag];e1+=raw[i]*raw[i];e2+=raw[i-lag]*raw[i-lag];}
      best=std::max(best,ac/std::sqrt(std::max(e1*e2,1e-15f)));}
    voiced_=best>.48f;
  }
  float synthesize(float hz) {
    float product=1;for(int i=0;i<10;++i){current_[i]+=(target_[i]-current_[i])*.035f;product*=1-current_[i]*current_[i];}
    level_+=(target_level_-level_)*.035f;
    float excitation;
    if(voiced_){if(!previous_voiced_)phase_=1;phase_+=hz/8000.f;excitation=0;
      if(phase_>=1){phase_-=std::floor(phase_);excitation=std::sqrt(8000.f/hz);}}
    else{noise_^=noise_<<13;noise_^=noise_>>17;noise_^=noise_<<5;excitation=(int32_t(noise_>>8)-8388608)*(1.7320508f/8388608.f);}
    previous_voiced_=voiced_;
    float value=excitation*level_*std::sqrt(std::max(product,1e-12f));
    for(int m=9;m>=0;--m){value-=current_[m]*state_[m];if(m<9)state_[m+1]=state_[m]+current_[m]*value;}
    state_[0]=value;deemph_=value+.85f*deemph_;float hp=deemph_-hp_x_+.9428f*hp_y_;hp_x_=deemph_;hp_y_=hp;
    return hp;
  }
  std::atomic<int32_t> p_[8];std::atomic<bool> reset_{false};int source_=0;VoiceLowCut low_cut_;VoiceReverb reverb_;
  float fir_[31]={},raw_[320]={},pre_[320]={},window_[320]={},state_[10]={},current_[10]={},target_[10]={};
  unsigned fir_pos_=0,write_=0,count_=0,hop_=0,sub_=0;uint32_t noise_=0x51005220u;
  float prev_=0,phase_=0,deemph_=0,hp_x_=0,hp_y_=0,a_=0,b_=0,level_=0,target_level_=0;
  bool voiced_=false,previous_voiced_=false;
};
