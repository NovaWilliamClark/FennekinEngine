#include "random.hpp"

unsigned int SeedGenerator::generateTrueRandomSeed() {
  std::random_device rd_;
  return rd_();
}
