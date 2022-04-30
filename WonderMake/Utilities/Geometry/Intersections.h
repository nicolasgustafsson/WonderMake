#pragma once
#include "Utilities/Geometry/Geometry.h"
#include "Utilities/Geometry/Line.h"

namespace WmIntersections2D
{
    inline bool IsPointWithinCircle(const SCircle& aCircle, const SVector2f aPoint) noexcept
    {
        const auto delta = aCircle.Position - aPoint;

        return (delta.LengthSquared() <= std::pow(aCircle.Radius, 2));
    }

    inline SVector2f GetClosestPointOnSurface(const SSurface& aSurface, const SVector2f aPoint) noexcept
    {
        //alghorithm based on one found here: https://stackoverflow.com/questions/3120357/get-closest-point-to-a-line
        const SVector2f lineStart = aSurface.First;
        const SVector2f lineEnd = aSurface.Second;
        if (lineStart == lineEnd)
            return lineStart;

        const SVector2f lineDelta = lineEnd - lineStart;
        const SVector2f sphereDeltaFromLineStart = aPoint - lineStart;

        const f32 lineDeltaMagnitude = lineDelta.LengthSquared();
        const f32 lineDeltaProduct = lineDelta.Dot(sphereDeltaFromLineStart);

        const f32 normalizedDistance = lineDeltaProduct / lineDeltaMagnitude; //distance on line to the closest point on it, from start to end

        if (normalizedDistance < 0.f)
            return lineStart; //clamp on line ends

        if (normalizedDistance > 1.f)
            return lineEnd;

        return lineStart + lineDelta * normalizedDistance;
    }

    inline bool CircleVsCircle(const SCircle& aFirst, const SCircle& aSecond)
    {
        const auto delta = aFirst.Position - aSecond.Position;

        return (delta.LengthSquared() <= std::pow(aFirst.Radius + aSecond.Radius, 2));
    }

    inline bool CircleVsSurface(const SCircle& aCircle, const SSurface& aSurface)
    {
        SVector2f closestPoint = GetClosestPointOnSurface(aSurface, aCircle.Position);

        const SVector2f lineDelta = aSurface.Second - aSurface.First;

        const SVector2f lineNormal = lineDelta.GetPerpendicularClockWise().GetNormalized();

        //[Nicos]: create a newline that is perpendicular to the one line we have to simulate width
        SSurface newSurface;
        newSurface.First = closestPoint - lineNormal * aSurface.Width;
        newSurface.Second = newSurface.First + lineNormal * aSurface.Width * 2.f;

        closestPoint = GetClosestPointOnSurface(newSurface, aCircle.Position);

        if (closestPoint.DistanceTo(aCircle.Position) < aCircle.Radius)
            return true;

        return false;
    }
}