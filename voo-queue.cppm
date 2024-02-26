export module voo:queue;
import mtx;
import vee;

namespace voo {
/// Queue access facade protected by a mutex.
/// This performs "host synchronisation" for queue access, as required by
/// Vulkan. It was designed with single-queue devices in mind (ex: Apple).
class queue {
  mtx::mutex *m_qmtx;
  vee::queue m_q;

public:
  constexpr queue(mtx::mutex *m, vee::queue q) : m_qmtx{m}, m_q{q} {}

  void device_wait_idle() {
    mtx::lock l{m_qmtx};
    vee::device_wait_idle();
  }
  void queue_present(vee::present_info si) {
    mtx::lock l{m_qmtx};
    si.queue = m_q;
    vee::queue_present(si);
  }
  void queue_submit(vee::submit_info si) {
    mtx::lock l{m_qmtx};
    si.queue = m_q;
    vee::queue_submit(si);
  }
};
}
