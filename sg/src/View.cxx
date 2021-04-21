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
#include "Renderer.h"

using namespace SG_NS;
using namespace std;

class View::Impl {
 public:
  Impl(WS_NS::Window* window) : wsi_(CG_NS::device().wsi(window)) {
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
    auto nextTgt = targets_.find(nextImg);
    if (nextTgt == targets_.end())
      throw runtime_error("Invalid render target");

    renderer_.render(*scene, *nextTgt->second);
    // TODO: catch broken swapchain errors
    wsi_->present(nextImg);
  }

 private:
  static Renderer renderer_;

  CG_NS::Wsi::Ptr wsi_{};
  CG_NS::Image::Ptr depthStencil_{};
  CG_NS::Pass::Ptr pass_{};
  unordered_map<CG_NS::Image*, CG_NS::Target::Ptr> targets_{};
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
    const auto& imgs = wsi_->images();
    const CG_NS::Size2 size{wsi_->window_->width(), wsi_->window_->height()};

    // Create depth/stencil image
    depthStencil_ = dev.image(CG_NS::PxFormatD16Unorm, size, 1, 1,
                              imgs[0]->samples_);

    // Create pass
    const vector<CG_NS::ColorAttach> clrAtts{{imgs[0]->format_,
                                              imgs[0]->samples_,
                                              CG_NS::LoadOpDontCare,
                                              CG_NS::StoreOpStore}};

    const CG_NS::DepStenAttach dsAtt{depthStencil_->format_,
                                     depthStencil_->samples_,
                                     CG_NS::LoadOpDontCare,
                                     CG_NS::StoreOpStore,
                                     CG_NS::LoadOpDontCare,
                                     CG_NS::StoreOpDontCare};

    pass_ = dev.pass(&clrAtts, nullptr, &dsAtt);

    // Create targets
    vector<CG_NS::AttachImg> clrs{{nullptr, 0, 0}};
    const CG_NS::AttachImg ds{depthStencil_.get(), 0, 0};

    for (const auto& img : imgs) {
      clrs[0].image = img;
      targets_.emplace(img, pass_->target(size, 1, &clrs, nullptr, &ds));
    }
  }
};

Renderer View::Impl::renderer_{};

View::View(WS_NS::Window* window) : impl_(make_unique<Impl>(window)) { }

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
