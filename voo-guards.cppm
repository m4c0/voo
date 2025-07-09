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
    [](const vee::render_pass_begin & rpb) {
      vee::cmd_begin_render_pass(rpb);
      return rpb.command_buffer;
    },
    vee::cmd_end_render_pass>;

  export class present_guard {
    queue * m_q;
    swapchain * m_swc;
    frame_sync_stuff * m_sync;
  public:
    present_guard(queue * q, swapchain * sc, frame_sync_stuff * sync)
      : m_q { q }, m_swc { sc }, m_sync { sync }
    {
      sync->wait_and_reset_fence();
      sc->acquire_next_image(sync->img_available_sema());
    }
    ~present_guard() {
      m_swc->queue_present(m_q, m_sync->rnd_finished_sema());
    }
  };
} // namespace voo
