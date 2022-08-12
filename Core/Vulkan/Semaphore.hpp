#pragma once

#include "Wrapper.hpp"

namespace Vulkan
{
	class Device;

	class Semaphore final
	{
	public:

		Semaphore(const Semaphore&) = delete;
		Semaphore& operator = (const Semaphore&) = delete;
		Semaphore& operator = (Semaphore&&) = delete;

		explicit Semaphore(const Device& device);
		Semaphore(Semaphore&& other) noexcept;
		~Semaphore();

		const class Device& get_device() const { return device; }

	private:

		const class Device& device;

		VULKAN_HANDLES(VkSemaphore, semaphore)
	};

}
