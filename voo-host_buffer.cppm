export module voo:host_buffer;
import vee;

namespace voo {
export class host_buffer {
  vee::buffer m_buf;
  vee::device_memory m_mem;

public:
  host_buffer() = default;
  explicit host_buffer(vee::physical_device pd, int sz) {
    m_buf = vee::create_transfer_src_buffer(sz);
    m_mem = vee::create_host_buffer_memory(pd, *m_buf);
    vee::bind_buffer_memory(*m_buf, *m_mem);
  }

  [[nodiscard]] auto buffer() const noexcept { return *m_buf; }
  [[nodiscard]] auto mapmem() { return vee::mapmem{*m_mem}; }
};
} // namespace voo
