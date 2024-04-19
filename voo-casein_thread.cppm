export module voo:casein_thread;
import :frame_count;
import :queue;
import :swapchain_and_stuff;
import casein;
import mtx;
import sith;
import vee;

namespace voo {
export class casein_thread : public sith::thread {
  volatile bool m_resized{};
  sith::run_guard m_run{};

  mtx::mutex m_mutex{};
  mtx::cond m_cond{};
  bool m_init{};

protected:
  [[nodiscard]] auto &resized() noexcept { return m_resized; }

  void extent_loop(auto fn) {
    frame_count fc{};

    resized() = false;
    while (!interrupted() && !resized()) {
      ++fc;
      fn();
    }

    fc.print();
  }
  void extent_loop(queue *q, swapchain_and_stuff &sw, auto fn) {
    extent_loop([&] {
      sw.acquire_next_image();
      fn();
      sw.queue_present(q);
    });
    q->device_wait_idle();
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
  casein_thread();
  virtual ~casein_thread() = default;
};
} // namespace voo
