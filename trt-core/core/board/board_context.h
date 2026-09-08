#pragma once

#include <memory>
#include <unordered_map>

#include "core/board/board_info.h"
#include "core/capabilities/capability_manager.h"
#include "interfaces/adc.h"
#include "interfaces/dac.h"
#include "interfaces/gpio.h"
#include "interfaces/i2c.h"
#include "interfaces/pwm.h"
#include "interfaces/spi.h"
#include "modules/module_manager.h"
#include "transports/transport.h"

namespace trt {
namespace core {
namespace board {

class BoardContext {
   public:
    BoardContext(BoardInfo info,
                 capabilities::CapabilityManager capability_manager,
                 std::shared_ptr<transports::ITransport> transport,
                 modules::ModuleManager module_manager);

    const BoardInfo& info() const;
    capabilities::CapabilityManager& capabilities();
    const capabilities::CapabilityManager& capabilities() const;
    modules::ModuleManager& modules();
    const modules::ModuleManager& modules() const;
    std::shared_ptr<transports::ITransport> transport() const;

    void set_gpio(std::shared_ptr<interfaces::IGpio> gpio);
    void set_pwm(std::shared_ptr<interfaces::IPwm> pwm);
    void set_adc(std::shared_ptr<interfaces::IAdc> adc);
    void set_dac(std::shared_ptr<interfaces::IDac> dac);
    void set_i2c(std::shared_ptr<interfaces::II2c> i2c);
    void set_spi(std::shared_ptr<interfaces::ISpi> spi);

    std::shared_ptr<interfaces::IGpio> gpio() const;
    std::shared_ptr<interfaces::IPwm> pwm() const;
    std::shared_ptr<interfaces::IAdc> adc() const;
    std::shared_ptr<interfaces::IDac> dac() const;
    std::shared_ptr<interfaces::II2c> i2c() const;
    std::shared_ptr<interfaces::ISpi> spi() const;

   private:
    BoardInfo info_;
    capabilities::CapabilityManager capability_manager_;
    std::shared_ptr<transports::ITransport> transport_;
    modules::ModuleManager module_manager_;

    std::shared_ptr<interfaces::IGpio> gpio_;
    std::shared_ptr<interfaces::IPwm> pwm_;
    std::shared_ptr<interfaces::IAdc> adc_;
    std::shared_ptr<interfaces::IDac> dac_;
    std::shared_ptr<interfaces::II2c> i2c_;
    std::shared_ptr<interfaces::ISpi> spi_;
};

}  // namespace board
}  // namespace core
}  // namespace trt
