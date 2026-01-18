export module voo:buffers;
import vee;
import wagen;

using namespace wagen;

namespace voo {
  export struct bound_buffer {
    vee::buffer buffer;
    vee::device_memory memory;

    [[nodiscard]] static auto create(unsigned mem_type_idx, unsigned sz, VkBufferUsageFlags usage) {
      bound_buffer res {
        .buffer = vee::create_buffer(sz, usage),
        .memory = vee::create_memory({
          .allocationSize = sz,
          .memoryTypeIndex = mem_type_idx,
        }),
      };
      vee::bind_buffer_memory(*res.buffer, *res.memory);
      return res;
    }

    [[nodiscard]] static auto create_from_host(unsigned sz, VkBufferUsageFlags usage) {
      return create(vee::find_host_memory_type_index(), sz, usage);
    }
    [[nodiscard]] static auto create_from_device_local(unsigned sz, VkBufferUsageFlags usage) {
      return create(vee::find_device_local_memory_type_index(), sz, usage);
    }
  };
}
