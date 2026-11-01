#include "unit.h"
const __unit_header unit_header_t unit_header = {
  .header_size=sizeof(unit_header_t), .target=UNIT_TARGET_PLATFORM | k_unit_module_masterfx,
  .api=UNIT_API_VERSION, .dev_id=0x5534544eU, .unit_id=0x4c504658U,
  .version=0x00000400U, .name="LPC VOICE FX", .num_presets=0, .num_params=8,
  .params={
    {0,1,0,0,k_unit_param_type_strings,0,0,0,{"INPUT"}},
    {0,60,0,18,k_unit_param_type_db,0,0,0,{"IN GAIN"}},
    {-24,36,0,0,k_unit_param_type_semi,0,0,0,{"PITCH"}},
    {0,100,0,100,k_unit_param_type_percent,0,0,0,{"MIX"}},
    {0,100,0,80,k_unit_param_type_percent,0,0,0,{"LEVEL"}},
    {20,1000,120,120,k_unit_param_type_hertz,0,0,0,{"LOW CUT"}},
    {0,100,0,25,k_unit_param_type_percent,0,0,0,{"REV MIX"}},
    {200,4000,1500,1500,k_unit_param_type_msec,0,0,0,{"REV TIME"}},
  }
};
