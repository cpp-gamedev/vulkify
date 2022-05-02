#pragma once
#include <vulkify/core/result.hpp>
#include <vulkify/graphics/geometry.hpp>
#include <vulkify/graphics/resource.hpp>
#include <type_traits>

namespace vf {
struct BufferWrite {
	void const* data{};
	std::size_t size{};
};

enum class PolygonMode { eFill, eLine, ePoint };
enum class Topology { eTriangleList, eTriangleStrip, eLineList, eLineStrip, ePointList };

struct PipelineState {
	PolygonMode polygonMode{PolygonMode::eFill};
	Topology topology{Topology::eTriangleList};
	float lineWidth{1.0f};
};

class GeometryBuffer : public GfxResource {
  public:
	using State = PipelineState;
	using GfxResource::GfxResource;

	Result<void> write(Geometry geometry);

	Geometry const& geometry() const { return m_geometry; }
	State state{};

  protected:
	Geometry m_geometry{};
};

class UniformBuffer : public GfxResource {
  public:
	using GfxResource::GfxResource;

	std::size_t size() const;
	Result<void> resize(std::size_t size);
	Result<void> write(BufferWrite data);

	template <typename T>
		requires(std::is_standard_layout_v<T>)
	Result<void> write(T const& t) { return write(std::span<T const>(&t, sizeof(T))); }
};
} // namespace vf
