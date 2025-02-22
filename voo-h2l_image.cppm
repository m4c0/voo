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

  static auto size_of(vee::format fmt) {
    switch (fmt) {
      case VK_FORMAT_R8_UNORM: return 1;
      default: return 4;
    }
  }

  static auto create_buffer(unsigned sz, bool clearable) {
    if (clearable) {
      return vee::create_buffer(sz, vee::buffer_usage::transfer_src_buffer, vee::buffer_usage::transfer_dst_buffer);
    } else {
      return vee::create_buffer(sz, vee::buffer_usage::transfer_src_buffer);
    }
  }

public:
  struct params {
    vee::physical_device pd;
    unsigned w;
    unsigned h;
    vee::format fmt;
    bool clearable = false;
  };

  h2l_image() = default;
  explicit h2l_image(const params & p)
      : m_hbuf { p.pd, create_buffer(p.w * p.h * size_of(p.fmt), p.clearable) } {
    m_w = p.w;
    m_h = p.h;
    m_img = vee::create_image({m_w, m_h}, p.fmt);
    m_mem = vee::create_local_image_memory(p.pd, *m_img);
    vee::bind_image_memory(*m_img, *m_mem);
    m_iv = vee::create_image_view(*m_img, p.fmt);
  }

  explicit h2l_image(vee::physical_device pd, unsigned w, unsigned h, vee::format fmt)
    : h2l_image {{ pd, w, h, fmt }} {}

  void setup_copy(vee::command_buffer cb) const {
    vee::cmd_pipeline_barrier(cb, *m_img, vee::from_host_to_transfer);
    vee::cmd_copy_buffer_to_image(cb, {m_w, m_h}, m_hbuf.buffer(), *m_img);
    vee::cmd_pipeline_barrier(cb, *m_img, vee::from_transfer_to_fragment);
  }
  void clear_host(vee::command_buffer cb) const {
    vee::cmd_pipeline_barrier(cb, m_hbuf.buffer(), vee::from_pipeline_to_host);
    vee::cmd_fill_buffer(cb, m_hbuf.buffer(), 0);
  }

  void debug_name(const char * name) {
    vee::set_debug_utils_object_name(VK_OBJECT_TYPE_IMAGE, *m_img, name);
  }

  [[nodiscard]] auto host_memory() const { return m_hbuf.memory(); }
  [[nodiscard]] auto iv() const { return *m_iv; }
  [[nodiscard]] constexpr auto width() const { return m_w; }
  [[nodiscard]] constexpr auto height() const { return m_h; }
};
} // namespace voo
