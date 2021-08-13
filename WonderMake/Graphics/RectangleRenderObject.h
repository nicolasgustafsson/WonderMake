#pragma once
class RectangleRenderObject : public RenderObject<EVertexAttribute::Position>
{
public:
	RectangleRenderObject(const SRectangle aRectangle, const std::filesystem::path& aFragmentShader);

	void SetRectangle(const SRectangle aRectangle);
};

