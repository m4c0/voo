export module voo:device_and_queue;
import :queue;
import mtx;
import no;
import vee;
import wagen;

using namespace wagen;

export namespace voo {
[[nodiscard]] auto extent_of(vee::physical_device pd, vee::surface::type s) {
  return vee::get_surface_capabilities(pd, s).currentExtent;
}

class device_and_queue : no::no {
  vee::instance m_i;
  vee::debug_utils_messenger m_dbg;
  vee::surface m_s {};
  vee::physical_device m_pd;
  vee::device m_d;
  unsigned m_qf;

public:
  struct params {
    bool debug = true;
    VkPhysicalDeviceFeatures feats {};
    void * next = nullptr;
  };

  device_and_queue(const char *app_name, bool debug = true) {
    m_i = vee::create_instance(app_name);
    if (debug) m_dbg = vee::create_debug_utils_messenger();
    auto [pd, qf] = vee::find_physical_device_with_universal_queue(nullptr);
    m_pd = pd;
    m_qf = qf;

    m_d = vee::create_single_queue_device(pd, qf);
    voo::queue::universal(qf);
  }
  device_and_queue(const char *app_name, auto native_ptr, const params & p) {
    m_i = vee::create_instance(app_name);
    if (p.debug) m_dbg = vee::create_debug_utils_messenger();
    m_s = vee::create_surface(native_ptr);
    auto [pd, qf] = vee::find_physical_device_with_universal_queue(*m_s);
    m_pd = pd;
    m_qf = qf;

    m_d = vee::create_single_queue_device(pd, qf, p.feats, p.next);
    voo::queue::universal(qf);
  }
  device_and_queue(const char *app_name, auto native_ptr) : device_and_queue { app_name, native_ptr, {} } {}

  ~device_and_queue() { vee::device_wait_idle(); }

  [[nodiscard]] constexpr const auto physical_device() const { return m_pd; }
  [[nodiscard]] constexpr const auto queue_family() const { return m_qf; }
  [[nodiscard]] constexpr const auto surface() const { return *m_s; }
  [[nodiscard]] constexpr voo::queue * queue() { return queue::universal(); }

  [[nodiscard]] auto find_best_surface_image_format() const {
    return vee::find_best_surface_image_format(physical_device(), surface());
  }
  [[nodiscard]] auto extent_of() const {
    return voo::extent_of(physical_device(), surface());
  }
  [[nodiscard]] auto aspect_of() const {
    auto [w, h] = extent_of();
    return static_cast<float>(w) / h;
  }
};
} // namespace voo
