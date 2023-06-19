#include "random.h"

unsigned int SeedGenerator::generateTrueRandomSeed() {
  std::random_device rd_;
  return rd_();
}
