#pragma once

#include <random>

// A random generator for generating pseudorandom seeds.
class SeedGenerator {
 public:
  explicit SeedGenerator(unsigned int seed) : m_gen(seed) {}

  // Returns the next sampled seed.
  unsigned int next() { return m_gen(); }

  // Generates a single random seed using an entropy device.
  static unsigned int generateTrueRandomSeed();

 private:
  std::mt19937 m_gen;
};

// A random distribution that generates uniform real numbers in the range
// [0.0, 1.0).
class UniformRandom {
 public:
  explicit UniformRandom(unsigned int seed)
      : m_gen(seed), m_rand(0.0f, 1.0f), m_seed(seed) {}
  UniformRandom() : UniformRandom(SeedGenerator::generateTrueRandomSeed()) {}

  // Returns the next sampled random number.
  float next() { return m_rand(m_gen); }

  // Returns the seed used to initialize this random sampler.
  unsigned int getSeed() const { return m_seed; }

 private:
  // RNG for random sampling. We use the Mersenne Twister because it has
  // high-quality characteristics.
  std::mt19937 m_gen;
  std::uniform_real_distribution<float> m_rand;
  const unsigned int m_seed;
};
