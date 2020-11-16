//
// CG
// StateVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "StateVK.h"
#include "ShaderVK.h"
#include "DcTableVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

GrStateVK::GrStateVK(const Config& config) : GrState(config) {
  // TODO
  throw runtime_error("Unimplemented");
}

GrStateVK::~GrStateVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyPipeline(dev, pipeline_, nullptr);
  vkDestroyPipelineLayout(dev, plLayout_, nullptr);
}

VkPipeline GrStateVK::pipeline() const {
  return pipeline_;
}

CpStateVK::CpStateVK(const Config& config) : CpState(config) {
  if (!config.shader || config.shader->stage_ != StageCompute)
    throw invalid_argument("CpStateVK requires a compute shader");

  auto dev = DeviceVK::get().device();
  VkResult res;

  // Create pipeline layout
  vector<VkDescriptorSetLayout> dsLays;
  for (const auto dtb : config.dcTables)
    // XXX: assuming non-null
    dsLays.push_back(static_cast<DcTableVK*>(dtb)->dsLayout());

  VkPipelineLayoutCreateInfo layInfo;
  layInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layInfo.pNext = nullptr;
  layInfo.flags = 0;
  layInfo.setLayoutCount = dsLays.size();
  layInfo.pSetLayouts = dsLays.data();
  layInfo.pushConstantRangeCount = 0;
  layInfo.pPushConstantRanges = nullptr;

  res = vkCreatePipelineLayout(dev, &layInfo, nullptr, &plLayout_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create pipeline layout");

  // Define shader stage
  VkPipelineShaderStageCreateInfo stgInfo;
  stgInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  stgInfo.pNext = nullptr;
  stgInfo.flags = 0;
  stgInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  stgInfo.module = static_cast<ShaderVK*>(config.shader)->module();
  stgInfo.pName = static_cast<ShaderVK*>(config.shader)->name();
  stgInfo.pSpecializationInfo = nullptr;

  // Create pipeline
  VkComputePipelineCreateInfo plInfo;
  plInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  plInfo.pNext = nullptr;
  plInfo.flags = 0;
  plInfo.stage = stgInfo;
  plInfo.layout = plLayout_;
  plInfo.basePipelineHandle = VK_NULL_HANDLE;
  plInfo.basePipelineIndex = -1;

  // TODO: pipeline cache
  res = vkCreateComputePipelines(dev, nullptr, 1, &plInfo, nullptr,
                                 &pipeline_);
  if (res != VK_SUCCESS) {
    vkDestroyPipelineLayout(dev, plLayout_, nullptr);
    throw DeviceExcept("Could not create compute pipeline");
  }
}

CpStateVK::~CpStateVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyPipeline(dev, pipeline_, nullptr);
  vkDestroyPipelineLayout(dev, plLayout_, nullptr);
}

VkPipeline CpStateVK::pipeline() const {
  return pipeline_;
}
