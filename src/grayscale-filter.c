#include <obs-module.h>
#include <graphics/graphics.h>
#include <graphics/matrix3.h>

struct grayscale_filter_data {
    obs_source_t *context;
    gs_effect_t *effect;
    gs_eparam_t *param_image;
};

static const char *grayscale_filter_name(void *unused)
{
    UNUSED_PARAMETER(unused);
    return obs_module_text("GrayscaleFilter");
}

static void grayscale_filter_destroy(void *data)
{
    struct grayscale_filter_data *filter = data;
    obs_enter_graphics();
    gs_effect_destroy(filter->effect);
    obs_leave_graphics();
    bfree(filter);
}

static void *grayscale_filter_create(obs_data_t *settings, obs_source_t *source)
{
    struct grayscale_filter_data *filter = bzalloc(sizeof(*filter));
    filter->context = source;

    char *effect_path = obs_module_file("grayscale.effect");
    if (effect_path) {
        obs_enter_graphics();
        filter->effect = gs_effect_create_from_file(effect_path, NULL);
        filter->param_image = gs_effect_get_param_by_name(filter->effect, "image");
        obs_leave_graphics();
        bfree(effect_path);
    }

    if (!filter->effect) {
        grayscale_filter_destroy(filter);
        return NULL;
    }

    return filter;
}

static void grayscale_filter_render(void *data, gs_effect_t *effect)
{
    UNUSED_PARAMETER(effect);

    struct grayscale_filter_data *filter = data;
    if (!obs_source_process_filter_begin(filter->context, GS_RGBA,
                                         OBS_ALLOW_DIRECT_RENDERING))
        return;

    /*  image parameter  -->  use the texture OBS already bound  */
    gs_effect_set_texture(filter->param_image, NULL);

    obs_source_process_filter_end(filter->context, filter->effect, 0, 0);
}

static struct obs_source_info grayscale_filter_info = {
    .id = "obs_grayscale_filter",
    .type = OBS_SOURCE_TYPE_FILTER,
    .output_flags = OBS_SOURCE_VIDEO,
    .get_name = grayscale_filter_name,
    .create = grayscale_filter_create,
    .destroy = grayscale_filter_destroy,
    .video_render = grayscale_filter_render,
};

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-grayscale-filter", "en-US")

// bool obs_module_load(void)
// {
//     obs_register_source(&grayscale_filter_info);
//     return true;
// }