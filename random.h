#ifndef __RANDOM_HH__
#define __RANDOM_HH__

#include <random>
#include <cmath>

struct Random {
public:
  explicit Random(long seed0 = 0) : uni_base_(0, 1) {
    gen_.seed(seed0);
  }
  void seed(long seed0) { gen_.seed(seed0); } 
  double r(void) { return uni_base_(gen_); }
  double exponential(double tau) { return -tau*std::log(1.0-r()); };
  double gauss(double mu, double sigma) { return sigma*std::sqrt(-2*std::log(r()))*std::cos(2*M_PI*r())+mu; } ;
  double uniform(double a, double b) {  return a + (b-a)*r(); };

private:
  std::mt19937 gen_;
  std::uniform_real_distribution<double> uni_base_;
};

#endif // __RANDOM_HH__
