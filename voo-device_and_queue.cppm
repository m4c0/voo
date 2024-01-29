export module voo:device_and_queue;
import casein;
import vee;

namespace voo {
export class device_and_queue {
  vee::instance m_i;
  vee::debug_utils_messenger m_dbg;
  vee::surface m_s;
  vee::physical_device m_pd;
  vee::device m_d;
  vee::queue m_q;
  vee::command_pool m_cp;

public:
  device_and_queue(const char *app_name, casein::native_handle_t nptr) {
    m_i = vee::create_instance(app_name);
    m_dbg = vee::create_debug_utils_messenger();
    m_s = vee::create_surface(nptr);
    auto [pd, qf] = vee::find_physical_device_with_universal_queue(*m_s);
    m_pd = pd;

    m_d = vee::create_single_queue_device(pd, qf);
    m_q = vee::get_queue_for_family(qf);
    m_cp = vee::create_command_pool(qf);
  }

  [[nodiscard]] constexpr const auto command_pool() const noexcept {
    return *m_cp;
  }
  [[nodiscard]] constexpr const auto physical_device() const noexcept {
    return m_pd;
  }
  [[nodiscard]] constexpr const auto queue() const noexcept { return m_q; }
  [[nodiscard]] constexpr const auto surface() const noexcept { return *m_s; }
};
} // namespace voo
