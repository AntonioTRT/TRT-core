#include "core/board/board_context.h"

namespace trt {
namespace core {
namespace board {

BoardContext::BoardContext(BoardInfo info,
                           capabilities::CapabilityManager capability_manager,
                           std::shared_ptr<transports::ITransport> transport,
                           modules::ModuleManager module_manager)
    : info_(std::move(info)),
      capability_manager_(std::move(capability_manager)),
      transport_(std::move(transport)),
      module_manager_(std::move(module_manager)) {}

const BoardInfo& BoardContext::info() const { return info_; }

capabilities::CapabilityManager& BoardContext::capabilities() { return capability_manager_; }

const capabilities::CapabilityManager& BoardContext::capabilities() const { return capability_manager_; }

modules::ModuleManager& BoardContext::modules() { return module_manager_; }

const modules::ModuleManager& BoardContext::modules() const { return module_manager_; }

std::shared_ptr<transports::ITransport> BoardContext::transport() const { return transport_; }

void BoardContext::set_gpio(std::shared_ptr<interfaces::IGpio> gpio) { gpio_ = std::move(gpio); }
void BoardContext::set_pwm(std::shared_ptr<interfaces::IPwm> pwm) { pwm_ = std::move(pwm); }
void BoardContext::set_adc(std::shared_ptr<interfaces::IAdc> adc) { adc_ = std::move(adc); }
void BoardContext::set_dac(std::shared_ptr<interfaces::IDac> dac) { dac_ = std::move(dac); }
void BoardContext::set_i2c(std::shared_ptr<interfaces::II2c> i2c) { i2c_ = std::move(i2c); }
void BoardContext::set_spi(std::shared_ptr<interfaces::ISpi> spi) { spi_ = std::move(spi); }

std::shared_ptr<interfaces::IGpio> BoardContext::gpio() const { return gpio_; }
std::shared_ptr<interfaces::IPwm> BoardContext::pwm() const { return pwm_; }
std::shared_ptr<interfaces::IAdc> BoardContext::adc() const { return adc_; }
std::shared_ptr<interfaces::IDac> BoardContext::dac() const { return dac_; }
std::shared_ptr<interfaces::II2c> BoardContext::i2c() const { return i2c_; }
std::shared_ptr<interfaces::ISpi> BoardContext::spi() const { return spi_; }

}  // namespace board
}  // namespace core
}  // namespace trt
