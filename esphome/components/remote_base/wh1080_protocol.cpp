#include "wh1080_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *const TAG = "remote.wh1080";

// Mark = Low, Space = High?
// constexpr uint32_t HEADER_LOW_US = 1000;
// constexpr uint32_t HEADER_HIGH_US = 500;
constexpr uint32_t BIT_LOW_US = 1000;
constexpr uint32_t BIT_ONE_HIGH_US = 500;
constexpr uint32_t BIT_ZERO_HIGH_US = 1500;

// constexpr uint32_t HEADER_LOW_US = 1000;
// constexpr uint32_t HEADER_HIGH_US = 500;
// //constexpr uint32_t BIT_MARK_US = 540;
// constexpr uint32_t BIT_ONE_MARK_US = 500;
// constexpr uint32_t BIT_ZERO_MARK_US = 1500;
// constexpr uint32_t BIT_SPACE_US = 1000;
// constexpr uint32_t BIT_ONE_SPACE_US = 1000;
// constexpr uint32_t BIT_ZERO_SPACE_US = 1000;
//constexpr unsigned int WH1080_IR_PACKET_BIT_SIZE = 88;  // 1 byte preamble + 10 bytes data
constexpr unsigned int WH1080_IR_PACKET_BIT_SIZE = 86;  // 6x "1" preamble bits + 10 bytes data

void Wh1080Protocol::encode_byte_(RemoteTransmitData *dst, uint8_t item) {
  // for (uint8_t mask = 1 << 7; mask != 0; mask >>= 1) {
  //   if (item & mask) {
  //     dst->space(BIT_ONE_SPACE_US);
  //   } else {
  //     dst->space(BIT_ZERO_SPACE_US);
  //   }
  //   dst->mark(BIT_MARK_US);
  // }
}

void Wh1080Protocol::encode(RemoteTransmitData *dst, const Wh1080Data &data) {
  // dst->set_carrier_frequency(38000);
  // dst->reserve(5 + ((data.data.size() + 1) * 2));
  // dst->mark(HEADER_LOW_US);
  // dst->space(HEADER_LOW_US);
  // dst->mark(HEADER_LOW_US);
  // dst->space(HEADER_HIGH_US);
  // dst->mark(BIT_MARK_US);
  // uint8_t checksum = 0;
  // for (uint8_t item : data.data) {
  //   this->encode_byte_(dst, item);
  //   checksum += item;
  // }
  // this->encode_byte_(dst, checksum);
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
  }
  ESP_LOGI(TAG, "Received Wh1080 packet");
  return out;
}

void Wh1080Protocol::dump(const Wh1080Data &data) {
  ESP_LOGI(TAG, "Received Wh1080: %s", format_hex_pretty(data.data).c_str());
}

}  // namespace remote_base
}  // namespace esphome
