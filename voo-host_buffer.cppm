export module voo:host_buffer;
import :device_and_queue;
import :dirty_flag;
import :fence;
import missingno;
import mtx;
import sith;
import vee;

namespace voo {
export class host_buffer {
  vee::buffer m_buf;
  vee::device_memory m_mem;

public:
  host_buffer() = default;
  host_buffer(vee::physical_device pd, unsigned sz) {
    m_buf = vee::create_transfer_src_buffer(sz);
    m_mem = vee::create_host_buffer_memory(pd, *m_buf);
    vee::bind_buffer_memory(*m_buf, *m_mem);
  }
  host_buffer(const device_and_queue &dq, unsigned sz)
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
  mtx::mutex m_mtx{};
  mtx::cond m_cond{};
  volatile bool m_fence_status{};

public:
  fenced_host_buffer() = default;
  fenced_host_buffer(vee::physical_device pd, vee::command_pool::type cp,
                     unsigned sz)
      : m_hbuf{pd, sz}, m_fence{fence::signaled{}},
        m_cb{vee::allocate_primary_command_buffer(cp)} {}

  [[nodiscard]] constexpr auto buffer() const noexcept {
    return m_hbuf.buffer();
  }
  [[nodiscard]] constexpr auto cmd_buf() const noexcept { return m_cb; }

  // Meant to run on non-vulkan threads
  [[nodiscard]] auto mapmem(sith::thread *t) {
    mtx::lock l{&m_mtx};
    while (!m_fence_status && !t->interrupted()) {
      m_cond.wait(&l);
    }
    // TODO: test if we can fix racing condition if we flip the "if/else"
    // See poc for details
    if (t->interrupted())
      return mno::req<dirt_guard>::failed("Thread interrupted");
    return mno::req{m_dirty.guard(m_hbuf.memory())};
  }

  // Meant to run on vulkan threads
  void submit(const vee::queue &q) {
    m_fence_status = m_fence.get();
    m_cond.wake_one();

    if (!m_dirty.get_and_clear())
      return;

    m_fence.reset();
    vee::queue_submit({
        .queue = q,
        .fence = *m_fence,
        .command_buffer = m_cb,
    });
  }
  void submit(const voo::device_and_queue &dq) { submit(dq.queue()); }
};
} // namespace voo
