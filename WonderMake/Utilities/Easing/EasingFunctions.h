#pragma once
#include "Utilities/WmMath.h"
#include <concepts>

template<typename T>
struct SRange
{
	T Start;
	T End;
};

namespace WmEasings
{

	inline [[nodiscard]] constexpr f32 Flip(const f32 aProgress) noexcept
	{
		return 1.f - aProgress;
	}

	template <WmMath::Interpolable T>
	inline [[nodiscard]] constexpr T Lerp(const SRange<T>& aRange, const f32 aProgress) noexcept
	{
		return WmMath::Lerp(aRange.Start, aRange.End, aProgress);
	}

	template <WmMath::Interpolable T>
	inline [[nodiscard]] constexpr T InvLerp(const SRange<T>& aRange, const f32 aProgress) noexcept
	{
		return (aProgress - aRange.Start) / (aRange.End - aRange.Start);
	}

	template <u32 Power = 2, WmMath::Interpolable T>
	inline [[nodiscard]] constexpr T SmoothStart(const SRange<T>& aRange, const f32 aProgress) noexcept
	{
		return Lerp(aRange, WmMath::Pow(aProgress, Power));
	}

	template <u32 Power = 2, WmMath::Interpolable T>
	inline [[nodiscard]] constexpr T SmoothStop(const SRange<T>& aRange, const f32 aProgress) noexcept
	{
		return Lerp(aRange, Flip(WmMath::Pow(Flip(aProgress), Power)));
	}

	template <WmMath::Interpolable T>
	inline [[nodiscard]] constexpr T Mix(const SRange<T>& aRange, T(*aFirstFunction)(const SRange<T>&, const f32), T(*aSecondFunction)(const SRange<T>&, const f32), const f32 aBlend, const f32 aProgress) noexcept
	{
		return Lerp(SRange<T>{ aFirstFunction(aRange, aProgress), aSecondFunction(aRange, aProgress) }, aBlend);
	}

	template <WmMath::Interpolable T>
	inline [[nodiscard]] constexpr T Crossfade(const SRange<T>& aRange, T(*aFirstFunction)(const SRange<T>&, const f32), T(*aSecondFunction)(const SRange<T>&, const f32), const f32 aProgress) noexcept
	{
		return Mix(aRange, aFirstFunction, aSecondFunction, aProgress, aProgress);
	}

	template <u32 Power, WmMath::Interpolable T> requires (Power == 1)
	inline [[nodiscard]] constexpr T SmoothStep(const SRange<T>& aRange, const f32 aProgress) noexcept
	{
		return Lerp(SRange<T>{aRange}, aProgress);
	}

	template <u32 Power = 3, WmMath::Interpolable T> requires (Power % 2 == 1 && Power > 1)
	inline [[nodiscard]] constexpr T SmoothStep(const SRange<T>& aRange, const f32 aProgress) noexcept
	{
		//return aProgress * aProgress * aProgress * (aProgress * (aProgress * T(6.0) - T(15.0)) + T(10.0));
		//return Crossfade<T>(aRange, SmoothStart<(Power - 1) / 2 + 1>, SmoothStop<(Power - 1) / 2 + 1>, aProgress);
		return Crossfade(aRange, 
			SmoothStart<(Power+1) / 2, T>, 
			SmoothStop<(Power+1) / 2, T>,
			SmoothStep<Power - 2>(SRange<f32>{0.f, 1.f}, aProgress));
	}

	template <u32 Power, WmMath::Interpolable T> requires (Power % 2 == 0 && Power > 3)
		inline [[nodiscard]] constexpr T SmoothStep(const SRange<T>& aRange, const f32 aProgress) noexcept
	{
		return Mix<T>(aRange, SmoothStep<Power - 1>, SmoothStep<Power + 1>, 0.5f, aProgress);
	}

	template <WmMath::Interpolable T>
	inline [[nodiscard]] constexpr T Scale(const SRange<T>& aRange, T(*aFunction)(const SRange<T>&, const f32), const f32 aProgress) noexcept
	{
		return aFunction(aRange, aProgress) * aProgress;
	}

	template <WmMath::Interpolable T>
	inline [[nodiscard]] constexpr T ReverseScale(const SRange<T>& aRange, T(*aFunction)(const SRange<T>&, const f32), const f32 aProgress) noexcept
	{
		return aFunction(aRange, aProgress) * Flip(aProgress);
	}

	template <u32 Power = 2, WmMath::Interpolable T> requires (Power % 2 == 0)
	inline [[nodiscard]] constexpr T BellCurve(const SRange<T>& aRange, const f32 aProgress) noexcept
	{
		return SmoothStart<Power / 2>(aRange, aProgress) * Flip(SmoothStop<Power / 2>(aRange, aProgress));
	}


	template <WmMath::Interpolable T>
	inline [[nodiscard]] constexpr T QuadraticBezier(const SRange<T>& aRange, const T& aControl, const f32 aProgress) noexcept
	{
		SRange<T> newRange = { Lerp<T>({ aRange.Start, aControl }, aProgress), Lerp<T>({aControl, aRange.End}, aProgress)};

		return Lerp(newRange, aProgress);
	}

	template <WmMath::Interpolable T>
	inline [[nodiscard]] constexpr T CubicBezier(const SRange<T>& aRange, const T& aControlFirst, const T& aControlSecond, const f32 aProgress) noexcept
	{
		//SRange<T> newRange = //{ QuadraticBezier<T>({ aRange.Start, aControlFirst }, aControlSecond, aProgress), QuadraticBezier<T>({ aControlSecond, aRange.End}, aControlFirst, aProgress) };
		SRange<T> newRange = { QuadraticBezier<T>({ aRange.Start, aControlSecond }, aControlFirst, aProgress), QuadraticBezier<T>({ aControlFirst, aRange.End}, aControlSecond, aProgress) };
		//
		
		//T green1 = Lerp<T>({ aRange.Start, aControlFirst }, aProgress);
		//T green2 = Lerp<T>({ aControlFirst, aControlSecond }, aProgress);
		//T green3 = Lerp<T>({ aControlSecond, aRange.End }, aProgress);
		//
		//T blue1 = Lerp<T>({ green1, green2 }, aProgress);
		//T blue2 = Lerp<T>({ green2, green3 }, aProgress);
		//newRange = { blue1, blue2 };

		return Lerp<T>(newRange, aProgress);
	}

	template <WmMath::Interpolable TMotionType = f32>
	struct SDampedSpringMotionParams
	{
		// newPos = posPosCoef*oldPos + posVelCoef*oldVel
		f32 PosPosCoef, PosVelCoef;
		// newVel = velPosCoef*oldPos + velVelCoef*oldVel
		f32 VelPosCoef, VelVelCoef;
	};

	template <WmMath::Interpolable TMotionType = f32>
	struct SSpringMotionParameters
	{
		TMotionType Position;
		TMotionType Velocity;
	};

	template <WmMath::Interpolable TMotionType = f32>
	SDampedSpringMotionParams<TMotionType> CalcDampedSpringMotionParams(
		       // motion parameters result
		const f32	           deltaTime,        // time step to advance
		f32	                   angularFrequency, // angular frequency of motion
		f32	                   dampingRatio)     // damping ratio of motion
	{
		SDampedSpringMotionParams returnParams;
		const f32 epsilon = 0.0001f;

		// force values into legal range
		if (dampingRatio < 0.0f) dampingRatio = 0.0f;
		if (angularFrequency < 0.0f) angularFrequency = 0.0f;

		// if there is no angular frequency, the spring will not move and we can
		// return identity
		if (angularFrequency < epsilon)
		{
			returnParams.PosPosCoef = 1.0f; returnParams.PosVelCoef = 0.0f;
			returnParams.VelPosCoef = 0.0f; returnParams.VelVelCoef = 1.0f;
			return returnParams;
		}

		if (dampingRatio > 1.0f + epsilon)
		{
			// over-damped
			const f32 za = -angularFrequency * dampingRatio;
			const f32 zb = angularFrequency * sqrtf(dampingRatio * dampingRatio - 1.0f);
			const f32 z1 = za - zb;
			const f32 z2 = za + zb;

			const f32 e1 = expf(z1 * deltaTime);
			const f32 e2 = expf(z2 * deltaTime);

			const f32 invTwoZb = 1.0f / (2.0f * zb); // = 1 / (z2 - z1)

			const f32 e1_Over_TwoZb = e1 * invTwoZb;
			const f32 e2_Over_TwoZb = e2 * invTwoZb;

			const f32 z1e1_Over_TwoZb = z1 * e1_Over_TwoZb;
			const f32 z2e2_Over_TwoZb = z2 * e2_Over_TwoZb;

			returnParams.PosPosCoef = e1_Over_TwoZb * z2 - z2e2_Over_TwoZb + e2;
			returnParams.PosVelCoef = -e1_Over_TwoZb + e2_Over_TwoZb;
					  
			returnParams.VelPosCoef = (z1e1_Over_TwoZb - z2e2_Over_TwoZb + e2) * z2;
			returnParams.VelVelCoef = -z1e1_Over_TwoZb + z2e2_Over_TwoZb;
		}
		else if (dampingRatio < 1.0f - epsilon)
		{
			// under-damped
			const f32 omegaZeta = angularFrequency * dampingRatio;
			const f32 alpha = angularFrequency * sqrtf(1.0f - dampingRatio * dampingRatio);

			const f32 expTerm = expf(-omegaZeta * deltaTime);
			const f32 cosTerm = cosf(alpha * deltaTime);
			const f32 sinTerm = sinf(alpha * deltaTime);

			const f32 invAlpha = 1.0f / alpha;

			const f32 expSin = expTerm * sinTerm;
			const f32 expCos = expTerm * cosTerm;
			const f32 expOmegaZetaSin_Over_Alpha = expTerm * omegaZeta * sinTerm * invAlpha;

			returnParams.PosPosCoef = expCos + expOmegaZetaSin_Over_Alpha;
			returnParams.PosVelCoef = expSin * invAlpha;
					  
			returnParams.VelPosCoef = -expSin * alpha - omegaZeta * expOmegaZetaSin_Over_Alpha;
			returnParams.VelVelCoef = expCos - expOmegaZetaSin_Over_Alpha;
		}
		else
		{
			// critically damped
			const f32 expTerm = expf(-angularFrequency * deltaTime);
			const f32 timeExp = deltaTime * expTerm;
			const f32 timeExpFreq = timeExp * angularFrequency;

			returnParams.PosPosCoef = timeExpFreq + expTerm;
			returnParams.PosVelCoef = timeExp;
			returnParams.VelPosCoef = -angularFrequency * timeExpFreq;
			returnParams.VelVelCoef = -timeExpFreq + expTerm;
		}

		return returnParams;
	}

	template <WmMath::Interpolable TMotionType = f32>
	SSpringMotionParameters<TMotionType> UpdateDampedSpringMotiona(
		SDampedSpringMotionParams<f32> 	dampedParams,
		SSpringMotionParameters<TMotionType>	aCurrentSpringMotionParams,
		const TMotionType						equilibriumPos)				
	{

		const TMotionType oldPos = aCurrentSpringMotionParams.Position - equilibriumPos; // update in equilibrium relative space
		const TMotionType oldVel = aCurrentSpringMotionParams.Velocity;

		aCurrentSpringMotionParams.Position = oldPos * dampedParams.PosPosCoef + oldVel * dampedParams.PosVelCoef + equilibriumPos;
		aCurrentSpringMotionParams.Velocity = oldPos * dampedParams.VelPosCoef + oldVel * dampedParams.VelVelCoef;
		return aCurrentSpringMotionParams;
	}

	template <WmMath::Interpolable TMotionType = f32>
	SSpringMotionParameters<TMotionType> UpdateDampedSpringMotion(
		SSpringMotionParameters<TMotionType>	aCurrentSpringMotionParams,	// velocity value to update
		const TMotionType						equilibriumPos,				// position to approach
		const f32								deltaTime,					// time step to advance
		const f32								angularFrequency,			// angular frequency of motion
		const f32								dampingRatio)				// motion parameters to use
	{
		auto params = CalcDampedSpringMotionParams(deltaTime, angularFrequency, dampingRatio);

		return UpdateDampedSpringMotiona<TMotionType>(params, aCurrentSpringMotionParams, equilibriumPos);
	}
}