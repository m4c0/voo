export module voo:h2l_buffer;
import :device_and_queue;
import :guards;
import :host_buffer;
import vee;

namespace voo {
export class h2l_buffer {
  host_buffer m_hbuf;

  vee::buffer m_buf;
  vee::device_memory m_mem;

  bool m_dirty{true};
  int m_size{};

public:
  h2l_buffer() = default;
  explicit h2l_buffer(vee::physical_device pd, int sz)
      : m_hbuf{pd, sz}, m_size{sz} {
    m_buf =
        vee::create_buffer(sz, vee::vertex_buffer, vee::transfer_dst_buffer);
    m_mem = vee::create_local_buffer_memory(pd, *m_buf);
    vee::bind_buffer_memory(*m_buf, *m_mem);
  }
  explicit h2l_buffer(const voo::device_and_queue &dq, int sz)
      : h2l_buffer{dq.physical_device(), sz} {}

  [[nodiscard]] auto mapmem() {
    m_dirty = true;
    return m_hbuf.mapmem();
  }

  [[nodiscard]] auto buffer() const noexcept { return *m_buf; }

  void run(const cmd_buf_one_time_submit &cb) {
    if (!m_dirty)
      return;

    vee::cmd_pipeline_barrier(*cb, *m_buf, vee::from_host_to_transfer);
    vee::cmd_copy_buffer(*cb, m_hbuf.buffer(), *m_buf, m_size);
    vee::cmd_pipeline_barrier(*cb, *m_buf, vee::from_transfer_to_vertex);
    m_dirty = false;
  }
};
} // namespace voo
