export module voo:h2l_buffer;
import :device_and_queue;
import :host_buffer;
import vee;

namespace voo {
export class h2l_buffer {
  host_buffer m_hbuf;
  unsigned m_sz;

  vee::buffer m_buf;
  vee::device_memory m_mem;

public:
  h2l_buffer() = default;
  explicit h2l_buffer(vee::physical_device pd, unsigned sz)
      : m_hbuf{pd, sz}, m_sz{sz} {
    m_buf = vee::create_buffer(sz, vee::buffer_usage::vertex_buffer, vee::buffer_usage::transfer_dst_buffer);
    m_mem = vee::create_local_buffer_memory(pd, *m_buf);
    vee::bind_buffer_memory(*m_buf, *m_mem);
  }
  explicit h2l_buffer(const voo::device_and_queue &dq, unsigned sz)
      : h2l_buffer{dq.physical_device(), sz} {}

  void setup_copy(vee::command_buffer cb) const {
    vee::cmd_pipeline_barrier(cb, *m_buf, vee::from_host_to_transfer);
    vee::cmd_copy_buffer(cb, m_hbuf.buffer(), *m_buf, m_sz);
    vee::cmd_pipeline_barrier(cb, *m_buf, vee::from_transfer_to_vertex);
  }

  [[nodiscard]] auto host_memory() const { return m_hbuf.memory(); }
  [[nodiscard]] auto local_buffer() const { return *m_buf; }
};
} // namespace voo
