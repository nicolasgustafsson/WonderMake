#pragma once
#include "Utilities/Range.h"

struct SCircle
{
    SVector2f Position;
    f32 Radius;

    [[nodiscard]] SRectangle GetBoundingBox() const
    {
        return { Position.X - Radius, Position.Y + Radius, Position.X + Radius, Position.Y - Radius };
    }

    [[nodiscard]] bool ContainsPoint(const SVector2f aPoint) const
    {
        return Position.DistanceTo(aPoint) < Radius;
    }

    [[nodiscard]] auto CreatePoints(const SRange<SDegreeF32> aSegmentRange = SRange<SDegreeF32>{0.f, 359.f}, const SDegreeF32 aDegreesPerPoint = 5.0f) const
    {
        Container<SVector2f> points;

        for(SDegreeF32 angle = aSegmentRange.Start; angle < aSegmentRange.End; angle += aDegreesPerPoint)
        {
            points.Add(Position + SVector2f::FromRotation(angle) * Radius);
        }

        points.Add(Position + SVector2f::FromRotation(aSegmentRange.End) * Radius);

        return points;
    }
};