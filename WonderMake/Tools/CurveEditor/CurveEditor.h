#include "System/System.h"
#include "Utilities/Debugging/Debugged.h"
#include "Imgui/CustomRenderArea.h"
#include "Utilities/Curve/Curve.h"

class CurveEditor
	: public System
	, public CustomRenderArea
{
public:
	CurveEditor();

protected:
	virtual void PreCustomArea() noexcept override;
	virtual void Render() override;

private:
	Curve myCurve;

	void DoDragPoint(SVector2f& aPoint, const f32 aSize);
	void DoCurvePoint(SCurvePoint& aPoint);
	void PostProcessPoints();
};