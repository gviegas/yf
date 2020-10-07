//
// yf
// CGShader.h
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_SHADER_H
#define YF_CG_SHADER_H

#include <string>

#include "YFDefs.h"

YF_NS_BEGIN

/// Programmable shader stages.
///
enum CGStage {
  CGStageVertex,
  CGStageTesCntrl,
  CGStageTesEval,
  CGStageGeometry,
  CGStageFragment,
  CGStageCompute
};

class CGShader {
 public:
  CGShader(CGStage stage,
           std::wstring&& codeFile,
           std::wstring&& entryPoint = L"main");

  virtual ~CGShader() = 0;

  /// Instance constants.
  ///
  const CGStage      stage;
  const std::wstring codeFile;
  const std::wstring entryPoint;
};

YF_NS_END

#endif // YF_CG_SHADER_H
