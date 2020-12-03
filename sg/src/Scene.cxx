//
// SG
// Scene.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Scene.h"

using namespace SG_NS;
using namespace std;

// TODO

class Scene::Impl {
};

Scene::Scene() : impl_(make_unique<Impl>()) { }

Scene::~Scene() { }
