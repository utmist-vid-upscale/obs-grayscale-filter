#include <obs-module.h>
#include <graphics/graphics.h>
#include <util/platform.h>

#define PLUGIN_VERSION "1.0.0"

/* 1.  per-instance data  */
struct grayscale_filter_data {
    obs_source_t *context;
    gs_effect_t  *effect;
    gs_eparam_t  *param_image;
};

/* 2.  display name  */
static const char *grayscale_filter_name(void *unused)
{
    UNUSED_PARAMETER(unused);
    return obs_module_text("GrayscaleFilter");
}

/* 3.  destructor  */
static void grayscale_filter_destroy(void *data)
{
    struct grayscale_filter_data *f = data;
    obs_enter_graphics();
    gs_effect_destroy(f->effect);
    obs_leave_graphics();
    bfree(f);
}

/* 4.  constructor  */
static void *grayscale_filter_create(obs_data_t *settings, obs_source_t *source)
{
    UNUSED_PARAMETER(settings);
    struct grayscale_filter_data *f = bzalloc(sizeof(*f));
    f->context = source;

    char *effect_path = obs_module_file("grayscale.effect");
    if (effect_path) {
        obs_enter_graphics();
        f->effect = gs_effect_create_from_file(effect_path, NULL);
        f->param_image = gs_effect_get_param_by_name(f->effect, "image");
        obs_leave_graphics();
        bfree(effect_path);
    }
    if (!f->effect) {
        grayscale_filter_destroy(f);
        return NULL;
    }
    return f;
}

/* 5.  per-frame render  */
static void grayscale_filter_render(void *data, gs_effect_t *effect)
{
    UNUSED_PARAMETER(effect);
    // simulate heavy work
    blog(LOG_INFO, "[grayscale] sleeping 130 ms");
    
    os_sleep_ms(130);

    struct grayscale_filter_data *f = data;

    if (!obs_source_process_filter_begin(f->context, GS_RGBA,
                                         OBS_ALLOW_DIRECT_RENDERING))
        return;

    gs_effect_set_texture(f->param_image, NULL);
    obs_source_process_filter_end(f->context, f->effect, 0, 0);
}

/* 6.  source info  */
static struct obs_source_info grayscale_filter_info = {
    .id           = "obs_grayscale_filter",
    .type         = OBS_SOURCE_TYPE_FILTER,
    .output_flags = OBS_SOURCE_VIDEO,
    .get_name     = grayscale_filter_name,
    .create       = grayscale_filter_create,
    .destroy      = grayscale_filter_destroy,
    .video_render = grayscale_filter_render,
};

/* 7.  module entry  */
OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-grayscale-filter", "en-US")

bool obs_module_load(void)
{
    blog(LOG_INFO, "grayscale filter 2.0 plugin loaded (version %s)", PLUGIN_VERSION);
    obs_register_source(&grayscale_filter_info);
    blog(LOG_INFO, "registering source (version %s)", PLUGIN_VERSION);

    return true;
}