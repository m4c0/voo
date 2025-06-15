export module voo:buffers;
import vee;

namespace voo {
  struct bound_buffer {
    vee::buffer buffer;
    vee::device_memory memory;

    [[nodiscard]] static auto create(
        unsigned mem_type_idx, unsigned sz, 
        vee::buffer_usage usage,
        auto ... usages) {
      bound_buffer res {
        .buffer = vee::create_buffer(sz, usage, usages...),
        .memory = vee::create_memory({
          .allocationSize = sz,
          .memoryTypeIndex = mem_type_idx,
        }),
      };
      vee::bind_buffer_memory(*res.buffer, *res.memory);
      return res;
    }

    [[nodiscard]] static auto create_from_host(
        vee::physical_device pd, unsigned sz, 
        vee::buffer_usage usage = vee::buffer_usage::transfer_src_buffer, 
        auto ... usages) {
      return create(vee::find_host_memory_type_index(pd), sz, usage, usages...);
    }
    [[nodiscard]] static auto create_from_device_local(
        vee::physical_device pd, unsigned sz, 
        vee::buffer_usage usage = vee::buffer_usage::transfer_dst_buffer, 
        auto ... usages) {
      return create(vee::find_device_local_memory_type_index(pd), sz, usage, usages...);
    }
  };
}
