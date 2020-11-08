//
// cg
// ShaderVK.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <fstream>
#include <filesystem>
#include <memory>

#include "ShaderVK.h"
#include "DeviceVK.h"

using namespace CG_NS;
using namespace std;

ShaderVK::ShaderVK(Stage stage, wstring&& codeFile, wstring&& entryPoint)
  : Shader(stage, move(codeFile), move(entryPoint)) {

  ifstream ifs(filesystem::path{codeFile});
  if (!ifs)
    // TODO
    throw runtime_error("Could not open file");

  ifs.seekg(0, ios_base::end);
  const auto sz = ifs.tellg();
  if (sz == 0 || sz%4 != 0)
    // TODO
    throw runtime_error("Invalid file");

  ifs.seekg(0);
  auto buf = make_unique<char[]>(sz);
  if (!ifs.read(buf.get(), sz))
    // TODO
    throw runtime_error("Failed to read data from file");

  VkShaderModuleCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.codeSize = sz;
  info.pCode = reinterpret_cast<uint32_t*>(buf.get());

  auto dev = DeviceVK::get().device();
  auto res = vkCreateShaderModule(dev, &info, nullptr, &_module);
  if (res != VK_SUCCESS)
    // TODO
    throw runtime_error("Failed to create shader module");
}

ShaderVK::~ShaderVK() {
  // TODO: notify
  auto dev = DeviceVK::get().device();
  vkDestroyShaderModule(dev, _module, nullptr);
}

VkShaderModule ShaderVK::module() const {
  return _module;
}
