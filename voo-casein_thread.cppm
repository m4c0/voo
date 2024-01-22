export module voo:casein_thread;
import :device_and_queue;
import :frame_count;
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
  mtx::cond m_cond{};
  bool m_init{};

protected:
  [[nodiscard]] auto &resized() noexcept { return m_resized; }
  [[nodiscard]] constexpr auto native_ptr() const noexcept { return m_nptr; }

  void extent_loop(auto fn) {
    frame_count fc{};

    resized() = false;
    while (!interrupted() && !resized()) {
      ++fc;
      fn();
    }

    fc.print();
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

  void wait_init() {
    mtx::lock l{&m_mutex};
    while (!interrupted() && !m_init) {
      m_cond.wait(&l);
    }
  }
  void release_init_lock() {
    mtx::lock l{&m_mutex};
    m_init = true;
    m_cond.wake_all();
  }

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
