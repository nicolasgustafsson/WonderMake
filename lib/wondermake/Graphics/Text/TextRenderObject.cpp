#include "pch.h"
#include "TextRenderObject.h"
#include "Font.h"
#include "Resources/ResourceSystem.h"

TextRenderObject::TextRenderObject(
	ResourceSystem<Shader<EShaderType::Vertex>>& aVsSystem,
	ResourceSystem<Shader<EShaderType::Fragment>>& aFsSystem,
	ResourceSystem<Shader<EShaderType::Geometry>>& aGsSystem,
	ResourceProxy<Font> aFont)
	: RenderObject(SRenderObjectInfo
		{	aVsSystem
		,	aFsSystem
		,	aGsSystem
		,	std::filesystem::current_path() / "Shaders/Vertex/Text.vert"
		,	"" //geometry shader
		,	"Shaders/Fragment/Text.frag"
		,	ResourceProxy<Texture>()
		,	6 * 16
		,	GL_TRIANGLES })
{
	myFont = std::move(aFont);
	SetSize(mySize);
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

void TextRenderObject::SetOrigin(const SVector2f aOrigin)
{
	myOrigin = aOrigin;
	UpdateVertices();
}

void TextRenderObject::SetPosition(const SVector2f aPosition)
{
	myPosition = aPosition;
	UpdateVertices();
}

SRectangleF TextRenderObject::GetBoundingBox() const
{
	return myBoundingBox;
}

void TextRenderObject::SetMaxWidth(const f32 aWidth)
{
	myMaxWidth = aWidth;
	UpdateVertices();
}

void TextRenderObject::UpdateVertices()
{
	myBoundingBox = CalculateBoundingBox();
	const SFontMetrics fontMetrics = myFont->GetFontMetrics();

	SVector2f cursorPosition = myPosition;
	cursorPosition.Y -= GetAscent() * mySize;

	cursorPosition.X -= myOrigin.X * myBoundingBox.GetWidth();
	cursorPosition.Y += myOrigin.Y * myBoundingBox.GetHeight();
	for (u32 i = 0; i < myText.size(); i++)
	{
		const SGlyphMetrics glyph = myFont->GetGlyphMetrics(myText[i]);
		
		if (myText[i] == '\n' || ((cursorPosition.X - myPosition.X) > myMaxWidth && myMaxWidth > 0.f))
		{
			cursorPosition.Y -= fontMetrics.LineHeight * mySize;
			cursorPosition.X = myPosition.X;
		}

		if (i > 0 && cursorPosition.X != myPosition.X)
		{
			cursorPosition.X += myFont->GetKerning(myText[i - 1], myText[i]) * mySize;
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
	}
}

SRectangleF TextRenderObject::CalculateBoundingBox() const
{
	const SFontMetrics fontMetrics = myFont->GetFontMetrics();

	SRectangleF boundingBox{};
	boundingBox.Left = myPosition.X;

	f32 maxX = myPosition.X;

	SVector2f cursorPosition = myPosition;
	cursorPosition.Y -= GetAscent() * mySize;
	boundingBox.Top = cursorPosition.Y + GetAscent() * mySize;

	for (u32 i = 0; i < myText.size(); i++)
	{
		const SGlyphMetrics glyph = myFont->GetGlyphMetrics(myText[i]);

		if (myText[i] == '\n' || ((cursorPosition.X - myPosition.X) > myMaxWidth && myMaxWidth > 0.f))
		{
			cursorPosition.Y -= fontMetrics.LineHeight * mySize;
			cursorPosition.X = myPosition.X;
		}

		if (i > 0 && cursorPosition.X != myPosition.X)
		{
			cursorPosition.X += myFont->GetKerning(myText[i - 1], myText[i]) * mySize;
		}

		if (cursorPosition.X + glyph.PlaneBounds.Right * mySize > maxX)
			maxX = cursorPosition.X + glyph.PlaneBounds.Right * mySize;

		cursorPosition.X += glyph.Advance * mySize;

		if (cursorPosition.X > maxX)
			maxX = cursorPosition.X;
	}
	boundingBox.Right = maxX;
	boundingBox.Bottom = cursorPosition.Y;// + fontMetrics.Descender * mySize; not including descender here cause it goes "under the line", so if we center it from there it will look off.

	const f32 width = boundingBox.GetWidth();
	const f32 height = boundingBox.GetHeight();

	boundingBox.Left -= width * myOrigin.X;
	boundingBox.Right -= width * myOrigin.X;
	boundingBox.Top += height * myOrigin.Y;
	boundingBox.Bottom += height * myOrigin.Y;

	return boundingBox;
}

f32 TextRenderObject::GetAscent() const
{
	//this is the best looking result
	return myFont->GetGlyphMetrics('W').PlaneBounds.Top;
}
