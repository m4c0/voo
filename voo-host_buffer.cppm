export module voo:host_buffer;
import :device_and_queue;
import traits;
import vee;

namespace voo {
export class host_buffer {
  vee::buffer m_buf;
  vee::device_memory m_mem;

public:
  host_buffer() = default;
  host_buffer(vee::physical_device pd, vee::buffer b)
    : m_buf { traits::move(b) }
    , m_mem { vee::create_host_buffer_memory(pd, *m_buf) } {
    vee::bind_buffer_memory(*m_buf, *m_mem);
  }
  host_buffer(vee::physical_device pd, unsigned sz)
    : host_buffer { pd, vee::create_transfer_src_buffer(sz) } {}
  host_buffer(const device_and_queue &dq, unsigned sz)
      : host_buffer{dq.physical_device(), sz} {}

  [[nodiscard]] auto buffer() const { return *m_buf; }
  [[nodiscard]] auto memory() const { return *m_mem; }
};
} // namespace voo
