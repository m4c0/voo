export module voo:h2l_image;
import :device_and_queue;
import :guards;
import :host_buffer;
import sith;
import vee;

namespace voo {
export class h2l_image {
  host_buffer m_hbuf;

  vee::image m_img;
  vee::device_memory m_mem;
  vee::image_view m_iv;

  unsigned m_w{};
  unsigned m_h{};

public:
  h2l_image() = default;
  explicit h2l_image(vee::physical_device pd, unsigned w, unsigned h,
                     bool rgba = true)
      : m_hbuf{pd, w * h * 4} {
    m_w = w;
    m_h = h;
    m_img = vee::create_image({m_w, m_h}, rgba ? vee::image_format_srgba
                                               : vee::image_format_r8);
    m_mem = vee::create_local_image_memory(pd, *m_img);
    vee::bind_image_memory(*m_img, *m_mem);
    m_iv = rgba ? vee::create_srgba_image_view(*m_img)
                : vee::create_r8_image_view(*m_img);
  }
  explicit h2l_image(const voo::device_and_queue &dq, unsigned w, unsigned h,
                     bool rgba = true)
      : h2l_image{dq.physical_device(), w, h, rgba} {}

  void setup_copy(vee::command_buffer cb) {
    vee::cmd_pipeline_barrier(cb, *m_img, vee::from_host_to_transfer);
    vee::cmd_copy_buffer_to_image(cb, {m_w, m_h}, m_hbuf.buffer(), *m_img);
    vee::cmd_pipeline_barrier(cb, *m_img, vee::from_transfer_to_fragment);
  }

  [[nodiscard]] auto host_memory() const noexcept { return m_hbuf.memory(); }
  [[nodiscard]] auto iv() const noexcept { return *m_iv; }
  [[nodiscard]] constexpr auto width() const noexcept { return m_w; }
  [[nodiscard]] constexpr auto height() const noexcept { return m_h; }
};
} // namespace voo
