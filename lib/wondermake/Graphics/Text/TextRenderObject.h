#pragma once

#include "Graphics/RenderObject.h"

#include "wondermake-io/FileResourceProxy.h"

#include "wondermake-utility/Geometry.h"
#include "wondermake-utility/SharedReference.h"
#include "wondermake-utility/Vector.h"

#include <memory>

class FontResource;
class ShaderProgram;
class ShaderResourceSystem;

class TextRenderObject
	: public RenderObject<
		EVertexAttribute::Position,
		EVertexAttribute::TexCoord>
{
public:
	TextRenderObject(ShaderResourceSystem& aShaderSystem, std::shared_ptr<ShaderProgram> aShaderProgram, FileResourceRef<FontResource> aFont);

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

	SRectangleF						myBoundingBox;
	FileResourceRef<FontResource>	myFont;
	
	std::string myText;

	f32			myMaxWidth	= 0.f;
	f32			mySize		= 16.f;
	SVector2f	myPosition;
	SVector2f	myOrigin;

};
