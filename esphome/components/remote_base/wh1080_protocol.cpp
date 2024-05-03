#include "wh1080_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *const TAG = "remote.wh1080";


constexpr uint32_t BIT_LOW_US = 1000;
constexpr uint32_t BIT_ONE_HIGH_US = 500;
constexpr uint32_t BIT_ZERO_HIGH_US = 1500;

constexpr unsigned int WH1080_IR_PACKET_BIT_SIZE = 86;  // 6x "1" preamble bits + 10 bytes data

// void Wh1080Protocol::encode_byte_(RemoteTransmitData *dst, uint8_t item) {

// }

// void Wh1080Protocol::setup() {
  
// }

// void Wh1080Protocol::loop() {
  
// }

// void Wh1080Protocol::update() {
//     // if (this->sensor1_ != nullptr)
//     //   this->sensor1_->publish_state(1.0f);
//     // if (this->sensor2_ != nullptr)
//     //   this->sensor2_->publish_state(2.0f);
//     // if (this->sensor3_ != nullptr)
//     //   this->sensor3_->publish_state(3.0f);
// }

// void Wh1080Protocol::dump_config() {
//     ESP_LOGCONFIG(TAG, "Wh1080Protocol sensor");
// }

void Wh1080Protocol::encode(RemoteTransmitData *dst, const Wh1080Data &data) {

}

bool Wh1080Protocol::expect_sync(RemoteReceiveData &src) {

  if (!src.is_valid(11))
    return false;

  // Follow Non Inverted logic
  if (!src.peek_mark(BIT_ONE_HIGH_US))
    return false;
  if (!src.peek_space(BIT_LOW_US, 1))
    return false;
  if (!src.peek_mark(BIT_ONE_HIGH_US, 2))
    return false;
  if (!src.peek_space(BIT_LOW_US, 3))
    return false;
  if (!src.peek_mark(BIT_ONE_HIGH_US, 4))
    return false;
  if (!src.peek_space(BIT_LOW_US, 5))
    return false;
  if (!src.peek_mark(BIT_ONE_HIGH_US, 6))
    return false;
  if (!src.peek_space(BIT_LOW_US, 7))
    return false;
  if (!src.peek_mark(BIT_ONE_HIGH_US, 8))
    return false;
  if (!src.peek_space(BIT_LOW_US, 9))
    return false;
  if (!src.peek_mark(BIT_ONE_HIGH_US, 10))
    return false;
  if (!src.peek_space(BIT_LOW_US, 11))
    return false;
  // Align with data bits following preamble:
  src.advance(13);

  return true;
}

optional<Wh1080Data> Wh1080Protocol::decode(RemoteReceiveData src) {

  size_t size = src.size() - src.get_index() - 1;
  // Size seems to be 86*2 bits, sometimes 87
  if (size < WH1080_IR_PACKET_BIT_SIZE)
    return {};
  if (size == 87*2) src.advance(2);

  bool preambleFound = false;
  for (uint8_t searchCounter = 1; searchCounter <= 4; searchCounter++) {
    // Search forward 4 bits in case one or two zeros were received in the preamble
    if (!this->expect_sync(src)) {
      src.advance(2);
    } else {
      preambleFound = true;
      break;
    }
  }
  if (!preambleFound) return {};

  ESP_LOGI(TAG, "Received Wh1080 preamble size %s",format_hex_pretty(size).c_str());
    
  Wh1080Data out;

  // Should be 80 bits of data
  size = 80 * 2;
  uint8_t checksum = 0;
  while (size > 0) {
    uint8_t data = 0;
    for (uint8_t mask = 0x80; mask != 0; mask >>= 1) {
      if (!src.expect_space(BIT_LOW_US)) {
        return {};
      }
      if (src.expect_mark(BIT_ONE_HIGH_US)) {
        data |= mask;
      } else if (!src.expect_mark(BIT_ZERO_HIGH_US)) {
        return {};
      }
      size -= 2;
    }
    if (size >= 0) {
      checksum += data;
      out.data.push_back(data);
    }
  }
  if (out.data.size() == 10)
  {
    ESP_LOGI(TAG, "crc %s",format_hex_pretty(out.data[9]).c_str());

    u_int8_t deviceId = (out.data[0] << 4) | (out.data[1] >> 4);
    float temp = (float)((((int32)(out.data[1] & 0x0F) << 8) | (int32)out.data[2]) - 400) / 10;
    u_int8_t humidity = out.data[3];
    float windAvg = (float)out.data[4] * 0.34;
    float windGust = (float)out.data[5] * 0.34;
    float rain = (float)((int32)out.data[6] << 8 | (int32)out.data[7]) * 0.2794;
    u_int8_t batteryFlag = out.data[8] >> 4;
    float windDirection = (float)(out.data[8] & 0x0F) * 22.5;

    // if (deviceId_sensor_ != nullptr) {
    //   deviceId_sensor_->publish_state(deviceId);
    // }
    // if (temp_sensor_ != nullptr) {
    //   temp_sensor_->publish_state(temp);
    // }
    // if (humidity_sensor_ != nullptr) {
    //   humidity_sensor_->publish_state(humidity);
    // }
    // if (windAvg_sensor_ != nullptr) {
    //   windAvg_sensor_->publish_state(windAvg);
    // }
    // if (windGust_sensor_ != nullptr) {
    //   windGust_sensor_->publish_state(windGust);
    // }
    // if (rain_sensor_ != nullptr) {
    //   rain_sensor_->publish_state(rain);
    // }
    // if (batteryFlag_sensor_ != nullptr) {
    //   batteryFlag_sensor_->publish_state(batteryFlag);
    // }
    // if (windDirection_sensor_ != nullptr) {
    //   windDirection_sensor_->publish_state(windDirection);
    // }

    ESP_LOGI(TAG, "Received Wh1080 %i %fC %i %fm/s %fm/s %fmm %i %fdeg",deviceId, temp, humidity, windAvg, windGust, rain, batteryFlag, windDirection);
  }
  //ESP_LOGI(TAG, "Received Wh1080 packet");
  return out;
}

void Wh1080Protocol::dump(const Wh1080Data &data) {
  ESP_LOGI(TAG, "Received Wh1080: %s", format_hex_pretty(data.data).c_str());
}

}  // namespace remote_base
}  // namespace esphome
