export module voo:casein_thread;
import :device_and_queue;
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
  mtx::cond m_init_cond{};
  bool m_init{};

protected:
  [[nodiscard]] auto &resized() { return m_resized; }

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
      m_init_cond.wait(&l);
    }
  }
  void release_init_lock() {
    mtx::lock l{&m_mutex};
    m_init = true;
    m_init_cond.wake_all();
  }

  void main_loop(const char * app_name, auto fn) {
    voo::device_and_queue dq { app_name };
    while (!interrupted()) {
      voo::swapchain_and_stuff sw { dq };
      fn(dq, sw);
    }
  }
  void ots_loop(voo::device_and_queue & dq, voo::swapchain_and_stuff & sw, auto && fn) {
    auto q = dq.queue();
    extent_loop(q, sw, [&] {
      sw.queue_one_time_submit(q, [&](auto pcb) {
        auto scb = sw.cmd_render_pass(pcb);
        fn(*scb);
      });
    });
  }

  using thread::interrupted;

public:
  casein_thread();
  virtual ~casein_thread() = default;
};
} // namespace voo
