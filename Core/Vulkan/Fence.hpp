#pragma once

#include "Wrapper.hpp"

namespace Vulkan
{
	class Device;

	class Fence
	{
	public:

		Fence(const Fence&) = delete;
		Fence& operator = (const Fence&) = delete;
		Fence& operator = (Fence&&) = delete;

		explicit Fence(const Device& device, bool signaled);
		Fence(Fence&& other) noexcept;
		~Fence();

		const class Device& get_device() const { return device; }
		const VkFence& Handle() const { return fence; }

		void Reset();
		void Wait(uint64_t timeout) const;

	private:

		const class Device& device;

		VkFence fence{};
	};

}
