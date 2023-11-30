export module voo:h2l_image;
import :guards;
import vee;

namespace voo {
export class h2l_image {
  unsigned m_w;
  unsigned m_h;

  vee::buffer m_sbuf;
  vee::device_memory m_smem;

  vee::image m_img;
  vee::device_memory m_mem;
  vee::image_view m_iv;

  bool m_dirty{true};

  void init(vee::physical_device pd) {
    m_sbuf = vee::create_transfer_src_buffer(m_w * m_h * 4);
    m_smem = vee::create_host_buffer_memory(pd, *m_sbuf);
    vee::bind_buffer_memory(*m_sbuf, *m_smem);

    m_img = vee::create_srgba_image({m_w, m_h});
    m_mem = vee::create_local_image_memory(pd, *m_img);
    vee::bind_image_memory(*m_img, *m_mem);
    m_iv = vee::create_srgba_image_view(*m_img);
  }

public:
  h2l_image() = default;
  explicit h2l_image(vee::physical_device pd, int w, int h) {
    m_w = w;
    m_h = h;
    init(pd);
  }

  [[nodiscard]] auto mapmem() {
    m_dirty = true;
    return vee::mapmem{*m_smem};
  }

  [[nodiscard]] auto iv() const noexcept { return *m_iv; }

  void run(const cmd_buf_one_time_submit &cb) {
    if (!m_dirty)
      return;

    vee::cmd_pipeline_barrier(*cb, *m_img, vee::from_host_to_transfer);
    vee::cmd_copy_buffer_to_image(*cb, {m_w, m_h}, *m_sbuf, *m_img);
    vee::cmd_pipeline_barrier(*cb, *m_img, vee::from_transfer_to_fragment);
    m_dirty = false;
  }
};
} // namespace voo
