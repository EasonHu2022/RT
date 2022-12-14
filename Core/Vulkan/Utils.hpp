#pragma once

#include "Wrapper.hpp"
#include <vector>
#include "Device.hpp"

namespace  Vulkan {

template <class TValue>
inline void get_vkEnumerate(VkResult(enumerate) (uint32_t*, TValue*), std::vector<TValue>& vector)
{
	uint32_t count = 0;
	Check(enumerate(&count, nullptr),
		"enumerate");

	vector.resize(count);
	Check(enumerate(&count, vector.data()),
		"enumerate");
}

template <class THandle, class TValue>
inline void get_vkEnumerate(THandle handle,VkResult(enumerate) (THandle, uint32_t*, TValue*), std::vector<TValue>& vector)
{
	uint32_t count = 0;
	Check(enumerate(handle ,&count, nullptr),
		"enumerate");

	vector.resize(count);
	Check(enumerate(handle ,&count, vector.data()),
		"enumerate");
}

template <class THandle, class TValue>
inline void get_vkEnumerate(THandle handle, void(enumerate) (THandle, uint32_t*, TValue*), std::vector<TValue>& vector)
{
	uint32_t count = 0;
	enumerate(handle, &count, nullptr);

	vector.resize(count);
	enumerate(handle, &count, vector.data());

}

template <class THandle1, class THandle2, class TValue>
inline void get_vkEnumerate(THandle1 handle1, THandle2 handle2, VkResult(enumerate) (THandle1, THandle2, uint32_t*, TValue*), std::vector<TValue>& vector)
{
	uint32_t count = 0;
	Check(enumerate(handle1, handle2, &count, nullptr),
		"enumerate");

	vector.resize(count);
	Check(enumerate(handle1, handle2, &count, vector.data()),
		"enumerate");
}

//***********************************//
template <class TValue>
inline std::vector<TValue> get_vkEnumerate(VkResult(enumerate) (uint32_t*, TValue*))
{
	std::vector<TValue> values;
	get_vkEnumerate(enumerate, values);
	return values;
}

template <class THandle, class TValue>
inline std::vector<TValue> get_vkEnumerate(THandle handle, void(enumerate) (THandle handle,uint32_t*, TValue*))
{
	std::vector<TValue> values;
	get_vkEnumerate(handle,enumerate, values);
	return values;
}

template <class THandle1, class THandle2, class TValue>
inline std::vector<TValue> get_vkEnumerate(THandle1 handle1, THandle2 handle2, VkResult(enumerate) (THandle1, THandle2, uint32_t*, TValue*))
{
	std::vector<TValue> values;
	get_vkEnumerate(handle1, handle2, enumerate, values);
	return values;
}



}
