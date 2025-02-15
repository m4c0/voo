export module voo:swapchain;
import :device_and_queue;
import hai;
import vee;

export namespace voo {
  class swapchain {
    vee::swapchain m_swc;
    hai::array<vee::image_view> m_civs;
    hai::array<vee::framebuffer> m_fbs;
    vee::extent m_ext;
    unsigned m_idx;

  public:
    swapchain(vee::physical_device pd, vee::surface::type s)
      : m_swc { vee::create_swapchain(pd, s) }
      , m_ext { extent_of(pd, s) }
    {
      auto swc_imgs = vee::get_swapchain_images(*m_swc);
      m_civs = hai::array<vee::image_view> { swc_imgs.size() };
      m_fbs = hai::array<vee::framebuffer> { swc_imgs.size() };

      for (auto i = 0; i < swc_imgs.size(); i++) {
        m_civs[i] = vee::create_image_view_for_surface(swc_imgs[i], pd, s);
      }
    }

    [[nodiscard]] constexpr auto count() const { return m_civs.size(); }
    [[nodiscard]] constexpr auto extent() const { return m_ext; }
    [[nodiscard]] constexpr auto framebuffer() const { return *m_fbs[m_idx]; }
    [[nodiscard]] constexpr auto image_view(unsigned i) const { return *m_civs[i]; }

    void framebuffer(unsigned idx, vee::framebuffer f) {
      m_fbs[idx] = traits::move(f);
    }
    void create_framebuffers(auto && fn) {
      for (auto i = 0; i < count(); i++) {
        m_fbs[i] = fn(*m_civs[i]);
      }
    }

    void acquire_next_image(vee::semaphore::type sema) {
      m_idx = vee::acquire_next_image(*m_swc, sema);
    }

    void queue_present(queue * q, vee::semaphore::type sema) {
      q->queue_present({
        .swapchain = *m_swc,
        .wait_semaphore = sema,
        .image_index = m_idx,
      });
    }
  };
}
