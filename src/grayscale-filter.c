// /*  Grayscale Filter for OBS Studio  –  step-by-step comments for beginners
//  *  ------------------------------------------------------------------------
//  *  This file shows the minimum code needed to add a **video-effect filter**
//  *  (turns the source black-and-white) using OBS’s C API and a small HLSL
//  *  shader.  Compile → install → restart OBS → the filter appears in
//  *  Filters → Effect Filters → “Grayscale Filter”.
//  */

// #include <obs-module.h>          /*  OBS module macros (OBS_DECLARE_MODULE …)  */
// #include <graphics/graphics.h>   /*  GPU helpers (gs_effect, gs_texture …)     */
// #include <graphics/matrix3.h>    /*  only needed here for the shader matrix    */
// // #include <util/platform.h>


// /*---------- 1.  Per-instance data  ------------------------------------------
//  *  OBS allocates one of these for **each** source that receives the filter.
//  *  We keep the compiled effect (shader) and the parameter handle inside.
//  *--------------------------------------------------------------------------*/
// struct grayscale_filter_data {
//     obs_source_t *context;   /*  back-pointer to the source we are attached to  */
//     gs_effect_t  *effect;    /*  compiled GPU program (grayscale.effect)        */
//     gs_eparam_t  *param_image; /*  shader variable “image” we must fill each frame */
// };

// /*---------- 2.  Display name (shown in the Filters list)  -------------------*/
// static const char *grayscale_filter_name(void *unused)
// {
//     UNUSED_PARAMETER(unused);
//     /*  obs_module_text() loads the string from data/locale/en-US.ini
//      *  key “GrayscaleFilter”.  Change the .ini file to rename the filter.     */
//     return obs_module_text("GrayscaleFilter");
// }

// /*---------- 3.  Destructor – free GPU objects  ------------------------------*/
// static void grayscale_filter_destroy(void *data)
// {
//     struct grayscale_filter_data *filter = data;

//     /*  All gs_* calls must be inside a graphics context lock.                */
//     obs_enter_graphics();
//     gs_effect_destroy(filter->effect);
//     obs_leave_graphics();

//     bfree(filter);
// }

// /*---------- 4.  Constructor – load the shader  ------------------------------*/
// static void *grayscale_filter_create(obs_data_t *settings, obs_source_t *source)
// {
//     UNUSED_PARAMETER(settings);   /*  no user settings yet                    */

//     struct grayscale_filter_data *filter = bzalloc(sizeof(*filter));
//     filter->context = source;

//     /*  Ask OBS for the full path to data/grayscale.effect (ships with plug-in) */
//     char *effect_path = obs_module_file("grayscale.effect");
//     if (effect_path) {
//         // Enter graphics context to create GPU resources (there is an internal mutex lock on this critical section)
//         obs_enter_graphics();
//         /*  Compile the HLSL file into a GPU program (NULL = no error string)   */
//         filter->effect = gs_effect_create_from_file(effect_path, NULL);
//         /*  Grab the “image” parameter so we can bind the source texture to it   */
//         filter->param_image = gs_effect_get_param_by_name(filter->effect, "image");
//         obs_leave_graphics();
//         bfree(effect_path);
//     }

//     /*  If the effect file is missing or invalid we abort creation.           */
//     if (!filter->effect) {
//         grayscale_filter_destroy(filter);
//         return NULL;
//     }

//     return filter;
// }

// /*---------- 5.  Every frame – apply the shader  -----------------------------*/
// static void grayscale_filter_render(void *data, gs_effect_t *effect)
// {
//     UNUSED_PARAMETER(effect);   /*  we use our own effect, not the one passed */

//     // simulate heavy work
//     // blog(LOG_INFO, "[grayscale] sleeping 130 ms");
    
//     // os_sleep_ms(130);

//     struct grayscale_filter_data *filter = data;

//     /*  Begin filter rendering; gives us a texture to work on.                */
//     if (!obs_source_process_filter_begin(filter->context, GS_RGBA,
//                                          OBS_ALLOW_DIRECT_RENDERING))
//         return;

//     /*  Bind the source texture to the “image” variable inside the shader.
//      *  Passing NULL tells OBS to use the texture it already set as active.    */
//     gs_effect_set_texture(filter->param_image, NULL);

//     /*  Draw a full-screen quad with our shader, then hand the result back.   */
//     obs_source_process_filter_end(filter->context, filter->effect, 0, 0);
// }

// /*---------- 6.  Plugin descriptor – tells OBS what we can create  -----------*/
// static struct obs_source_info grayscale_filter_info = {
//     .id           = "obs_grayscale_filter",   /*  unique internal ID            */
//     .type         = OBS_SOURCE_TYPE_FILTER,   /*  appears in Effect Filters     */
//     .output_flags = OBS_SOURCE_VIDEO |        /*  we modify video frames        */
//                    OBS_SOURCE_CAP_OBSOLETE,   /*  puts us in “Effect Filters”   */
//     .get_name     = grayscale_filter_name,    /*  display name callback         */
//     .create       = grayscale_filter_create,  /*  constructor                   */
//     .destroy      = grayscale_filter_destroy, /*  destructor                    */
//     .video_render = grayscale_filter_render,  /*  per-frame callback            */
// };

// /*---------- 7.  Module entry point – register everything  -------------------*/
// // OBS_DECLARE_MODULE()                           /*  mandatory macro               */
// // OBS_MODULE_USE_DEFAULT_LOCALE("obs-grayscale-filter", "en-US")
// //                                                /*  load translations             */

// bool obs_module_load(void)
// {
//     /*  Register the filter so OBS knows it exists.  Without this call
//      *  the plug-in loads but the filter never appears in any menu.            */
//     obs_register_source(&grayscale_filter_info);
//     return true;
// }