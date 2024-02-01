export module voo:command_pool;
import :device_and_queue;
import vee;

namespace voo {
export class command_pool {
  vee::command_pool m_cp;

public:
  explicit command_pool(const device_and_queue &dq)
      : command_pool{dq.queue_family()} {}
  explicit command_pool(unsigned qf) : m_cp{vee::create_command_pool(qf)} {}

  auto allocate_primary_command_buffer() {
    return vee::allocate_primary_command_buffer(*m_cp);
  }
  auto allocate_secondary_command_buffer() {
    return vee::allocate_secondary_command_buffer(*m_cp);
  }
};
} // namespace voo
