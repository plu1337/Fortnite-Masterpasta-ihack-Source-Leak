#include "stdafx.h"

struct Vector2
{
	float                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

	inline Vector2 operator/(float v) const
	{
		return Vector2(X / v, Y / v);
	}

	inline Vector2() //UD CODE
		: X(0), Y(0)
	{ }

	inline Vector2(float x, float y)
		: X(x),
		Y(y)
	{ }

	Vector2& operator+=(const Vector2& v) {
		X += v.X;
		X += v.Y;
		return *this;
	}

	Vector2 operator-(const Vector2& v) {
		return Vector2{ X - v.X, Y - v.Y };
	}

	Vector2 operator+(const Vector2& v) {
		return Vector2{ X + v.X, Y + v.Y };
	}


	inline double Length() {
		return (double)(Customsqrtf_(X * X + Y * Y));
	}

	inline float Customsqrtf_(float x)
	{
		union { float f; uint32_t i; } z = { x };
		z.i = 0x5f3759df - (z.i >> 1);
		z.f *= (1.5f - (x * 0.5f * z.f * z.f));
		z.i = 0x7EEEEEEE - z.i;
		return z.f;
	}
};