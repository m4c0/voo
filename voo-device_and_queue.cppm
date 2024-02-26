export module voo:device_and_queue;
import :queue;
import casein;
import mtx;
import no;
import vee;

namespace voo {
export class device_and_queue : no::no {
  vee::instance m_i;
  vee::debug_utils_messenger m_dbg;
  vee::surface m_s;
  vee::physical_device m_pd;
  vee::device m_d;
  vee::render_pass m_rp;
  vee::queue m_q;
  unsigned m_qf;
  mtx::mutex m_qmtx{};

public:
  device_and_queue(const char *app_name, casein::native_handle_t nptr) {
    m_i = vee::create_instance(app_name);
    m_dbg = vee::create_debug_utils_messenger();
    m_s = vee::create_surface(nptr);
    auto [pd, qf] = vee::find_physical_device_with_universal_queue(*m_s);
    m_pd = pd;
    m_qf = qf;

    m_d = vee::create_single_queue_device(pd, qf);
    m_rp = vee::create_render_pass(pd, *m_s);
    m_q = vee::get_queue_for_family(qf);
  }
  ~device_and_queue() { vee::device_wait_idle(); }

  [[nodiscard]] constexpr const auto physical_device() const noexcept {
    return m_pd;
  }
  [[nodiscard]] constexpr const auto queue_family() const noexcept {
    return m_qf;
  }
  [[nodiscard]] constexpr const auto render_pass() const noexcept {
    return *m_rp;
  }
  [[nodiscard]] constexpr const auto surface() const noexcept { return *m_s; }

  [[nodiscard]] constexpr auto queue() noexcept {
    return voo::queue{&m_qmtx, m_qf};
  }
};
} // namespace voo
