//
// CG
// ShaderVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <fstream>
#include <filesystem>
#include <memory>

#include "ShaderVK.h"
#include "DeviceVK.h"
#include "yf/Except.h"

using namespace CG_NS;
using namespace std;

ShaderVK::ShaderVK(Stage stage, wstring&& codeFile, wstring&& entryPoint)
  : Shader(stage, move(codeFile), move(entryPoint)) {

  ifstream ifs(filesystem::path{codeFile});
  if (!ifs)
    throw FileExcept("Could not open file");

  ifs.seekg(0, ios_base::end);
  const auto sz = ifs.tellg();
  if (sz == 0 || sz%4 != 0)
    throw FileExcept("Invalid file");

  ifs.seekg(0);
  auto buf = make_unique<char[]>(sz);
  if (!ifs.read(buf.get(), sz))
    throw FileExcept("Failed to read data from file");

  VkShaderModuleCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.codeSize = sz;
  info.pCode = reinterpret_cast<uint32_t*>(buf.get());

  auto dev = DeviceVK::get().device();
  auto res = vkCreateShaderModule(dev, &info, nullptr, &module_);
  if (res != VK_SUCCESS)
    throw DeviceExcept("Failed to create shader module");
}

ShaderVK::~ShaderVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyShaderModule(dev, module_, nullptr);
}

VkShaderModule ShaderVK::module() const {
  return module_;
}
