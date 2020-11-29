#pragma once
struct SLine
{
	bool operator==(const SLine& aOther) const { return (aOther.First == First && aOther.Second == Second) || (aOther.First == Second && aOther.Second == First); };
	bool operator!=(const SLine& aOther) const { return !(aOther == *this); };

	f32 LengthSquared() const noexcept
	{
		return (First - Second).LengthSquared();
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

	SDegreeF32 GetRotation() const noexcept
	{
		return (Second - First).GetAngle<SDegreeF32>(0);
	}


	SVector2f First;
	SVector2f Second;
};

