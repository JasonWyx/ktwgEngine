#ifndef _SIMPLE_FORWARD_PARAMS_H_
#define _SIMPLE_FORWARD_PARAMS_H_

struct SimpleForwardParams
{
  float4 m_Color;
};

#define SimpleForwardParamsSlot register(b0)

#ifdef _SIMPLE_FORWARD_

cbuffer SIMPLE_FORWARD_PARAMS : SimpleForwardParamsSlot
{
  SimpleForwardParams cSimpleForwardParams;
};

float4 GetColor()
{
  return cSimpleForwardParams.m_Color;
}

#define g_SimpleForwardParamsColor GetColor()

#endif // #ifdef _SIMPLE_FORWARD_

#endif // #ifndef _SIMPLE_FORWARD_PARAMS_H_