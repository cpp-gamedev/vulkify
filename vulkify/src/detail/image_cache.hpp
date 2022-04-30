#pragma once
#include <detail/vram.hpp>

namespace vf {
struct ImageCache {
	vk::ImageCreateInfo& setDepth() {
		static constexpr auto flags = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransientAttachment;
		info = vk::ImageCreateInfo();
		info.usage = flags;
		aspect |= vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
		return info;
	}

	vk::ImageCreateInfo& setColour() {
		static constexpr auto flags = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc;
		info = vk::ImageCreateInfo();
		info.usage = flags;
		aspect |= vk::ImageAspectFlagBits::eColor;
		return info;
	}

	vk::ImageCreateInfo& setTexture(bool transferSrc) {
		static constexpr auto flags = vk::ImageUsageFlagBits::eSampled;
		info = vk::ImageCreateInfo();
		info.usage = flags;
		info.format = vram.textureFormat;
		info.usage |= vk::ImageUsageFlagBits::eTransferDst;
		if (transferSrc) { info.usage |= vk::ImageUsageFlagBits::eTransferSrc; }
		aspect |= vk::ImageAspectFlagBits::eColor;
		return info;
	}

	bool ready(vk::Extent3D extent, vk::Format format) const noexcept { return image && extent == info.extent && info.format == format; }

	VKImage make(vk::Extent3D extent, vk::Format format) {
		info.extent = extent;
		info.format = format;
		vram.device.defer(std::move(image), std::move(view));
		image = vram.makeImage(info, preferHost, name.c_str());
		view = vram.device.makeImageView(image->resource, format, aspect);
		return peek();
	}

	VKImage refresh(vk::Extent3D extent, vk::Format format = {}) {
		if (format == vk::Format()) { format = info.format; }
		if (!ready(extent, format)) { make(extent, format); }
		return peek();
	}

	VKImage peek() const noexcept { return {image->resource, view ? *view : vk::ImageView(), {info.extent.width, info.extent.height}}; }

	Vram vram{};
	std::string name{};
	vk::ImageCreateInfo info{};
	UniqueImage image{};
	vk::UniqueImageView view{};
	vk::ImageAspectFlags aspect{};
	bool preferHost{};
	vk::ImageViewType viewType{vk::ImageViewType::e2D};
};
} // namespace vf
