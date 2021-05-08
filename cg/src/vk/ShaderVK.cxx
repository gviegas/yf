//
// CG
// ShaderVK.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <cwchar>
#include <cstring>
#include <memory>
#include <fstream>

#include "ShaderVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

ShaderVK::ShaderVK(Stage stage, const wstring& codeFile,
                   const wstring& entryPoint)
  : Shader(stage, codeFile, entryPoint) {

  if (codeFile_.empty() || entryPoint_.empty())
    throw invalid_argument("ShaderVK requires valid codeFile and entryPoint");

  const wchar_t* wsrc;
  size_t len;
  mbstate_t state;

  // Convert path string
  string path{};
  len = (codeFile_.size() + 1) * sizeof(wchar_t);
  path.resize(len);
  wsrc = codeFile_.data();
  memset(&state, 0, sizeof state);
  len = wcsrtombs(path.data(), &wsrc, path.size(), &state);
  if (wsrc || static_cast<size_t>(-1) == len)
    throw ConversionExcept("Could not convert shader code file path");
  path.resize(len);

  // Get shader code data and create module
  ifstream ifs(path);
  if (!ifs)
    throw FileExcept("Could not open shader file");

  ifs.seekg(0, ios_base::end);
  const auto sz = ifs.tellg();
  if (sz == 0 || sz%4 != 0)
    throw FileExcept("Invalid shader file");

  ifs.seekg(0);
  auto buf = make_unique<char[]>(sz);
  if (!ifs.read(buf.get(), sz))
    throw FileExcept("Could not read data from shader file");

  VkShaderModuleCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.codeSize = sz;
  info.pCode = reinterpret_cast<uint32_t*>(buf.get());

  auto dev = deviceVK().device();
  auto res = vkCreateShaderModule(dev, &info, nullptr, &module_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Could not create shader module");

  // Set shader function name
  len = (entryPoint_.size() + 1) * sizeof(wchar_t);
  name_.resize(len);
  wsrc = entryPoint_.data();
  memset(&state, 0, sizeof state);
  len = wcsrtombs(name_.data(), &wsrc, name_.size(), &state);
  if (wsrc || static_cast<size_t>(-1) == len)
    throw ConversionExcept("Could not set shader function name");
  name_.resize(len);
}

ShaderVK::~ShaderVK() {
  // TODO: notify
  auto dev = deviceVK().device();
  vkDestroyShaderModule(dev, module_, nullptr);
}

VkShaderModule ShaderVK::module() {
  return module_;
}

const string& ShaderVK::name() const {
  return name_;
}
