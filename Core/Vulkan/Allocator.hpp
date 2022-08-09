#pragma once
#include "Wrapper.hpp"
#include <vk_mem_alloc.h>
namespace Vulkan
{
	class Device;
	class Instance;
	class Allocator
	{
	public:
		 ~Allocator();
		Allocator(VmaAllocator vma, Device device, Instance instance);

		Allocator(Allocator const&) = delete;
		Allocator& operator= (Allocator const&) = delete;

		Allocator(Allocator&&) noexcept;
		Allocator& operator = (Allocator&&) noexcept;

	
	private:
		//const class Device& device;
		VULKAN_HANDLES(VmaAllocator, allocator)
	};
}

