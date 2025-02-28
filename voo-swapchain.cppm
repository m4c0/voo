export module voo:swapchain;
import :device_and_queue;
import hai;
import traits;
import vee;

using traits::is_callable;

export namespace voo {
  class swapchain {
    vee::swapchain m_swc;
    hai::array<vee::image_view> m_civs;
    vee::extent m_ext;
    unsigned m_idx;

  public:
    explicit swapchain(const device_and_queue & dq)
      : swapchain { dq.physical_device(), dq.surface() }
    {}

    swapchain(vee::physical_device pd, vee::surface::type s)
      : m_swc { vee::create_swapchain(pd, s) }
      , m_ext { extent_of(pd, s) }
    {
      auto swc_imgs = vee::get_swapchain_images(*m_swc);
      m_civs = hai::array<vee::image_view> { swc_imgs.size() };

      for (auto i = 0; i < swc_imgs.size(); i++) {
        m_civs[i] = vee::create_image_view_for_surface(swc_imgs[i], pd, s);
      }
    }

    [[nodiscard]] constexpr auto count() const { return m_civs.size(); }
    [[nodiscard]] constexpr auto extent() const { return m_ext; }
    [[nodiscard]] constexpr auto image_view(unsigned i) const { return *m_civs[i]; }
    [[nodiscard]] constexpr auto index() const { return m_idx; }

    [[nodiscard]] constexpr auto aspect() const {
      return static_cast<float>(extent().width) / static_cast<float>(extent().height);
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
