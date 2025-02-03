export module voo:h2l_yuv_image;
import :device_and_queue;
import :host_buffer;
import vee;

namespace voo {
export class h2l_yuv_image {
  host_buffer m_buf_y;
  host_buffer m_buf_u;
  host_buffer m_buf_v;

  vee::sampler_ycbcr_conversion m_smp_conv;

  vee::image m_img;
  vee::device_memory m_mem;
  vee::image_view m_iv;

  unsigned m_w{};
  unsigned m_h{};

public:
  h2l_yuv_image() = default;
  explicit h2l_yuv_image(vee::physical_device pd, unsigned w, unsigned h)
      : m_buf_y{pd, w * h}
      , m_buf_u{pd, w * h / 4}
      , m_buf_v{pd, w * h / 4}
      , m_smp_conv{vee::create_sampler_yuv420p_conversion(pd)}
      , m_w{w}
      , m_h{h} {
    m_img = vee::create_image({m_w, m_h}, VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM);
    m_mem = vee::create_local_image_memory(pd, *m_img);
    vee::bind_image_memory(*m_img, *m_mem);
    m_iv = vee::create_yuv420p_image_view(*m_img, *m_smp_conv);
  }
  explicit h2l_yuv_image(const voo::device_and_queue &dq, unsigned w,
                         unsigned h)
      : h2l_yuv_image{dq.physical_device(), w, h} {}

  void setup_copy(vee::command_buffer cb) const {
    vee::cmd_pipeline_barrier(cb, *m_img, vee::from_host_to_transfer);
    vee::cmd_copy_yuv420p_buffers_to_image(cb, {m_w, m_h}, m_buf_y.buffer(),
                                           m_buf_u.buffer(), m_buf_v.buffer(),
                                           *m_img);
    vee::cmd_pipeline_barrier(cb, *m_img, vee::from_transfer_to_fragment);
  }

  [[nodiscard]] auto host_memory_y() const { return m_buf_y.memory(); }
  [[nodiscard]] auto host_memory_u() const { return m_buf_u.memory(); }
  [[nodiscard]] auto host_memory_v() const { return m_buf_v.memory(); }

  [[nodiscard]] auto iv() const { return *m_iv; }
  [[nodiscard]] auto conv() const { return *m_smp_conv; }
  [[nodiscard]] constexpr auto width() const { return m_w; }
  [[nodiscard]] constexpr auto height() const { return m_h; }
};
} // namespace voo
