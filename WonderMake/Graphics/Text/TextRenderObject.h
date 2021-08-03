#pragma once
#include "Graphics/RenderObject.h"
#include "Font.h"

class TextRenderObject : public RenderObject<EVertexAttribute::Position,
	EVertexAttribute::TexCoord>
{
public:
	TextRenderObject(const std::filesystem::path& aFontPath);

	void SetText(const std::string& aText);
	void SetSize(const f32 aSize);
	void SetColor(const SColor aColor);

	void SetPosition(const SVector2f aPosition);

private:

	void UpdateVertices();

	ResourceProxy<Font> myFont;
	std::string myText{};
	f32 mySize = 16.f;
	SVector2f myPosition{};
};

