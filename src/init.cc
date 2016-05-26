#include "common.h"
#include "orbindexer.h"

extern "C" void init(Local<Object> target) {
  OrbIndexer::Init(target);
};

NODE_MODULE(orbidx, init)
