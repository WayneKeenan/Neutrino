#pragma once


#define TEST_BIT(Bitmask, Bit) ((Bitmask >> static_cast<uint8>(Bit)) & 1)
#define SET_BIT(Bitmask, Bit) (Bitmask |= 1 << static_cast<uint8>(Bit))
#define CLEAR_BIT(Bitmask, Bit) (Bitmask ^= (1 << static_cast<uint8>(Bit)))

template<class T>
T clamp(T v, T min, T max)
{
	if (v > max)
		v = max;

	if (v < min)
		v = min;

	return (v);
}

