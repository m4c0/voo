export module voo:h2l_image;
import :device_and_queue;
import :dirty_flag;
import :guards;
import :host_buffer;
import vee;

namespace voo {
export class h2l_image {
  dirty_flag m_dirty{};
  unsigned m_w;
  unsigned m_h;

  host_buffer m_buf;

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
  explicit h2l_image(vee::physical_device pd, int w, int h)
      : m_buf{pd, w * h * 4} {
    m_w = w;
    m_h = h;
    init(pd);
  }
  explicit h2l_image(const voo::device_and_queue &dq, int w, int h)
      : h2l_image{dq.physical_device(), w, h} {}

  [[nodiscard]] auto mapmem() { return m_dirty.guard(m_buf.memory()); }

  [[nodiscard]] auto iv() const noexcept { return *m_iv; }

  void run(const cmd_buf_one_time_submit &cb) {
    if (!m_dirty.get_and_clear())
      return;

    vee::cmd_pipeline_barrier(*cb, *m_img, vee::from_host_to_transfer);
    vee::cmd_copy_buffer_to_image(*cb, {m_w, m_h}, m_buf.buffer(), *m_img);
    vee::cmd_pipeline_barrier(*cb, *m_img, vee::from_transfer_to_fragment);
  }
};
} // namespace voo
