#pragma once
#include <ktl/kunique_ptr.hpp>
#include <vulkify/core/result.hpp>
#include <vulkify/instance/instance.hpp>
#include <vulkify/instance/instance_create_info.hpp>

namespace vf {
class VulkifyInstance : public Instance {
  public:
	using CreateInfo = InstanceCreateInfo;
	using Result = vf::Result<ktl::kunique_ptr<VulkifyInstance>>;

	static Result make(CreateInfo const& createInfo);

	~VulkifyInstance();

	Vram const& vram() const override;
	Gpu const& gpu() const override;
	glm::uvec2 framebufferSize() const override;
	glm::uvec2 windowSize() const override;
	glm::ivec2 position() const override;
	glm::vec2 contentScale() const override;
	glm::vec2 cursorPosition() const override;
	CursorMode cursorMode() const override;
	MonitorList monitors() const override;
	WindowFlags windowFlags() const override;

	void show() const override;
	void hide() const override;
	void close() const override;
	void setPosition(glm::ivec2 xy) const override;
	void setSize(glm::uvec2 size) const override;
	void setIcons(std::span<Icon const> icons) const override;
	bool closing() const override;
	void setCursorMode(CursorMode mode) const override;
	Cursor makeCursor(Icon icon) const override;
	void destroyCursor(Cursor cursor) const override;
	bool setCursor(Cursor cursor) const override;
	void setWindowed(glm::uvec2 extent) const override;
	void setFullscreen(Monitor const& monitor, glm::uvec2 resolution) const override;
	void updateWindowFlags(WindowFlags set, WindowFlags unset) const override;

	Poll poll() override;
	Surface beginPass() override;
	bool endPass() override;

  private:
	struct Impl;
	ktl::kunique_ptr<Impl> m_impl;

	VulkifyInstance(ktl::kunique_ptr<Impl> impl) noexcept;
};

} // namespace vf
