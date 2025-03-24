export module voo:device_and_queue;
import :queue;
import mtx;
import no;
import vee;

export namespace voo {
class device_and_queue : no::no {
  vee::instance m_i;
  vee::debug_utils_messenger m_dbg;
  vee::surface m_s {};
  vee::physical_device m_pd;
  vee::device m_d;
  vee::render_pass m_rp;
  queue m_q;
  unsigned m_qf;

public:
  device_and_queue(const char *app_name, bool debug = true) {
    m_i = vee::create_instance(app_name);
    if (debug) m_dbg = vee::create_debug_utils_messenger();
    auto [pd, qf] = vee::find_physical_device_with_universal_queue(nullptr);
    m_pd = pd;
    m_qf = qf;

    m_d = vee::create_single_queue_device(pd, qf);
    m_q = voo::queue{qf};
  }
  device_and_queue(const char *app_name, auto native_ptr, bool debug = true) {
    m_i = vee::create_instance(app_name);
    if (debug) m_dbg = vee::create_debug_utils_messenger();
    m_s = vee::create_surface(native_ptr);
    auto [pd, qf] = vee::find_physical_device_with_universal_queue(*m_s);
    m_pd = pd;
    m_qf = qf;

    m_d = vee::create_single_queue_device(pd, qf);
    m_rp = vee::create_render_pass(pd, *m_s);
    m_q = voo::queue{qf};
  }
  ~device_and_queue() { vee::device_wait_idle(); }

  [[nodiscard]] constexpr const auto physical_device() const { return m_pd; }
  [[nodiscard]] constexpr const auto queue_family() const { return m_qf; }
  [[nodiscard]] constexpr const auto render_pass() const { return *m_rp; }
  [[nodiscard]] constexpr const auto surface() const { return *m_s; }
  [[nodiscard]] constexpr auto *queue() { return &m_q; }

  [[nodiscard]] auto find_best_surface_image_format() const {
    return vee::find_best_surface_image_format(physical_device(), surface());
  }
};

auto extent_of(vee::physical_device pd, vee::surface::type s) {
  return vee::get_surface_capabilities(pd, s).currentExtent;
}
auto extent_of(device_and_queue & dq) {
  return extent_of(dq.physical_device(), dq.surface());
}
} // namespace voo
