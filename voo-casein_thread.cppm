export module voo:casein_thread;
import casein;
import sith;
import vee;

namespace voo {
export class casein_thread : sith::thread, public casein::handler {
  casein::native_handle_t m_nptr{};
  volatile bool m_resized{};

protected:
  [[nodiscard]] auto &resized() noexcept { return m_resized; }
  [[nodiscard]] constexpr auto native_ptr() const noexcept { return m_nptr; }

  using thread::interrupted;

public:
  casein_thread() = default;
  virtual ~casein_thread() = default;

  void create_window(const casein::events::create_window &e) override {
    m_nptr = *e;
    start();
  }

  void resize_window(const casein::events::resize_window &e) override {
    m_resized = true;
  }

  void quit(const casein::events::quit &e) override { stop(); }
};
} // namespace voo
