export module voo:host_buffer;
import :device_and_queue;
import :dirty_flag;
import :fence;
import vee;

namespace voo {
export class host_buffer {
  vee::buffer m_buf;
  vee::device_memory m_mem;

public:
  host_buffer() = default;
  host_buffer(vee::physical_device pd, int sz) {
    m_buf = vee::create_transfer_src_buffer(sz);
    m_mem = vee::create_host_buffer_memory(pd, *m_buf);
    vee::bind_buffer_memory(*m_buf, *m_mem);
  }
  host_buffer(const device_and_queue &dq, int sz)
      : host_buffer{dq.physical_device(), sz} {}

  [[nodiscard]] auto buffer() const noexcept { return *m_buf; }
  [[nodiscard]] auto memory() const noexcept { return *m_mem; }
  [[nodiscard]] auto mapmem() { return vee::mapmem{*m_mem}; }
};

class fenced_host_buffer {
  host_buffer m_hbuf;
  dirty_flag m_dirty{};
  fence m_fence{};
  vee::command_buffer m_cb;

public:
  fenced_host_buffer() = default;
  fenced_host_buffer(vee::physical_device pd, vee::command_pool::type cp,
                     int sz)
      : m_hbuf{pd, sz}, m_fence{fence::signaled{}},
        m_cb{vee::allocate_primary_command_buffer(cp)} {}

  [[nodiscard]] constexpr auto buffer() const noexcept {
    return m_hbuf.buffer();
  }
  [[nodiscard]] constexpr auto cmd_buf() const noexcept { return m_cb; }

  [[nodiscard]] auto mapmem(unsigned timeout_ms = ~0U) {
    m_fence.wait_and_reset(timeout_ms);
    return m_dirty.guard(m_hbuf.memory());
  }

  void submit(const vee::queue &q) {
    // TODO: consider "get" the fence status instead of a bool flag
    if (!m_dirty.get_and_clear())
      return;

    vee::queue_submit({
        .queue = q,
        .fence = *m_fence,
        .command_buffer = m_cb,
    });
  }
  void submit(const voo::device_and_queue &dq) { submit(dq.queue()); }
};
} // namespace voo
