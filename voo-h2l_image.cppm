export module voo:h2l_image;
import :device_and_queue;
import :dirty_flag;
import :fence;
import :guards;
import :host_buffer;
import vee;

namespace voo {
export class h2l_image {
  dirty_flag m_dirty{};
  fence m_fence{};
  unsigned m_w;
  unsigned m_h;

  host_buffer m_buf;

  vee::command_buffer m_cb;
  vee::image m_img;
  vee::device_memory m_mem;
  vee::image_view m_iv;

  void init(vee::physical_device pd) {
    m_img = vee::create_srgba_image({m_w, m_h});
    m_mem = vee::create_local_image_memory(pd, *m_img);
    vee::bind_image_memory(*m_img, *m_mem);
    m_iv = vee::create_srgba_image_view(*m_img);
  }

public:
  h2l_image() = default;
  explicit h2l_image(vee::physical_device pd, vee::command_pool::type cp, int w,
                     int h)
      : m_fence{fence::signaled{}}, m_buf{pd, w * h * 4},
        m_cb{vee::allocate_primary_command_buffer(cp)} {
    m_w = w;
    m_h = h;
    init(pd);
  }
  explicit h2l_image(const voo::device_and_queue &dq, int w, int h)
      : h2l_image{dq.physical_device(), dq.command_pool(), w, h} {}

  [[nodiscard]] auto mapmem() {
    m_fence.wait_and_reset();
    return m_dirty.guard(m_buf.memory());
  }

  [[nodiscard]] auto iv() const noexcept { return *m_iv; }
  [[nodiscard]] constexpr auto width() const noexcept { return m_w; }
  [[nodiscard]] constexpr auto height() const noexcept { return m_h; }

  void submit(const vee::queue &q) {
    if (!m_dirty.get_and_clear())
      return;

    {
      voo::cmd_buf_one_time_submit pcb{m_cb};
      vee::cmd_pipeline_barrier(*pcb, *m_img, vee::from_host_to_transfer);
      vee::cmd_copy_buffer_to_image(*pcb, {m_w, m_h}, m_buf.buffer(), *m_img);
      vee::cmd_pipeline_barrier(*pcb, *m_img, vee::from_transfer_to_fragment);
    }
    vee::queue_submit({
        .queue = q,
        .fence = *m_fence,
        .command_buffer = m_cb,
    });
  }
  void submit(const voo::device_and_queue &dq) { submit(dq.queue()); }
};
} // namespace voo
