#pragma once
#include <vulkify/core/result.hpp>
#include <vulkify/instance/instance.hpp>

namespace vf {
class HeadlessInstance : public Instance {
  public:
	using Result = vf::Result<ktl::kunique_ptr<HeadlessInstance>>;

	static Result make() { return ktl::make_unique<HeadlessInstance>(); }

	GPU const& gpu() const override { return m_gpu; }
	glm::ivec2 framebufferSize() const override { return m_framebufferSize; }
	glm::ivec2 windowSize() const override { return m_windowSize; }

	bool closing() const override { return m_closing; }
	void show() override {}
	void hide() override {}
	void close() override {}
	Poll poll() override { return std::move(m_poll); }

	bool beginPass() override { return true; }
	bool endPass(Rgba) override { return true; }

	Poll m_poll{};
	glm::ivec2 m_framebufferSize{};
	glm::ivec2 m_windowSize{};
	bool m_closing{};

  private:
	GPU m_gpu = {"vulkify (headless)", GPU::Type::eOther};
};
} // namespace vf
