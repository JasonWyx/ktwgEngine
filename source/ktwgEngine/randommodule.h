#pragma once
#include <random>

class Random
{
public:
  static float Range(float a = FLT_MIN, float b = FLT_MAX);
  static int Range(int a, int b);

private:
  static std::uniform_real_distribution<float> uni_real_dis;
  static std::uniform_int_distribution<> uni_int_dis;
};