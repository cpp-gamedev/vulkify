#include <VkBootstrap.h>
#include <detail/vk_instance.hpp>

namespace vf {
Result<VKInstance> VKInstance::make(MakeSurface const makeSurface, bool const validation) {
	if (!makeSurface) { return Error::eInvalidArgument; }
	vk::DynamicLoader dl;
	VULKAN_HPP_DEFAULT_DISPATCHER.init(dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));
	vkb::InstanceBuilder vib;
	if (validation) { vib.request_validation_layers(); }
	auto vi = vib.set_app_name("vulkify").use_default_debug_messenger().build();
	if (!vi) { return Error::eVulkanInitFailure; }
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vi->instance);
	VKInstance ret;
	ret.instance = vk::UniqueInstance(vi->instance, {nullptr});
	ret.messenger = vk::UniqueDebugUtilsMessengerEXT(vi->debug_messenger, {vi->instance});
	auto surface = makeSurface(vk::Instance(vi->instance));
	if (!surface) { return Error::eVulkanInitFailure; }
	ret.surface = vk::UniqueSurfaceKHR(surface, {vi->instance});
	vkb::PhysicalDeviceSelector vpds(vi.value());
	auto vpd = vpds.require_present().prefer_gpu_device_type(vkb::PreferredDeviceType::discrete).set_surface(surface).select();
	if (!vpd) { return Error::eVulkanInitFailure; }
	ret.gpu.properties = vk::PhysicalDeviceProperties(vpd->properties);
	ret.gpu.device = vk::PhysicalDevice(vpd->physical_device);
	ret.gpu.formats = ret.gpu.device.getSurfaceFormatsKHR(*ret.surface);
	vkb::DeviceBuilder vdb(vpd.value());
	auto vd = vdb.build();
	if (!vd) { return Error::eVulkanInitFailure; }
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vd->device);
	ret.device = vk::UniqueDevice(vd->device, {nullptr});
	auto queue = vd->get_queue(vkb::QueueType::graphics);
	auto qfam = vd->get_queue_index(vkb::QueueType::graphics);
	if (!queue || !qfam) { return Error::eVulkanInitFailure; }
	ret.queue = VKQueue{vk::Queue(queue.value()), qfam.value()};
	ret.mutex = std::make_unique<std::mutex>();
	return ret;
}

vk::UniqueImageView makeImageView(vk::Device device, vk::Image const image, vk::Format const format, vk::ImageAspectFlags const aspects) {
	vk::ImageViewCreateInfo info;
	info.viewType = vk::ImageViewType::e2D;
	info.format = format;
	// TODO: research
	// info.components.r = vk::ComponentSwizzle::eR;
	// info.components.g = vk::ComponentSwizzle::eG;
	// info.components.b = vk::ComponentSwizzle::eB;
	// info.components.a = vk::ComponentSwizzle::eA;
	info.components.r = info.components.g = info.components.b = info.components.a = vk::ComponentSwizzle::eIdentity;
	info.subresourceRange = {aspects, 0, 1, 0, 1};
	info.image = image;
	return device.createImageViewUnique(info);
}
} // namespace vf
