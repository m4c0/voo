export module voo:single_dset;
import vee;

namespace voo {
  export class single_dset {
    vee::descriptor_set_layout m_dsl;
    vee::descriptor_pool m_pool;
    vee::descriptor_set m_dset;

  public:
    constexpr single_dset() = default;
    single_dset(auto dsl_bind, auto pool_size)
      : m_dsl { vee::create_descriptor_set_layout({ dsl_bind }) }
      , m_pool { vee::create_descriptor_pool(1, { pool_size }) }
      , m_dset { vee::allocate_descriptor_set(*m_pool, *m_dsl) } {}

    [[nodiscard]] constexpr auto descriptor_set() const { return m_dset; }
    [[nodiscard]] constexpr auto descriptor_set_layout() const { return *m_dsl; }
  };
}
