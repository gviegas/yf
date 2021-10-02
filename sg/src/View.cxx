//
// SG
// View.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <thread>
#include <stdexcept>

#include "yf/cg/Device.h"
#include "yf/cg/Queue.h"
#include "yf/cg/Pass.h"
#include "yf/cg/Image.h"

#include "yf/ws/Event.h"

#include "View.h"
#include "Scene.h"
#include "Camera.h"
#include "Renderer.h"

using namespace SG_NS;
using namespace std;

class View::Impl {
 public:
  Impl(WS_NS::Window& window) : wsi_(CG_NS::device().wsi(window)) {
    initTargets();
  }

  void loop(Scene* scene, uint32_t fps, const UpdateFn& update) {
    looping_ = true;
    scene_ = scene;

    const chrono::nanoseconds ipd{1'000'000'000 / fps};
    auto before = chrono::system_clock::now();
    auto now = before;

    while (update(now-before)) {
      render(scene_);

      before = now;
      now = chrono::system_clock::now();

      if (now-before < ipd) {
        this_thread::sleep_until(now + ipd-(now-before));
        now = chrono::system_clock::now();
      }

      WS_NS::dispatch();
    }

    scene_ = nullptr;
    looping_ = false;
  }

  void swapScene(Scene* newScene) {
    scene_ = newScene;
  }

  void render(Scene* scene) {
    auto nextImg = wsi_->nextImage(false);
    if (!nextImg.first || nextImg.second >= targets_.size())
      throw runtime_error("Invalid render target");

    auto nextTgt = targets_[nextImg.second].get();

    // TODO: This should only be done on `loop()` start and window resize
    scene->camera().adjust(static_cast<float>(nextTgt->size().width) /
                           static_cast<float>(nextTgt->size().height));

    renderer_.render(*scene, *nextTgt);
    // TODO: Catch broken swapchain errors
    wsi_->present(nextImg.second);
  }

 private:
  static Renderer renderer_;

  CG_NS::Wsi::Ptr wsi_{};
  CG_NS::Image::Ptr depthStencil_{};
  CG_NS::Pass::Ptr pass_{};
  vector<CG_NS::Target::Ptr> targets_{};
  bool looping_ = false;
  Scene* scene_ = nullptr;

  /// Creates/recreates targets and their dependencies.
  ///
  void initTargets() {
    if (!wsi_)
      throw runtime_error("View failed to create a valid Wsi object");

    if (pass_) {
      targets_.clear();
      pass_.reset();
      depthStencil_.reset();
    }

    auto& dev = CG_NS::device();
    const CG_NS::Size2 size{wsi_->window().width(), wsi_->window().height()};

    // Create depth/stencil image
    depthStencil_ = dev.image(CG_NS::PxFormatD16Unorm, size, 1, 1,
                              (*wsi_)[0]->samples());

    // Create pass
    const vector<CG_NS::ColorAttach> clrAtts{{(*wsi_)[0]->format(),
                                              (*wsi_)[0]->samples(),
                                              CG_NS::LoadOpDontCare,
                                              CG_NS::StoreOpStore}};

    const CG_NS::DepStenAttach dsAtt{depthStencil_->format(),
                                     depthStencil_->samples(),
                                     CG_NS::LoadOpDontCare,
                                     CG_NS::StoreOpStore,
                                     CG_NS::LoadOpDontCare,
                                     CG_NS::StoreOpDontCare};

    pass_ = dev.pass(&clrAtts, nullptr, &dsAtt);

    // Create targets
    vector<CG_NS::AttachImg> clrs{{nullptr, 0, 0}};
    const CG_NS::AttachImg ds{depthStencil_.get(), 0, 0};

    for (const auto& img : *wsi_) {
      clrs[0].image = img;
      targets_.push_back(pass_->target(size, 1, &clrs, nullptr, &ds));
    }
  }
};

Renderer View::Impl::renderer_{};

View::View(WS_NS::Window& window) : impl_(make_unique<Impl>(window)) { }

View::~View() { }

void View::loop(Scene& scene, uint32_t fps, const UpdateFn& update) {
  if (fps == 0)
    throw invalid_argument("View loop() `fps` must be greater than zero");

  impl_->loop(&scene, fps, update);
}

void View::swapScene(Scene& newScene) {
  impl_->swapScene(&newScene);
}

void View::render(Scene& scene) {
  impl_->render(&scene);
}
