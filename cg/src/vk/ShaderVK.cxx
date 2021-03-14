//
// CG
// ShaderVK.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <fstream>
#include <memory>
#include <cstring>
#include <cwchar>

#include "ShaderVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

ShaderVK::ShaderVK(Stage stage, wstring&& codeFile, wstring&& entryPoint)
  : Shader(stage, move(codeFile), move(entryPoint)) {

  if (codeFile_.empty() || entryPoint_.empty())
    throw invalid_argument("ShaderVK requires valid codeFile and entryPoint");

  const wchar_t* wsrc;
  mbstate_t state;

  // Convert path string
  char path[256];
  wsrc = codeFile_.data();
  memset(&state, 0, sizeof state);
  wcsrtombs(path, &wsrc, sizeof path, &state);
  if (wsrc)
    throw LimitExcept("Could not convert shader code file path");

  // Get shader code data and create module
  ifstream ifs(path);
  if (!ifs)
    throw FileExcept("Could not open file");

  ifs.seekg(0, ios_base::end);
  const auto sz = ifs.tellg();
  if (sz == 0 || sz%4 != 0)
    throw FileExcept("Invalid file");

  ifs.seekg(0);
  auto buf = make_unique<char[]>(sz);
  if (!ifs.read(buf.get(), sz))
    throw FileExcept("Could not read data from file");

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
  wsrc = entryPoint_.data();
  memset(&state, 0, sizeof state);
  wcsrtombs(name_, &wsrc, sizeof name_, &state);
  if (wsrc)
    throw LimitExcept("Could not set shader function name");
}

ShaderVK::~ShaderVK() {
  // TODO: notify
  auto dev = deviceVK().device();
  vkDestroyShaderModule(dev, module_, nullptr);
}

VkShaderModule ShaderVK::module() const {
  return module_;
}

const char* ShaderVK::name() const {
  return name_;
}
