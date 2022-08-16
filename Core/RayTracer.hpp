#pragma once

#include "ModelViewController.hpp"
#include "SceneList.hpp"
#include "UserSettings.hpp"
#include "Vulkan/RayTracing/Application.hpp"

class RayTracer final : public Vulkan::RayTracing::Application
{
public:

	VULKAN_OBJECT(RayTracer)

	RayTracer(const UserSettings& userSettings, const Vulkan::WindowConfig& windowConfig, VkPresentModeKHR presentMode);
	~RayTracer();

protected:

	const Assets::Scene& get_scene() const override { return *scene_; }
	Assets::UniformBufferObject get_uniformBufferObjects(VkExtent2D extent) const override;

	void set_physical_device(
		VkPhysicalDevice physicalDevice, 
		std::vector<const char*>& requiredExtensions, 
		VkPhysicalDeviceFeatures& deviceFeatures, 
		void* nextDeviceFeatures) override;

	void on_device_set() override;
	void create_swapChain() override;
	void delete_swapChain() override;
	void draw_frame() override;
	void render(VkCommandBuffer commandBuffer, uint32_t imageIndex) override;

	void on_key(int key, int scancode, int action, int mods) override;
	void on_cursorPosition(double xpos, double ypos) override;
	void on_mouseButton(int button, int action, int mods) override;
	void on_scroll(double xoffset, double yoffset) override;

private:

	void LoadScene(uint32_t sceneIndex);
	void CheckAndUpdateBenchmarkState(double prevTime);
	void CheckFramebufferSize() const;

	uint32_t sceneIndex_{};
	UserSettings userSettings_{};
	UserSettings previousSettings_{};
	SceneList::CameraInitialSate cameraInitialSate_{};
	ModelViewController modelViewController_{};

	std::unique_ptr<const Assets::Scene> scene_;
	std::unique_ptr<class UserInterface> userInterface_;

	double time_{};

	uint32_t totalNumberOfSamples_{};
	uint32_t numberOfSamples_{};
	bool resetAccumulation_{};

	// Benchmark stats
	double sceneInitialTime_{};
	double periodInitialTime_{};
	uint32_t periodTotalFrames_{};
};
