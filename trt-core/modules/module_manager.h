#pragma once

#include <memory>
#include <vector>

#include "modules/module.h"

namespace trt {
namespace modules {

class ModuleManager {
   public:
    void register_module(std::shared_ptr<IModule> module);
    const std::vector<std::shared_ptr<IModule>>& all_modules() const;

   private:
    std::vector<std::shared_ptr<IModule>> modules_;
};

}  // namespace modules
}  // namespace trt
