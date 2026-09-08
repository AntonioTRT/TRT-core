#include "core/services/system_service.h"

#include <algorithm>
#include <array>
#include <iomanip>
#include <sstream>

#include "core/board/board_context.h"
#include "core/capabilities/capabilities.h"
#include "core/protocol/identifiers.h"

namespace {

using trt::core::dispatcher::CommandResult;
using trt::core::errors::ErrorCode;

bool parse_firmware_version(const std::string& value, std::array<uint8_t, 3>& version) {
    std::size_t offset = 0;
    for (std::size_t part = 0; part < version.size(); ++part) {
        const std::size_t separator = value.find('.', offset);
        if ((part < version.size() - 1 && separator == std::string::npos) ||
            (part == version.size() - 1 && separator != std::string::npos)) {
            return false;
        }

        const std::size_t length = (separator == std::string::npos ? value.size() : separator) - offset;
        if (length == 0 || length > 3) {
            return false;
        }

        unsigned int component = 0;
        for (std::size_t index = offset; index < offset + length; ++index) {
            const char character = value[index];
            if (character < '0' || character > '9') {
                return false;
            }
            component = component * 10U + static_cast<unsigned int>(character - '0');
        }
        if (component > 255U) {
            return false;
        }

        version[part] = static_cast<uint8_t>(component);
        offset = separator == std::string::npos ? value.size() : separator + 1;
    }
    return true;
}

CommandResult get_version(const trt::core::protocol::Frame&, trt::core::board::BoardContext& context) {
    std::array<uint8_t, 3> version{};
    if (!parse_firmware_version(context.info().firmware_version, version)) {
        return {ErrorCode::kInternalError};
    }
    return {ErrorCode::kNone, static_cast<uint16_t>(trt::core::protocol::ResponseId::kData),
            {version[0], version[1], version[2]}};
}

CommandResult get_id(const trt::core::protocol::Frame&, trt::core::board::BoardContext& context) {
    const auto& revision = context.info().revision;
    return {ErrorCode::kNone, static_cast<uint16_t>(trt::core::protocol::ResponseId::kData),
            std::vector<uint8_t>(revision.begin(), revision.end())};
}

CommandResult get_capabilities(const trt::core::protocol::Frame&, trt::core::board::BoardContext& context) {
    std::vector<uint16_t> identifiers;
    for (const auto capability : context.capabilities().all()) {
        if (capability == trt::core::capabilities::Capability::kSystem) {
            identifiers.push_back(static_cast<uint16_t>(trt::core::protocol::CapabilityId::kSystem));
        }
    }
    std::sort(identifiers.begin(), identifiers.end());

    std::vector<uint8_t> payload{static_cast<uint8_t>(identifiers.size())};
    for (const auto identifier : identifiers) {
        payload.push_back(static_cast<uint8_t>((identifier >> 8U) & 0xFFU));
        payload.push_back(static_cast<uint8_t>(identifier & 0xFFU));
    }
    return {ErrorCode::kNone, static_cast<uint16_t>(trt::core::protocol::ResponseId::kData), std::move(payload)};
}

CommandResult get_build_id(const trt::core::protocol::Frame&, trt::core::board::BoardContext& context) {
    std::ostringstream stream;
    stream << std::setfill('0') << std::setw(6) << context.info().build_id;
    const auto value = stream.str();
    return {ErrorCode::kNone, static_cast<uint16_t>(trt::core::protocol::ResponseId::kData),
            std::vector<uint8_t>(value.begin(), value.end())};
}

}  // namespace

namespace trt {
namespace core {
namespace services {

void SystemService::register_commands(dispatcher::CommandRegistry& registry) const {
    registry.register_handler(static_cast<uint16_t>(protocol::CommandId::kFwVersion), get_version);
    registry.register_handler(static_cast<uint16_t>(protocol::CommandId::kBoardVersion), get_id);
    registry.register_handler(static_cast<uint16_t>(protocol::CommandId::kCapabilities), get_capabilities);
    registry.register_handler(static_cast<uint16_t>(protocol::CommandId::kBuildId), get_build_id);
}

}  // namespace services
}  // namespace core
}  // namespace trt