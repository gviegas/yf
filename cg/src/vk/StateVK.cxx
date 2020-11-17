//
// CG
// StateVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "StateVK.h"
#include "ShaderVK.h"
#include "DcTableVK.h"
#include "PassVK.h"
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

// ------------------------------------------------------------------------
// GrStateVK

GrStateVK::GrStateVK(const Config& config)
  : GrState(config), stgFlags_(0), plLayout_(plLayoutVK(config.dcTables)) {

  auto dev = DeviceVK::get().device();
  auto deinit = [&] { vkDestroyPipelineLayout(dev, plLayout_, nullptr); };

  if (!config.pass) {
    deinit();
    throw invalid_argument("GrStateVK requires a valid pass");
  }

  // Define shader stages
  vector<VkPipelineShaderStageCreateInfo> stgInfos;

  for (const auto shd : config.shaders) {
    // XXX: assuming non-null
    stgInfos.push_back({});
    auto& info = stgInfos.back();
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.stage = toShaderStageVK(shd->stage_);
    info.module = static_cast<ShaderVK*>(shd)->module();
    info.pName = static_cast<ShaderVK*>(shd)->name();
    info.pSpecializationInfo = nullptr;

    if (info.stage & stgFlags_) {
      deinit();
      throw invalid_argument("GrStateVK requires a unique set of stages");
    }

    stgFlags_ |= info.stage;
  }

  if (!(stgFlags_ & VK_SHADER_STAGE_VERTEX_BIT)) {
    deinit();
    throw invalid_argument("GrStateVK requires a vertex shader");
  }
  // TODO: check other invalid stage combinations

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
  vpInfo.flags = 0;
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
  depInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depInfo.pNext = nullptr;
  depInfo.flags = 0;
  depInfo.depthTestEnable = true;
  depInfo.depthWriteEnable = true;
  depInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
  depInfo.depthBoundsTestEnable = false;

  // TODO: stencil
  depInfo.stencilTestEnable = false;
  depInfo.front.failOp = VK_STENCIL_OP_KEEP;
  depInfo.front.passOp = VK_STENCIL_OP_KEEP;
  depInfo.front.depthFailOp = VK_STENCIL_OP_KEEP;
  depInfo.front.compareOp = VK_COMPARE_OP_ALWAYS;
  depInfo.front.compareMask = 0;
  depInfo.front.writeMask = 0;
  depInfo.front.reference = 0;
  depInfo.back.failOp = VK_STENCIL_OP_KEEP;
  depInfo.back.passOp = VK_STENCIL_OP_KEEP;
  depInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
  depInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
  depInfo.back.compareMask = 0;
  depInfo.back.writeMask = 0;
  depInfo.back.reference = 0;

  depInfo.minDepthBounds = 0.0f;
  depInfo.maxDepthBounds = 0.0f;

  // Define color blend state
  VkPipelineColorBlendAttachmentState cbdAtt;
  cbdAtt.blendEnable = true;
  cbdAtt.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  cbdAtt.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  cbdAtt.colorBlendOp = VK_BLEND_OP_ADD;
  cbdAtt.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  cbdAtt.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  cbdAtt.alphaBlendOp = VK_BLEND_OP_ADD;
  cbdAtt.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                          VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT |
                          VK_COLOR_COMPONENT_A_BIT;

  vector<VkPipelineColorBlendAttachmentState>
  cbdAtts(config.pass->colors_ ? config.pass->colors_->size() : 0, cbdAtt);

  VkPipelineColorBlendStateCreateInfo cbdInfo;
  cbdInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  cbdInfo.pNext = nullptr;
  cbdInfo.flags = 0;
  cbdInfo.logicOpEnable = false;
  cbdInfo.logicOp = VK_LOGIC_OP_NO_OP;
  cbdInfo.attachmentCount = cbdAtts.size();
  cbdInfo.pAttachments = cbdAtts.data();
  cbdInfo.blendConstants[0] = 1.0f;
  cbdInfo.blendConstants[1] = 1.0f;
  cbdInfo.blendConstants[2] = 1.0f;
  cbdInfo.blendConstants[3] = 1.0f;

  // Define dynamic state
  // TODO: other dynamic states
  vector<VkDynamicState> dynStates{VK_DYNAMIC_STATE_VIEWPORT,
                                   VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynInfo;
  dynInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynInfo.pNext = nullptr;
  dynInfo.flags = 0;
  dynInfo.dynamicStateCount = dynStates.size();
  dynInfo.pDynamicStates = dynStates.data();

  // Create pipeline
  VkGraphicsPipelineCreateInfo plInfo;
  plInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  plInfo.pNext = nullptr;
  plInfo.flags = 0;
  plInfo.stageCount = stgInfos.size();
  plInfo.pStages = stgInfos.data();
  plInfo.pVertexInputState = &vxInfo;
  plInfo.pInputAssemblyState = &iaInfo;
  plInfo.pTessellationState = &tesInfo;
  plInfo.pViewportState = &vpInfo;
  plInfo.pRasterizationState = &rzInfo;
  plInfo.pMultisampleState = &msInfo;
  plInfo.pDepthStencilState = &depInfo;
  plInfo.pColorBlendState = &cbdInfo;
  plInfo.pDynamicState = &dynInfo;
  plInfo.layout = plLayout_;
  plInfo.renderPass = static_cast<PassVK*>(config.pass)->renderPass();
  plInfo.subpass = 0;
  plInfo.basePipelineHandle = VK_NULL_HANDLE;
  plInfo.basePipelineIndex = -1;

  // TODO: pipeline cache
  auto res = vkCreateGraphicsPipelines(dev, nullptr, 1, &plInfo, nullptr,
                                       &pipeline_);
  if (res != VK_SUCCESS) {
    deinit();
    throw DeviceExcept("Could not create graphics pipeline");
  }
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

// ------------------------------------------------------------------------
// CpStateVK

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
