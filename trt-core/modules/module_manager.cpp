#include "modules/module_manager.h"

namespace trt {
namespace modules {

void ModuleManager::register_module(std::shared_ptr<IModule> module) {
    modules_.push_back(std::move(module));
}

const std::vector<std::shared_ptr<IModule>>& ModuleManager::all_modules() const {
    return modules_;
}

}  // namespace modules
}  // namespace trt
