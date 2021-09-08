#pragma once
class RectangleRenderObject : public RenderObject<EVertexAttribute::Position, EVertexAttribute::TexCoord>
{
public:
	RectangleRenderObject(const SRectangle aRectangle, const std::filesystem::path& aFragmentShader);

	void SetRectangle(const SRectangle aRectangle);
};

