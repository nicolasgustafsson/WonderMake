#include "pch.h"
#include "Color.h"

template <>
SColorTemplate<EColorSpace::HSV> ConvertColor(SColorTemplate<EColorSpace::LinearRGB> aFrom)
{
    const f32 minColorValue = WmMath::Min(WmMath::Min(aFrom.R, aFrom.G), aFrom.B);
    const f32 maxColorValue = WmMath::Max(WmMath::Max(aFrom.R, aFrom.G), aFrom.B);
    const f32 colorValueDelta = maxColorValue - minColorValue;

    SColorHSV hsv;

    if (colorValueDelta > 0) {
        if (maxColorValue == aFrom.R) {
            hsv.Hue = 60 * (WmMath::Mod(((aFrom.G - aFrom.B) / colorValueDelta), 6.f));
        }
        else if (maxColorValue == aFrom.G) {
            hsv.Hue = 60 * (((aFrom.B - aFrom.R) / colorValueDelta) + 2);
        }
        else if (maxColorValue == aFrom.B) {
            hsv.Hue = 60 * (((aFrom.R - aFrom.G) / colorValueDelta) + 4);
        }

        if (maxColorValue > 0) {
            hsv.Saturation = colorValueDelta / maxColorValue;
        }
        else {
            hsv.Saturation = 0;
        }

        hsv.Value = maxColorValue;
    }
    else {
        hsv.Hue = 0;
        hsv.Saturation = 0;
        hsv.Value = maxColorValue;
    }

    if (hsv.Hue < 0) {
        hsv.Hue = SDegreeF32(360.f) + hsv.Hue;
    }

	return hsv;
}

template <>
SColorTemplate<EColorSpace::LinearRGB> ConvertColor(SColorTemplate<EColorSpace::HSV> aFrom)
{
    SColor rgb;

    f32 chroma = aFrom.Value * aFrom.Saturation;
    f32 hueInSixth = WmMath::Mod(aFrom.Hue.Rotation / 60.0f, 6.f);
    f32 fX = chroma * (1 - fabs(WmMath::Mod(hueInSixth, 2.f) - 1.f));
    f32 baseValue = aFrom.Value - chroma;

    if (0 <= hueInSixth && hueInSixth < 1) {
        rgb.R = chroma;
        rgb.G = fX;
        rgb.B = 0;
    }
    else if (1 <= hueInSixth && hueInSixth < 2) {
        rgb.R = fX;
        rgb.G = chroma;
        rgb.B = 0;
    }
    else if (2 <= hueInSixth && hueInSixth < 3) {
        rgb.R = 0;
        rgb.G = chroma;
        rgb.B = fX;
    }
    else if (3 <= hueInSixth && hueInSixth < 4) {
        rgb.R = 0;
        rgb.G = fX;
        rgb.B = chroma;
    }
    else if (4 <= hueInSixth && hueInSixth < 5) {
        rgb.R = fX;
        rgb.G = 0;
        rgb.B = chroma;
    }
    else if (5 <= hueInSixth && hueInSixth < 6) {
        rgb.R = chroma;
        rgb.G = 0;
        rgb.B = fX;
    }
    else {
        rgb.R = 0;
        rgb.G = 0;
        rgb.B = 0;
    }

    rgb.R += baseValue;
    rgb.G += baseValue;
    rgb.B += baseValue;

	return rgb;
}

SColorTemplate<EColorSpace::LinearRGB>::operator SColorTemplate<EColorSpace::HSV>() const
{
	return ConvertColor<EColorSpace::LinearRGB, EColorSpace::HSV>(*this);
}

SColorTemplate<EColorSpace::HSV>::operator SColorTemplate<EColorSpace::LinearRGB>() const
{
	return ConvertColor<EColorSpace::HSV, EColorSpace::LinearRGB>(*this);
}
