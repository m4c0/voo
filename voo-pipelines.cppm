export module voo:pipelines;
import :shader;
import jute;
import sv;
import vee;
import traits;

using namespace jute::literals;

namespace voo {
  export auto create_graphics_pipeline(const char * name, vee::gr_pipeline_params && gpp) {
    auto vert = voo::vert_shader((sv::unsafe(name) + ".vert.spv"_s).cstr());
    auto frag = voo::frag_shader((sv::unsafe(name) + ".frag.spv"_s).cstr());
    gpp.shaders = { *vert, *frag };
    auto res = vee::create_graphics_pipeline(traits::move(gpp));
    vee::set_debug_utils_object_name(*res, name);
    return res;
  }

  // TODO: find better place for this
  export auto allocate_descriptor_set(const char * name, VkDescriptorPool pool, VkDescriptorSetLayout dsl) {
    auto dset = vee::allocate_descriptor_set(pool, dsl);
    vee::set_debug_utils_object_name(VK_OBJECT_TYPE_DESCRIPTOR_SET, dset, name);
    return dset;
  }
}
