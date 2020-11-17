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

INTERNAL_NS_BEGIN

/// Creates pipeline layout object.
///
inline VkPipelineLayout plLayoutVK(const vector<DcTable*>& dcTables) {
  vector<VkDescriptorSetLayout> dsLays;
  for (const auto dtb : dcTables)
    // XXX: assuming non-null
    dsLays.push_back(static_cast<DcTableVK*>(dtb)->dsLayout());

  VkPipelineLayoutCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.setLayoutCount = dsLays.size();
  info.pSetLayouts = dsLays.data();
  info.pushConstantRangeCount = 0;
  info.pPushConstantRanges = nullptr;

  VkPipelineLayout plLay;
  auto dev = DeviceVK::get().device();
  auto res = vkCreatePipelineLayout(dev, &info, nullptr, &plLay);
  if (res != VK_SUCCESS)
    throw yf::DeviceExcept("Could not create pipeline layout");

  return plLay;
}

INTERNAL_NS_END

GrStateVK::GrStateVK(const Config& config)
  : GrState(config), stgFlags_(0), plLayout_(plLayoutVK(config.dcTables)) {

  auto dev = DeviceVK::get().device();
  auto deinit = [&] { vkDestroyPipelineLayout(dev, plLayout_, nullptr); };

  // Define shader stages
  vector<VkPipelineShaderStageCreateInfo> stgInfos;

  for (const auto shd : config.shaders) {
    // XXX: assuming non-null
    stgInfos.push_back({});
    auto info = stgInfos.back();
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.stage = toShaderStageVK(shd->stage_);
    info.module = static_cast<ShaderVK*>(shd)->module();
    info.pName = static_cast<ShaderVK*>(shd)->name();
    info.pSpecializationInfo = nullptr;

    if (info.stage & stgFlags_) {
      deinit();
      throw invalid_argument("Non-unique shader stages on pipeline creation");
    }

    stgFlags_ |= info.stage;
  }

  if (!(stgFlags_ & VK_SHADER_STAGE_VERTEX_BIT)) {
    deinit();
    throw invalid_argument("Graphics pipeline requires a vertex shader");
  }
  // TODO: check other invalid stage combinations

  // TODO...

  // Define vertex input state
  vector<VkVertexInputBindingDescription> vxBinds;
  vector<VkVertexInputAttributeDescription> vxAttrs;

  // TODO: validate
  for (uint32_t bind = 0; bind < config.vxInputs.size(); ++bind) {
    const auto& in = config.vxInputs[bind];
    vxBinds.push_back({bind, in.stride, toInputRateVK(in.stepFunction)});
    for (const auto& at : in.attributes)
      vxAttrs.push_back({at.first, bind, toFormatVK(at.second.format),
                         at.second.offset});
  }

  VkPipelineVertexInputStateCreateInfo vxInfo;
  vxInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vxInfo.pNext = nullptr;
  vxInfo.flags = 0;
  vxInfo.vertexBindingDescriptionCount = vxBinds.size();
  vxInfo.pVertexBindingDescriptions = vxBinds.data();
  vxInfo.vertexAttributeDescriptionCount = vxAttrs.size();
  vxInfo.pVertexAttributeDescriptions = vxAttrs.data();

  // Define input assembly state
  VkPipelineInputAssemblyStateCreateInfo iaInfo;
  iaInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  iaInfo.pNext = nullptr;
  iaInfo.flags = 0;
  iaInfo.topology = toTopologyVK(config.primitive);
  iaInfo.primitiveRestartEnable = false;

  // Define tessellation state
  VkPipelineTessellationStateCreateInfo tesInfo;
  tesInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
  tesInfo.pNext = nullptr;
  tesInfo.flags = 0;
  // TODO: take from `config`
  tesInfo.patchControlPoints = 16;

  // Define viewport state
  VkPipelineViewportStateCreateInfo vpInfo;
  vpInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  vpInfo.pNext = nullptr;
  // TODO: take from `config`
  vpInfo.viewportCount = 1;
  vpInfo.pViewports = nullptr; // dynamic
  vpInfo.scissorCount = 1;
  vpInfo.pScissors = nullptr; // dynamic

  // Define rasterization state
  VkPipelineRasterizationStateCreateInfo rzInfo;
  rzInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rzInfo.pNext = nullptr;
  rzInfo.flags = 0;
  rzInfo.depthClampEnable = false;
  rzInfo.rasterizerDiscardEnable = false;
  rzInfo.polygonMode = toPolygonModeVK(config.polyMode);
  rzInfo.cullMode = toCullModeVK(config.cullMode);
  rzInfo.frontFace = toFrontFaceVK(config.winding);
  rzInfo.depthBiasEnable = false;
  rzInfo.depthBiasConstantFactor = 0.0f;
  rzInfo.depthBiasClamp = 0.0f;
  rzInfo.depthBiasSlopeFactor = 0.0f;
  // TODO: take from `config`
  rzInfo.lineWidth = 1.0f;

  // Define multisample state
  // TODO: compute this value during pass creation instead
  VkSampleCountFlagBits splCount = VK_SAMPLE_COUNT_1_BIT;
  if (config.pass->colors_)
    // XXX: assuming all colors have the same sample count
    splCount = toSampleCountVK(config.pass->colors_->back().samples);
  else
    splCount = toSampleCountVK(config.pass->depthStencil_->samples);

  VkPipelineMultisampleStateCreateInfo msInfo;
  msInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  msInfo.pNext = nullptr;
  msInfo.flags = 0;
  msInfo.rasterizationSamples = splCount;
  msInfo.sampleShadingEnable = false;
  msInfo.minSampleShading = 0.0f;
  msInfo.pSampleMask = nullptr;
  msInfo.alphaToCoverageEnable = false;
  msInfo.alphaToOneEnable = false;

  // Define depth/stencil state
  VkPipelineDepthStencilStateCreateInfo depInfo;

  // Define color blend state
  VkPipelineColorBlendStateCreateInfo bndInfo;

  // Define dynamic state
  VkPipelineDynamicStateCreateInfo dynInfo;

  // TODO...
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

CpStateVK::CpStateVK(const Config& config)
  : CpState(config), plLayout_(plLayoutVK(config.dcTables)) {

  if (!config.shader || config.shader->stage_ != StageCompute)
    throw invalid_argument("CpStateVK requires a compute shader");

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

  auto dev = DeviceVK::get().device();
  // TODO: pipeline cache
  auto res = vkCreateComputePipelines(dev, nullptr, 1, &plInfo, nullptr,
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
