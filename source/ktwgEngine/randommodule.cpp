#include "randommodule.h"

std::uniform_real_distribution<float> Random::uni_real_dis;
std::uniform_int_distribution<> Random::uni_int_dis;

float Random::Range(float a, float b)
{
  static std::random_device rd{};
  static std::mt19937_64 gen(rd());
  uni_real_dis = std::uniform_real_distribution<float>{ a,b };
  return uni_real_dis(gen);
}

int Random::Range(int a, int b)
{
  static std::random_device rd{};
  static std::mt19937_64 gen(rd());
  uni_int_dis = std::uniform_int_distribution<>{ a,b };
  return uni_int_dis(gen);
}
