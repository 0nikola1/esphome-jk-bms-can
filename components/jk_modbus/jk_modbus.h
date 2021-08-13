#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace jk_modbus {

class JkModbusDevice;

class JkModbus : public uart::UARTDevice, public Component {
 public:
  JkModbus() = default;

  void setup() override;

  void loop() override;

  void dump_config() override;

  void register_device(JkModbusDevice *device) { this->devices_.push_back(device); }

  float get_setup_priority() const override;

  void send(uint8_t address, uint8_t function, uint16_t start_address, uint16_t register_count);

  void set_flow_control_pin(GPIOPin *flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }

 protected:
  GPIOPin *flow_control_pin_{nullptr};

  bool parse_jk_modbus_byte_(uint8_t byte);

  std::vector<uint8_t> rx_buffer_;
  uint32_t last_jk_modbus_byte_{0};
  std::vector<JkModbusDevice *> devices_;
};

uint16_t crc16(const uint8_t *data, uint8_t len);

class JkModbusDevice {
 public:
  void set_parent(JkModbus *parent) { parent_ = parent; }
  void set_address(uint8_t address) { address_ = address; }
  virtual void on_jk_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) = 0;

  void send(uint8_t function, uint16_t start_address, uint16_t register_count) {
    this->parent_->send(this->address_, function, start_address, register_count);
  }

 protected:
  friend JkModbus;

  JkModbus *parent_;
  uint8_t address_;
};

}  // namespace jk_modbus
}  // namespace esphome