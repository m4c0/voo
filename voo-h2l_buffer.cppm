export module voo:h2l_buffer;
import :device_and_queue;
import :guards;
import :host_buffer;
import traits;
import vee;

namespace voo {
class dirt_guard {
  vee::mapmem m_mem;
  bool *m_dirty;

public:
  dirt_guard(vee::device_memory::type m, bool *flag)
      : m_mem{vee::mapmem{m}}, m_dirty{flag} {}
  ~dirt_guard() { *m_dirty = true; }

  [[nodiscard]] auto operator*() { return *m_mem; }
};
export class h2l_buffer {
  host_buffer m_hbuf;

  vee::buffer m_buf;
  vee::device_memory m_mem;
  vee::command_buffer m_cb;
  vee::fence m_fence;

  bool m_dirty{true};
  int m_size{};

public:
  h2l_buffer() = default;
  explicit h2l_buffer(vee::physical_device pd, vee::command_pool::type cp,
                      int sz)
      : m_hbuf{pd, sz}, m_cb{vee::allocate_primary_command_buffer(cp)},
        m_fence{vee::create_fence_signaled()}, m_size{sz} {
    m_buf =
        vee::create_buffer(sz, vee::vertex_buffer, vee::transfer_dst_buffer);
    m_mem = vee::create_local_buffer_memory(pd, *m_buf);
    vee::bind_buffer_memory(*m_buf, *m_mem);
  }
  explicit h2l_buffer(const voo::device_and_queue &dq, int sz)
      : h2l_buffer{dq.physical_device(), dq.command_pool(), sz} {}

  [[nodiscard]] auto mapmem() {
    vee::wait_and_reset_fence(*m_fence);
    return dirt_guard{m_hbuf.memory(), &m_dirty};
  }

  [[nodiscard]] auto buffer() const noexcept { return *m_buf; }

  void cmd_bind_vertex_buffer(const voo::cmd_render_pass &scb,
                              unsigned binding) {
    vee::cmd_bind_vertex_buffers(*scb, binding, *m_buf);
  }

  void submit(const vee::queue &q) {
    if (!m_dirty)
      return;

    {
      voo::cmd_buf_one_time_submit pcb{m_cb};
      vee::cmd_pipeline_barrier(*pcb, *m_buf, vee::from_host_to_transfer);
      vee::cmd_copy_buffer(*pcb, m_hbuf.buffer(), *m_buf, m_size);
      vee::cmd_pipeline_barrier(*pcb, *m_buf, vee::from_transfer_to_vertex);
    }
    vee::queue_submit({
        .queue = q,
        .fence = *m_fence,
        .command_buffer = m_cb,
    });
    m_dirty = false;
  }
  void submit(const voo::device_and_queue &dq) { submit(dq.queue()); }
};
} // namespace voo
