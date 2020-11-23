//
// CG
// VK.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_VK_H
#define YF_CG_VK_H

#include <vector>

#include "Defs.h"

#if defined(__linux__)
# define VK_USE_PLATFORM_WAYLAND_KHR
# define VK_USE_PLATFORM_XCB_KHR
#elif defined(__APPLE__)
# define VK_USE_PLATFORM_METAL_EXT
#elif defined (_WIN32)
# define VK_USE_PLATFORM_WIN32_KHR
#else
# error "Invalid platform"
#endif
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

CG_NS_BEGIN

/// Initializes VK.
///
void initVK();

/// Sets VK instance-level procedures (null/non-null instance).
///
void setProcsVK(VkInstance instance);

/// Sets VK device-level procedures.
///
void setProcsVK(VkDevice device);

/// Deinitializes VK.
///
void deinitVK();

#define CG_INSTPROCVK(inst, name) \
name = reinterpret_cast<PFN_##name>(CG_NS::vkGetInstanceProcAddr(inst, #name))

#define CG_DEVPROCVK(dev, name) \
name = reinterpret_cast<PFN_##name>(CG_NS::vkGetDeviceProcAddr(dev, #name))

#define CG_DECLVK(name) extern PFN_##name name
#define CG_DEFVK(name)  PFN_##name CG_NS::name

/// Instance-level procedures (null instance).
///
CG_DECLVK(vkGetInstanceProcAddr);
CG_DECLVK(vkEnumerateInstanceVersion); // 1.1
CG_DECLVK(vkEnumerateInstanceExtensionProperties);
CG_DECLVK(vkEnumerateInstanceLayerProperties);
CG_DECLVK(vkCreateInstance);

/// Instance-level procedures (non-null instance).
///
CG_DECLVK(vkDestroyInstance);
CG_DECLVK(vkEnumeratePhysicalDevices);
CG_DECLVK(vkGetPhysicalDeviceProperties);
CG_DECLVK(vkGetPhysicalDeviceQueueFamilyProperties);
CG_DECLVK(vkGetPhysicalDeviceMemoryProperties);
CG_DECLVK(vkGetPhysicalDeviceFormatProperties);
CG_DECLVK(vkGetPhysicalDeviceFeatures);
CG_DECLVK(vkGetPhysicalDeviceProperties2); // 1.1
CG_DECLVK(vkGetPhysicalDeviceQueueFamilyProperties2); // 1.1
CG_DECLVK(vkGetPhysicalDeviceMemoryProperties2); // 1.1
CG_DECLVK(vkGetPhysicalDeviceFormatProperties2); // 1.1
CG_DECLVK(vkGetPhysicalDeviceFeatures2); // 1.1
CG_DECLVK(vkEnumerateDeviceExtensionProperties);
CG_DECLVK(vkCreateDevice);
CG_DECLVK(vkDestroySurfaceKHR); // VK_KHR_surface
CG_DECLVK(vkGetPhysicalDeviceSurfaceSupportKHR); // VK_KHR_surface
CG_DECLVK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR); // VK_KHR_surface
CG_DECLVK(vkGetPhysicalDeviceSurfaceFormatsKHR); // VK_KHR_surface
CG_DECLVK(vkGetPhysicalDeviceSurfacePresentModesKHR); // VK_KHR_surface
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
CG_DECLVK(vkCreateWaylandSurfaceKHR); // VK_KHR_wayland_surface
CG_DECLVK(vkGetPhysicalDeviceWaylandPresentationSupportKHR); // VK_KHR_wayland_surface
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
CG_DECLVK(vkCreateWin32SurfaceKHR); // VK_KHR_win32_surface
CG_DECLVK(vkGetPhysicalDeviceWin32PresentationSupportKHR); // VK_KHR_win32_surface
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
CG_DECLVK(vkCreateXcbSurfaceKHR); // VK_KHR_xcb_surface
CG_DECLVK(vkGetPhysicalDeviceXcbPresentationSupportKHR); // VK_KHR_xcb_surface
#endif
#ifdef VK_USE_PLATFORM_METAL_EXT
CG_DECLVK(vkCreateMetalSurfaceEXT); // VK_EXT_metal_surface
#endif

/// Device-level procedures.
///
CG_DECLVK(vkGetDeviceProcAddr);
CG_DECLVK(vkDestroyDevice);
CG_DECLVK(vkGetDeviceQueue);
CG_DECLVK(vkCreateCommandPool);
CG_DECLVK(vkTrimCommandPool); // 1.1
CG_DECLVK(vkResetCommandPool);
CG_DECLVK(vkDestroyCommandPool);
CG_DECLVK(vkAllocateCommandBuffers);
CG_DECLVK(vkResetCommandBuffer);
CG_DECLVK(vkFreeCommandBuffers);
CG_DECLVK(vkBeginCommandBuffer);
CG_DECLVK(vkEndCommandBuffer);
CG_DECLVK(vkQueueSubmit);
CG_DECLVK(vkCmdExecuteCommands);
CG_DECLVK(vkCreateFence);
CG_DECLVK(vkDestroyFence);
CG_DECLVK(vkGetFenceStatus);
CG_DECLVK(vkResetFences);
CG_DECLVK(vkWaitForFences);
CG_DECLVK(vkCreateSemaphore);
CG_DECLVK(vkDestroySemaphore);
CG_DECLVK(vkGetSemaphoreCounterValue); // 1.2
CG_DECLVK(vkWaitSemaphores); // 1.2
CG_DECLVK(vkSignalSemaphore); // 1.2
CG_DECLVK(vkCmdPipelineBarrier);
CG_DECLVK(vkQueueWaitIdle);
CG_DECLVK(vkDeviceWaitIdle);
CG_DECLVK(vkCreateRenderPass);
CG_DECLVK(vkDestroyRenderPass);
CG_DECLVK(vkCreateFramebuffer);
CG_DECLVK(vkDestroyFramebuffer);
CG_DECLVK(vkCmdBeginRenderPass);
CG_DECLVK(vkCmdEndRenderPass);
CG_DECLVK(vkCmdNextSubpass);
CG_DECLVK(vkCreateShaderModule);
CG_DECLVK(vkDestroyShaderModule);
CG_DECLVK(vkCreateGraphicsPipelines);
CG_DECLVK(vkCreateComputePipelines);
CG_DECLVK(vkDestroyPipeline);
CG_DECLVK(vkCmdBindPipeline);
CG_DECLVK(vkCreatePipelineCache);
CG_DECLVK(vkMergePipelineCaches);
CG_DECLVK(vkGetPipelineCacheData);
CG_DECLVK(vkDestroyPipelineCache);
CG_DECLVK(vkAllocateMemory);
CG_DECLVK(vkFreeMemory);
CG_DECLVK(vkMapMemory);
CG_DECLVK(vkUnmapMemory);
CG_DECLVK(vkCreateBuffer);
CG_DECLVK(vkDestroyBuffer);
CG_DECLVK(vkCreateBufferView);
CG_DECLVK(vkDestroyBufferView);
CG_DECLVK(vkCreateImage);
CG_DECLVK(vkDestroyImage);
CG_DECLVK(vkGetImageSubresourceLayout);
CG_DECLVK(vkCreateImageView);
CG_DECLVK(vkDestroyImageView);
CG_DECLVK(vkGetBufferMemoryRequirements);
CG_DECLVK(vkGetImageMemoryRequirements);
CG_DECLVK(vkBindBufferMemory);
CG_DECLVK(vkBindImageMemory);
CG_DECLVK(vkCreateSampler);
CG_DECLVK(vkDestroySampler);
CG_DECLVK(vkCreateDescriptorSetLayout);
CG_DECLVK(vkDestroyDescriptorSetLayout);
CG_DECLVK(vkGetDescriptorSetLayoutSupport); // 1.1
CG_DECLVK(vkCreatePipelineLayout);
CG_DECLVK(vkDestroyPipelineLayout);
CG_DECLVK(vkCreateDescriptorPool);
CG_DECLVK(vkDestroyDescriptorPool);
CG_DECLVK(vkResetDescriptorPool);
CG_DECLVK(vkAllocateDescriptorSets);
CG_DECLVK(vkFreeDescriptorSets);
CG_DECLVK(vkUpdateDescriptorSets);
CG_DECLVK(vkCmdBindDescriptorSets);
CG_DECLVK(vkCmdPushConstants);
CG_DECLVK(vkCmdClearColorImage);
CG_DECLVK(vkCmdClearDepthStencilImage);
CG_DECLVK(vkCmdClearAttachments);
CG_DECLVK(vkCmdFillBuffer);
CG_DECLVK(vkCmdUpdateBuffer);
CG_DECLVK(vkCmdCopyBuffer);
CG_DECLVK(vkCmdCopyImage);
CG_DECLVK(vkCmdCopyBufferToImage);
CG_DECLVK(vkCmdCopyImageToBuffer);
CG_DECLVK(vkCmdBlitImage);
CG_DECLVK(vkCmdResolveImage);
CG_DECLVK(vkCmdDraw);
CG_DECLVK(vkCmdDrawIndexed);
CG_DECLVK(vkCmdDrawIndirect);
CG_DECLVK(vkCmdDrawIndexedIndirect);
CG_DECLVK(vkCmdBindVertexBuffers);
CG_DECLVK(vkCmdBindIndexBuffer);
CG_DECLVK(vkCmdSetViewport);
CG_DECLVK(vkCmdSetLineWidth);
CG_DECLVK(vkCmdSetDepthBias);
CG_DECLVK(vkCmdSetScissor);
CG_DECLVK(vkCmdSetDepthBounds);
CG_DECLVK(vkCmdSetStencilCompareMask);
CG_DECLVK(vkCmdSetStencilWriteMask);
CG_DECLVK(vkCmdSetStencilReference);
CG_DECLVK(vkCmdSetBlendConstants);
CG_DECLVK(vkCmdDispatch);
CG_DECLVK(vkCmdDispatchIndirect);
CG_DECLVK(vkCreateSwapchainKHR); // VK_KHR_swapchain
CG_DECLVK(vkDestroySwapchainKHR); // VK_KHR_swapchain
CG_DECLVK(vkGetSwapchainImagesKHR); // VK_KHR_swapchain
CG_DECLVK(vkAcquireNextImageKHR); // VK_KHR_swapchain
CG_DECLVK(vkQueuePresentKHR); // VK_KHR_swapchain

CG_NS_END

#endif // YF_CG_VK_H
