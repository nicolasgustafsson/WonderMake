#pragma once
struct SLine
{
	bool operator==(const SLine& aOther) const { return (aOther.First == First && aOther.Second == Second) || (aOther.First == Second && aOther.Second == First); };
	bool operator!=(const SLine& aOther) const { return !(aOther == *this); };

	constexpr f32 LengthSquared() const noexcept
	{
		return (First - Second).LengthSquared();
	}

	constexpr SVector2f GetNormal() const noexcept
	{
		return (Second - First).GetPerpendicularClockWise().GetNormalized();
	}

	void ShortenEnd(const f32 aShortenAmount) noexcept
	{
		if (aShortenAmount * aShortenAmount >= LengthSquared())
			Second = First;

		Second -= (Second - First).GetNormalized() * aShortenAmount;
	}

	void ExtendEnd(const f32 aExtendAmount) noexcept
	{
		Second += (Second - First).GetNormalized() * aExtendAmount;
	}

	void SetLength(const f32 aLength)
	{
		Second = First + (Second - First).GetNormalized() * aLength;
	}

	SDegreeF32 GetRotation() const noexcept
	{
		return (Second - First).GetAngle<SDegreeF32>(0);
	}

	constexpr SVector2f GetMiddle() const noexcept
	{
		return First + (Second - First) * 0.5f;
	}


	SVector2f First;
	SVector2f Second;
};

struct SSurface : public SLine
{
    f32 Width = 0.f;
};

