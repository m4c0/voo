export module voo:host_buffer;
import :bound_buffer;
import :device_and_queue;
import traits;
import vee;

namespace voo {
  export struct host_buffer : public bound_buffer {
    host_buffer() = default;

    host_buffer(vee::physical_device pd, unsigned sz)
      : host_buffer { pd, sz, vee::buffer_usage::transfer_src_buffer }
    {}

    host_buffer(vee::physical_device pd, unsigned sz, vee::buffer_usage usage, auto... usages)
      : bound_buffer { vee::find_host_memory_type_index(pd), sz, usage, usages... }
    {}
  };
} // namespace voo
