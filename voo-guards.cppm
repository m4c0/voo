export module voo:guards;
import :frame_sync_stuff;
import :queue;
import :swapchain;
import hay;
import traits;
import vee;

namespace voo {
  template<typename T>
  concept consumes_cmd_buf = traits::is_callable_r<T, void, vee::command_buffer>;

  template<auto F> using cb_guard = hay<
    vee::command_buffer,
    [](vee::command_buffer cb) {
      F(cb);
      return cb;
    },
    vee::end_cmd_buf>;

  export using cmd_buf_one_time_submit = cb_guard<vee::begin_cmd_buf_one_time_submit>;
  export using cmd_buf_sim_use = cb_guard<vee::begin_cmd_buf_sim_use>;

  export using cmd_buf_render_pass_continue = hay<
    vee::command_buffer,
    [](vee::command_buffer cb, vee::render_pass::type rp) {
      vee::begin_cmd_buf_render_pass_continue(cb, rp);
      return cb;
    },
    vee::end_cmd_buf>;

  export using cmd_render_pass = hay<
    vee::command_buffer,
    [](const vee::render_pass_begin & rpb, bool inlined) {
      vee::cmd_begin_render_pass(rpb, inlined);
      return rpb.command_buffer;
    },
    vee::cmd_end_render_pass>;

  export class present_guard {
    swapchain * m_swc;
  public:
    explicit present_guard(swapchain * sc) : m_swc { sc } {
      sc->acquire_next_image();
    }
    ~present_guard() {
      m_swc->queue_present();
    }
  };
  export class ots_present_guard {
    swapchain * m_swc;
    vee::command_buffer m_cb;
  public:
    ots_present_guard(swapchain * sc, vee::command_buffer cb) : m_swc { sc }, m_cb { cb } {
      m_swc->acquire_next_image();
      vee::begin_cmd_buf_one_time_submit(m_cb);
    }
    ~ots_present_guard() {
      vee::end_cmd_buf(m_cb);
      m_swc->queue_submit(m_cb);
      m_swc->queue_present();
    }
  };
} // namespace voo
