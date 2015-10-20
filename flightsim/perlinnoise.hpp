//shamelessly stolen from http://stackoverflow.com/questions/4753055/perlin-noise-generation-for-terrain

#pragma once

class PerlinNoise
{
public:

  // Constructor
    PerlinNoise();
    PerlinNoise(float _persistence, float _frequency, float _amplitude, int _octaves, int _randomseed);

  // Get Height
    float GetHeight(float x, float y) const;

  // Get
  float Persistence() const { return persistence; }
  float Frequency()   const { return frequency;   }
  float Amplitude()   const { return amplitude;   }
  int    Octaves()     const { return octaves;     }
  int    RandomSeed()  const { return randomseed;  }

  // Set
  void Set(float _persistence, float _frequency, float _amplitude, int _octaves, int _randomseed);

  void SetPersistence(float _persistence) { persistence = _persistence; }
  void SetFrequency(  float _frequency)   { frequency = _frequency;     }
  void SetAmplitude(  float _amplitude)   { amplitude = _amplitude;     }
  void SetOctaves(    int    _octaves)     { octaves = _octaves;         }
  void SetRandomSeed( int    _randomseed)  { randomseed = _randomseed;   }

private:

    float Total(float i, float j) const;
    float GetValue(float x, float y) const;
    float Interpolate(float x, float y, float a) const;
    float Noise(int x, int y) const;

    float persistence, frequency, amplitude;
    int octaves, randomseed;
};
