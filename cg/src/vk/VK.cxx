//
// CG
// VK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <stdexcept>

#include "VK.h"
#include "yf/cg/Defs.h"

#if defined(__linux__)
# include <dlfcn.h>
# define CG_LIBVK "libvulkan.so.1"
#elif defined(__APPLE__)
# include <dlfcn.h>
# define CG_LIBVK "libvulkan.dylib"
#elif defined(_WIN32)
# include <windows.h>
# define CG_LIBVK "vulkan-1.dll"
#else
# error "Invalid platform"
#endif

using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

/// Lib handle.
///
void* libHandle = nullptr;

#if defined(__linux__) || defined(__APPLE__)
/// Loads VK lib and retrieves `vkGetInstanceProcAddr` symbol.
///
inline bool loadVK() {
  if (libHandle)
    return true;

  void* handle = dlopen(CG_LIBVK, RTLD_LAZY);
  if (!handle)
    return false;

  void* sym = dlsym(handle, "vkGetInstanceProcAddr");
  if (!sym) {
    dlclose(handle);
    return false;
  }

  libHandle = handle;
  vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(sym);

  return true;
}

/// Unloads VK lib.
///
inline void unloadVK() {
  if (libHandle) {
    dlclose(libHandle);
    libHandle = nullptr;
  }
}
#elif defined(_WIN32)
/// Loads VK lib and retrieves `vkGetInstanceProcAddr` symbol.
///
inline bool loadVK() {
  // TODO
  throw runtime_error("Unimplemented);"
}

/// Unloads VK lib.
///
inline void unloadVK() {
  // TODO
  throw runtime_error("Unimplemented);"
}
#else
# error "Invalid platform"
#endif // defined(__linux__) || defined(__APPLE__)

INTERNAL_NS_END

bool CG_NS::initVK() {
  return loadVK();
}

void CG_NS::setProcsVK(VkInstance instance) {
  if (!libHandle && !loadVK())
    // TODO
    throw runtime_error("Failed to load VK lib");

  if (!instance) {
    CG_INSTPROCVK(nullptr, vkGetInstanceProcAddr);
    CG_INSTPROCVK(nullptr, vkEnumerateInstanceVersion);
    CG_INSTPROCVK(nullptr, vkEnumerateInstanceExtensionProperties);
    CG_INSTPROCVK(nullptr, vkEnumerateInstanceLayerProperties);
    CG_INSTPROCVK(nullptr, vkCreateInstance);
  } else {
    CG_INSTPROCVK(instance, vkDestroyInstance);
    CG_INSTPROCVK(instance, vkEnumeratePhysicalDevices);
    CG_INSTPROCVK(instance, vkGetPhysicalDeviceProperties);
    CG_INSTPROCVK(instance, vkGetPhysicalDeviceQueueFamilyProperties);
    CG_INSTPROCVK(instance, vkGetPhysicalDeviceMemoryProperties);
    CG_INSTPROCVK(instance, vkGetPhysicalDeviceFormatProperties);
    CG_INSTPROCVK(instance, vkGetPhysicalDeviceFeatures);
    CG_INSTPROCVK(instance, vkGetPhysicalDeviceProperties2);
    CG_INSTPROCVK(instance, vkGetPhysicalDeviceQueueFamilyProperties2);
    CG_INSTPROCVK(instance, vkGetPhysicalDeviceMemoryProperties2);
    CG_INSTPROCVK(instance, vkGetPhysicalDeviceFormatProperties2);
    CG_INSTPROCVK(instance, vkGetPhysicalDeviceFeatures2);
    CG_INSTPROCVK(instance, vkEnumerateDeviceExtensionProperties);
    CG_INSTPROCVK(instance, vkCreateDevice);
    CG_INSTPROCVK(instance, vkDestroySurfaceKHR);
    CG_INSTPROCVK(instance, vkGetPhysicalDeviceSurfaceSupportKHR);
    CG_INSTPROCVK(instance, vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    CG_INSTPROCVK(instance, vkGetPhysicalDeviceSurfaceFormatsKHR);
    CG_INSTPROCVK(instance, vkGetPhysicalDeviceSurfacePresentModesKHR);
    CG_INSTPROCVK(instance, vkGetDeviceProcAddr);
  }
}

void CG_NS::setProcsVK(VkDevice device) {
  if (!device)
    // TODO
    throw invalid_argument("setProcsVK() expects a valid device handle");

  if (!libHandle || !loadVK())
    // TODO
    throw runtime_error("Failed to load VK lib");

  CG_DEVPROCVK(device, vkGetDeviceProcAddr);
  CG_DEVPROCVK(device, vkDestroyDevice);
  CG_DEVPROCVK(device, vkGetDeviceQueue);
  CG_DEVPROCVK(device, vkCreateCommandPool);
  CG_DEVPROCVK(device, vkTrimCommandPool);
  CG_DEVPROCVK(device, vkResetCommandPool);
  CG_DEVPROCVK(device, vkDestroyCommandPool);
  CG_DEVPROCVK(device, vkAllocateCommandBuffers);
  CG_DEVPROCVK(device, vkResetCommandBuffer);
  CG_DEVPROCVK(device, vkFreeCommandBuffers);
  CG_DEVPROCVK(device, vkBeginCommandBuffer);
  CG_DEVPROCVK(device, vkEndCommandBuffer);
  CG_DEVPROCVK(device, vkQueueSubmit);
  CG_DEVPROCVK(device, vkCmdExecuteCommands);
  CG_DEVPROCVK(device, vkCreateFence);
  CG_DEVPROCVK(device, vkDestroyFence);
  CG_DEVPROCVK(device, vkGetFenceStatus);
  CG_DEVPROCVK(device, vkResetFences);
  CG_DEVPROCVK(device, vkWaitForFences);
  CG_DEVPROCVK(device, vkCreateSemaphore);
  CG_DEVPROCVK(device, vkDestroySemaphore);
  CG_DEVPROCVK(device, vkGetSemaphoreCounterValue);
  CG_DEVPROCVK(device, vkWaitSemaphores);
  CG_DEVPROCVK(device, vkSignalSemaphore);
  CG_DEVPROCVK(device, vkCmdPipelineBarrier);
  CG_DEVPROCVK(device, vkQueueWaitIdle);
  CG_DEVPROCVK(device, vkDeviceWaitIdle);
  CG_DEVPROCVK(device, vkCreateRenderPass);
  CG_DEVPROCVK(device, vkDestroyRenderPass);
  CG_DEVPROCVK(device, vkCreateFramebuffer);
  CG_DEVPROCVK(device, vkDestroyFramebuffer);
  CG_DEVPROCVK(device, vkCmdBeginRenderPass);
  CG_DEVPROCVK(device, vkCmdEndRenderPass);
  CG_DEVPROCVK(device, vkCmdNextSubpass);
  CG_DEVPROCVK(device, vkCreateShaderModule);
  CG_DEVPROCVK(device, vkDestroyShaderModule);
  CG_DEVPROCVK(device, vkCreateGraphicsPipelines);
  CG_DEVPROCVK(device, vkCreateComputePipelines);
  CG_DEVPROCVK(device, vkDestroyPipeline);
  CG_DEVPROCVK(device, vkCmdBindPipeline);
  CG_DEVPROCVK(device, vkCreatePipelineCache);
  CG_DEVPROCVK(device, vkMergePipelineCaches);
  CG_DEVPROCVK(device, vkGetPipelineCacheData);
  CG_DEVPROCVK(device, vkDestroyPipelineCache);
  CG_DEVPROCVK(device, vkAllocateMemory);
  CG_DEVPROCVK(device, vkFreeMemory);
  CG_DEVPROCVK(device, vkMapMemory);
  CG_DEVPROCVK(device, vkUnmapMemory);
  CG_DEVPROCVK(device, vkCreateBuffer);
  CG_DEVPROCVK(device, vkDestroyBuffer);
  CG_DEVPROCVK(device, vkCreateBufferView);
  CG_DEVPROCVK(device, vkDestroyBufferView);
  CG_DEVPROCVK(device, vkCreateImage);
  CG_DEVPROCVK(device, vkDestroyImage);
  CG_DEVPROCVK(device, vkCreateImageView);
  CG_DEVPROCVK(device, vkDestroyImageView);
  CG_DEVPROCVK(device, vkGetBufferMemoryRequirements);
  CG_DEVPROCVK(device, vkGetImageMemoryRequirements);
  CG_DEVPROCVK(device, vkBindBufferMemory);
  CG_DEVPROCVK(device, vkBindImageMemory);
  CG_DEVPROCVK(device, vkCreateSampler);
  CG_DEVPROCVK(device, vkDestroySampler);
  CG_DEVPROCVK(device, vkCreateDescriptorSetLayout);
  CG_DEVPROCVK(device, vkDestroyDescriptorSetLayout);
  CG_DEVPROCVK(device, vkGetDescriptorSetLayoutSupport);
  CG_DEVPROCVK(device, vkCreatePipelineLayout);
  CG_DEVPROCVK(device, vkDestroyPipelineLayout);
  CG_DEVPROCVK(device, vkCreateDescriptorPool);
  CG_DEVPROCVK(device, vkDestroyDescriptorPool);
  CG_DEVPROCVK(device, vkResetDescriptorPool);
  CG_DEVPROCVK(device, vkAllocateDescriptorSets);
  CG_DEVPROCVK(device, vkFreeDescriptorSets);
  CG_DEVPROCVK(device, vkUpdateDescriptorSets);
  CG_DEVPROCVK(device, vkCmdBindDescriptorSets);
  CG_DEVPROCVK(device, vkCmdPushConstants);
  CG_DEVPROCVK(device, vkCmdClearColorImage);
  CG_DEVPROCVK(device, vkCmdClearDepthStencilImage);
  CG_DEVPROCVK(device, vkCmdClearAttachments);
  CG_DEVPROCVK(device, vkCmdFillBuffer);
  CG_DEVPROCVK(device, vkCmdUpdateBuffer);
  CG_DEVPROCVK(device, vkCmdCopyBuffer);
  CG_DEVPROCVK(device, vkCmdCopyImage);
  CG_DEVPROCVK(device, vkCmdCopyBufferToImage);
  CG_DEVPROCVK(device, vkCmdCopyImageToBuffer);
  CG_DEVPROCVK(device, vkCmdBlitImage);
  CG_DEVPROCVK(device, vkCmdResolveImage);
  CG_DEVPROCVK(device, vkCmdDraw);
  CG_DEVPROCVK(device, vkCmdDrawIndexed);
  CG_DEVPROCVK(device, vkCmdDrawIndirect);
  CG_DEVPROCVK(device, vkCmdDrawIndexedIndirect);
  CG_DEVPROCVK(device, vkCmdBindVertexBuffers);
  CG_DEVPROCVK(device, vkCmdBindIndexBuffer);
  CG_DEVPROCVK(device, vkCmdSetViewport);
  CG_DEVPROCVK(device, vkCmdSetLineWidth);
  CG_DEVPROCVK(device, vkCmdSetDepthBias);
  CG_DEVPROCVK(device, vkCmdSetScissor);
  CG_DEVPROCVK(device, vkCmdSetDepthBounds);
  CG_DEVPROCVK(device, vkCmdSetStencilCompareMask);
  CG_DEVPROCVK(device, vkCmdSetStencilWriteMask);
  CG_DEVPROCVK(device, vkCmdSetStencilReference);
  CG_DEVPROCVK(device, vkCmdSetBlendConstants);
  CG_DEVPROCVK(device, vkCmdDispatch);
  CG_DEVPROCVK(device, vkCmdDispatchIndirect);
  CG_DEVPROCVK(device, vkCreateSwapchainKHR);
  CG_DEVPROCVK(device, vkDestroySwapchainKHR);
  CG_DEVPROCVK(device, vkGetSwapchainImagesKHR);
  CG_DEVPROCVK(device, vkAcquireNextImageKHR);
  CG_DEVPROCVK(device, vkQueuePresentKHR);
}

void CG_NS::deinitVK() {
  unloadVK();
}

/// Instance-level procedures (null instance).
///
CG_DEFVK(vkGetInstanceProcAddr);
CG_DEFVK(vkEnumerateInstanceVersion); // 1.1
CG_DEFVK(vkEnumerateInstanceExtensionProperties);
CG_DEFVK(vkEnumerateInstanceLayerProperties);
CG_DEFVK(vkCreateInstance);

/// Instance-level procedures (non-null instance).
///
CG_DEFVK(vkDestroyInstance);
CG_DEFVK(vkEnumeratePhysicalDevices);
CG_DEFVK(vkGetPhysicalDeviceProperties);
CG_DEFVK(vkGetPhysicalDeviceQueueFamilyProperties);
CG_DEFVK(vkGetPhysicalDeviceMemoryProperties);
CG_DEFVK(vkGetPhysicalDeviceFormatProperties);
CG_DEFVK(vkGetPhysicalDeviceFeatures);
CG_DEFVK(vkGetPhysicalDeviceProperties2); // 1.1
CG_DEFVK(vkGetPhysicalDeviceQueueFamilyProperties2); // 1.1
CG_DEFVK(vkGetPhysicalDeviceMemoryProperties2); // 1.1
CG_DEFVK(vkGetPhysicalDeviceFormatProperties2); // 1.1
CG_DEFVK(vkGetPhysicalDeviceFeatures2); // 1.1
CG_DEFVK(vkEnumerateDeviceExtensionProperties);
CG_DEFVK(vkCreateDevice);
CG_DEFVK(vkDestroySurfaceKHR); // VK_KHR_surface
CG_DEFVK(vkGetPhysicalDeviceSurfaceSupportKHR); // VK_KHR_surface
CG_DEFVK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR); // VK_KHR_surface
CG_DEFVK(vkGetPhysicalDeviceSurfaceFormatsKHR); // VK_KHR_surface
CG_DEFVK(vkGetPhysicalDeviceSurfacePresentModesKHR); // VK_KHR_surface

/// Device-level procedures.
///
CG_DEFVK(vkGetDeviceProcAddr);
CG_DEFVK(vkDestroyDevice);
CG_DEFVK(vkGetDeviceQueue);
CG_DEFVK(vkCreateCommandPool);
CG_DEFVK(vkTrimCommandPool); // 1.1
CG_DEFVK(vkResetCommandPool);
CG_DEFVK(vkDestroyCommandPool);
CG_DEFVK(vkAllocateCommandBuffers);
CG_DEFVK(vkResetCommandBuffer);
CG_DEFVK(vkFreeCommandBuffers);
CG_DEFVK(vkBeginCommandBuffer);
CG_DEFVK(vkEndCommandBuffer);
CG_DEFVK(vkQueueSubmit);
CG_DEFVK(vkCmdExecuteCommands);
CG_DEFVK(vkCreateFence);
CG_DEFVK(vkDestroyFence);
CG_DEFVK(vkGetFenceStatus);
CG_DEFVK(vkResetFences);
CG_DEFVK(vkWaitForFences);
CG_DEFVK(vkCreateSemaphore);
CG_DEFVK(vkDestroySemaphore);
CG_DEFVK(vkGetSemaphoreCounterValue); // 1.2
CG_DEFVK(vkWaitSemaphores); // 1.2
CG_DEFVK(vkSignalSemaphore); // 1.2
CG_DEFVK(vkCmdPipelineBarrier);
CG_DEFVK(vkQueueWaitIdle);
CG_DEFVK(vkDeviceWaitIdle);
CG_DEFVK(vkCreateRenderPass);
CG_DEFVK(vkDestroyRenderPass);
CG_DEFVK(vkCreateFramebuffer);
CG_DEFVK(vkDestroyFramebuffer);
CG_DEFVK(vkCmdBeginRenderPass);
CG_DEFVK(vkCmdEndRenderPass);
CG_DEFVK(vkCmdNextSubpass);
CG_DEFVK(vkCreateShaderModule);
CG_DEFVK(vkDestroyShaderModule);
CG_DEFVK(vkCreateGraphicsPipelines);
CG_DEFVK(vkCreateComputePipelines);
CG_DEFVK(vkDestroyPipeline);
CG_DEFVK(vkCmdBindPipeline);
CG_DEFVK(vkCreatePipelineCache);
CG_DEFVK(vkMergePipelineCaches);
CG_DEFVK(vkGetPipelineCacheData);
CG_DEFVK(vkDestroyPipelineCache);
CG_DEFVK(vkAllocateMemory);
CG_DEFVK(vkFreeMemory);
CG_DEFVK(vkMapMemory);
CG_DEFVK(vkUnmapMemory);
CG_DEFVK(vkCreateBuffer);
CG_DEFVK(vkDestroyBuffer);
CG_DEFVK(vkCreateBufferView);
CG_DEFVK(vkDestroyBufferView);
CG_DEFVK(vkCreateImage);
CG_DEFVK(vkDestroyImage);
CG_DEFVK(vkCreateImageView);
CG_DEFVK(vkDestroyImageView);
CG_DEFVK(vkGetBufferMemoryRequirements);
CG_DEFVK(vkGetImageMemoryRequirements);
CG_DEFVK(vkBindBufferMemory);
CG_DEFVK(vkBindImageMemory);
CG_DEFVK(vkCreateSampler);
CG_DEFVK(vkDestroySampler);
CG_DEFVK(vkCreateDescriptorSetLayout);
CG_DEFVK(vkDestroyDescriptorSetLayout);
CG_DEFVK(vkGetDescriptorSetLayoutSupport); // 1.1
CG_DEFVK(vkCreatePipelineLayout);
CG_DEFVK(vkDestroyPipelineLayout);
CG_DEFVK(vkCreateDescriptorPool);
CG_DEFVK(vkDestroyDescriptorPool);
CG_DEFVK(vkResetDescriptorPool);
CG_DEFVK(vkAllocateDescriptorSets);
CG_DEFVK(vkFreeDescriptorSets);
CG_DEFVK(vkUpdateDescriptorSets);
CG_DEFVK(vkCmdBindDescriptorSets);
CG_DEFVK(vkCmdPushConstants);
CG_DEFVK(vkCmdClearColorImage);
CG_DEFVK(vkCmdClearDepthStencilImage);
CG_DEFVK(vkCmdClearAttachments);
CG_DEFVK(vkCmdFillBuffer);
CG_DEFVK(vkCmdUpdateBuffer);
CG_DEFVK(vkCmdCopyBuffer);
CG_DEFVK(vkCmdCopyImage);
CG_DEFVK(vkCmdCopyBufferToImage);
CG_DEFVK(vkCmdCopyImageToBuffer);
CG_DEFVK(vkCmdBlitImage);
CG_DEFVK(vkCmdResolveImage);
CG_DEFVK(vkCmdDraw);
CG_DEFVK(vkCmdDrawIndexed);
CG_DEFVK(vkCmdDrawIndirect);
CG_DEFVK(vkCmdDrawIndexedIndirect);
CG_DEFVK(vkCmdBindVertexBuffers);
CG_DEFVK(vkCmdBindIndexBuffer);
CG_DEFVK(vkCmdSetViewport);
CG_DEFVK(vkCmdSetLineWidth);
CG_DEFVK(vkCmdSetDepthBias);
CG_DEFVK(vkCmdSetScissor);
CG_DEFVK(vkCmdSetDepthBounds);
CG_DEFVK(vkCmdSetStencilCompareMask);
CG_DEFVK(vkCmdSetStencilWriteMask);
CG_DEFVK(vkCmdSetStencilReference);
CG_DEFVK(vkCmdSetBlendConstants);
CG_DEFVK(vkCmdDispatch);
CG_DEFVK(vkCmdDispatchIndirect);
CG_DEFVK(vkCreateSwapchainKHR); // VK_KHR_swapchain
CG_DEFVK(vkDestroySwapchainKHR); // VK_KHR_swapchain
CG_DEFVK(vkGetSwapchainImagesKHR); // VK_KHR_swapchain
CG_DEFVK(vkAcquireNextImageKHR); // VK_KHR_swapchain
CG_DEFVK(vkQueuePresentKHR); // VK_KHR_swapchain
