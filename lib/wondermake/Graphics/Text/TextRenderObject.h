#pragma once

#include "Graphics/Text/Font.h"
#include "Graphics/RenderObject.h"

#include "wondermake-utility/Geometry.h"
#include "wondermake-utility/Vector.h"

class TextRenderObject
	: public RenderObject<
		EVertexAttribute::Position,
		EVertexAttribute::TexCoord>
{
public:
	TextRenderObject(
		ResourceSystem<Shader<EShaderType::Vertex>>& aVsSystem,
		ResourceSystem<Shader<EShaderType::Fragment>>& aFsSystem,
		ResourceSystem<Shader<EShaderType::Geometry>>& aGsSystem,
		ResourceProxy<Font> aFont);

	void SetText(const std::string& aText);
	void SetSize(const f32 aSize);
	void SetColor(const SColor aColor);
	void SetOrigin(const SVector2f aOrigin);

	void SetPosition(const SVector2f aPosition);

	SRectangleF GetBoundingBox() const;

	void SetMaxWidth(const f32 aWidth);

private:
	void UpdateVertices();

	SRectangleF CalculateBoundingBox() const;
	f32 GetAscent() const;

	SRectangleF			myBoundingBox;
	ResourceProxy<Font>	myFont;
	
	std::string myText;

	f32			myMaxWidth	= 0.f;
	f32			mySize		= 16.f;
	SVector2f	myPosition;
	SVector2f	myOrigin;

};
