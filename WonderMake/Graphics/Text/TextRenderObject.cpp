#include "pch.h"
#include "TextRenderObject.h"
#include "Font.h"
#include "Resources/ResourceSystem.h"

TextRenderObject::TextRenderObject(const std::filesystem::path& aFontPath) :
	RenderObject(SRenderObjectInfo
		{ std::filesystem::current_path() / "Shaders/Vertex/Text.vert"
		,	"" //geometry shader
		,	"Shaders/Fragment/Text.frag"
		,	""
		,	6 * 16
		,	GL_TRIANGLES })
{
	SetSize(mySize);
	myFont = SystemPtr<ResourceSystem<Font>>()->GetResource(aFontPath);
	SetTexture(myFont->GetTexture());
	UpdateVertices();
}

void TextRenderObject::SetText(const std::string& aText)
{ 
	myText = aText;
	SetRenderCount(static_cast<u32>(myText.size()) * 6);
	UpdateVertices();
}

void TextRenderObject::SetSize(const f32 aSize)
{
	mySize = aSize;
	UpdateVertices();
	SetProperty("Size", mySize);
}

void TextRenderObject::SetColor(const SColor aColor)
{
	SetProperty("Color", aColor);
}

void TextRenderObject::SetPosition(const SVector2f aPosition)
{
	myPosition = aPosition;
	UpdateVertices();
}

void TextRenderObject::UpdateVertices()
{
	SVector2f cursorPosition = myPosition;
	for (u32 i = 0; i < myText.size(); i++)
	{
		SGlyphMetrics glyph = myFont->GetGlyphMetrics(myText[i]);

		if (myText[i] == '\n')
		{
			cursorPosition.Y -= myFont->GetFontMetrics().LineHeight * mySize;
			cursorPosition.X = myPosition.X;
		}

		SetAttribute<EVertexAttribute::Position>(i * 6 + 0, cursorPosition + SVector2f(glyph.PlaneBounds.Left, glyph.PlaneBounds.Bottom) * mySize);
		SetAttribute<EVertexAttribute::Position>(i * 6 + 1, cursorPosition + SVector2f(glyph.PlaneBounds.Right, glyph.PlaneBounds.Bottom) * mySize);
		SetAttribute<EVertexAttribute::Position>(i * 6 + 2, cursorPosition + SVector2f(glyph.PlaneBounds.Right, glyph.PlaneBounds.Top) * mySize);

		SetAttribute<EVertexAttribute::Position>(i * 6 + 3, cursorPosition + SVector2f(glyph.PlaneBounds.Left, glyph.PlaneBounds.Bottom) * mySize);
		SetAttribute<EVertexAttribute::Position>(i * 6 + 4, cursorPosition + SVector2f(glyph.PlaneBounds.Right, glyph.PlaneBounds.Top) * mySize);
		SetAttribute<EVertexAttribute::Position>(i * 6 + 5, cursorPosition + SVector2f(glyph.PlaneBounds.Left, glyph.PlaneBounds.Top) * mySize);

		SetAttribute<EVertexAttribute::TexCoord>(i * 6 + 0, SVector2f(glyph.AtlasBounds.Left, 1.f - glyph.AtlasBounds.Bottom));
		SetAttribute<EVertexAttribute::TexCoord>(i * 6 + 1, SVector2f(glyph.AtlasBounds.Right, 1.f - glyph.AtlasBounds.Bottom));
		SetAttribute<EVertexAttribute::TexCoord>(i * 6 + 2, SVector2f(glyph.AtlasBounds.Right, 1.f - glyph.AtlasBounds.Top));

		SetAttribute<EVertexAttribute::TexCoord>(i * 6 + 3, SVector2f(glyph.AtlasBounds.Left, 1.f - glyph.AtlasBounds.Bottom));
		SetAttribute<EVertexAttribute::TexCoord>(i * 6 + 4, SVector2f(glyph.AtlasBounds.Right, 1.f - glyph.AtlasBounds.Top));
		SetAttribute<EVertexAttribute::TexCoord>(i * 6 + 5, SVector2f(glyph.AtlasBounds.Left, 1.f - glyph.AtlasBounds.Top));

		cursorPosition.X += glyph.Advance * mySize;
		if (i > 0)
		{
			cursorPosition.X += myFont->GetKerning(myText[i], myText[i]);
		}


		//SVector2f previousLocation = previousPoint;
		//SVector2f location = aPoints[i % aPoints.size()];
		//SVector2f nextLocation = aPoints[(i + 1) % aPoints.size()];
		//
		//SVector2f direction = ((location - nextLocation).GetNormalized() - (location - previousLocation).GetNormalized()).GetNormalized();
		//SVector2f perpendicularCw = direction.GetPerpendicularClockWise();
		//SVector2f perpendicularCcw = direction.GetPerpendicularCounterClockWise();
		//
		//SetAttribute<EVertexAttribute::Position>(static_cast<u32>(i * 2), location + perpendicularCw * (outerThickness));
		//SetAttribute<EVertexAttribute::Position>(static_cast<u32>(i * 2 + 1), location + perpendicularCcw * (innerThickness));
		//
		//WmDrawDebugLine(location + perpendicularCw * (outerThickness), location + perpendicularCcw * (innerThickness), SColor::Blue(), 100.f);
		//
		//previousPoint = location;
	}
}
