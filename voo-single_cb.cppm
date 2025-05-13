export module voo:single_cb;
import :queue;
import vee;

export namespace voo {
  class single_cb {
    vee::command_pool m_cp;
    vee::command_buffer m_cb;
  public:
    explicit single_cb()
      : single_cb { voo::queue::instance()->queue_family() }
    {}
    explicit single_cb(unsigned qf)
      : m_cp { vee::create_command_pool(qf) }
      , m_cb { vee::allocate_primary_command_buffer(*m_cp) } {}

    [[nodiscard]] constexpr auto cb() const { return m_cb; }
  };
}
