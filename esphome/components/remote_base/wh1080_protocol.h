#pragma once

#include "remote_base.h"
#include <vector>

namespace esphome {
namespace remote_base {

struct Wh1080Data {
  std::vector<uint8_t> data;

  bool operator==(const Wh1080Data &rhs) const { return data == rhs.data; }
};

class Wh1080Protocol : public RemoteProtocol<Wh1080Data> {
 public:
  void encode(RemoteTransmitData *dst, const Wh1080Data &data) override;
  optional<Wh1080Data> decode(RemoteReceiveData src) override;
  void dump(const Wh1080Data &data) override;

 protected:
  // void encode_byte_(RemoteTransmitData *dst, uint8_t item);
  bool expect_sync(RemoteReceiveData &src);
};

DECLARE_REMOTE_PROTOCOL(Wh1080)

template<typename... Ts> class Wh1080Action : public RemoteTransmitterActionBase<Ts...> {
 public:
  TEMPLATABLE_VALUE(std::vector<uint8_t>, code)

  void encode(RemoteTransmitData *dst, Ts... x) override {
    Wh1080Data data{};
    data.data = this->code_.value(x...);
    Wh1080Protocol().encode(dst, data);
  }
};

}  // namespace remote_base
}  // namespace esphome
