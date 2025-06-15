export module voo:bound_buffer;
import vee;

namespace voo {
  export struct bound_image {
    vee::image img;
    vee::device_memory mem;
    vee::image_view iv;
  };
}
