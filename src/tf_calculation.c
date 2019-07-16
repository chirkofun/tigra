#include <tf_calculation.h>

#define dT  0.001

void tfOutCalculation ( TFConf_t* p_tf  )
{
    if(p_tf->input==1)
    {
        p_tf->k = 20; //
    }
    else
    {
        p_tf->k = 0;
    }
}
