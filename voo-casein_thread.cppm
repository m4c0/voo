export module voo:casein_thread;
import :device_and_queue;
import :swapchain_and_stuff;
import casein;
import hai;
import mtx;
import sith;
import vee;

namespace voo {
export class casein_thread : sith::thread, public casein::handler {
  casein::native_handle_t m_nptr{};
  volatile bool m_resized{};

  mtx::mutex m_mutex{};
  hai::uptr<mtx::lock> m_lock{new mtx::lock{&m_mutex}};

protected:
  [[nodiscard]] auto &resized() noexcept { return m_resized; }
  [[nodiscard]] constexpr auto native_ptr() const noexcept { return m_nptr; }

  void extent_loop(auto fn) {
    resized() = false;
    while (!interrupted() && !resized()) {
      fn();
    }
    vee::device_wait_idle();
  }
  void extent_loop(const device_and_queue &dq, swapchain_and_stuff &sw,
                   auto fn) {
    extent_loop([&] {
      sw.acquire_next_image();
      fn();
      sw.queue_present(dq);
    });
  }

  [[nodiscard]] auto wait_init() { return mtx::lock{&m_mutex}; }
  void release_init_lock() { m_lock = {}; }

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
