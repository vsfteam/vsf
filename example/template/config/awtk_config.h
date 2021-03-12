/**
 * 嵌入式系统有自己的 main 函数时，请定义本宏。
 *
 */
#define USE_GUI_MAIN 1

/**
 * 如果需要支持预先解码的位图字体，请定义本宏。一般只在 RAM 极小时，才启用本宏。
 * #define WITH_BITMAP_FONT 1
 */

/**
 * 如果支持 png/jpeg 图片，请定义本宏
 *
 * #define WITH_STB_IMAGE 1
 */

/**
 * 如果用 stb 支持 Truetype 字体，请定义本宏
 *
 */
#define WITH_STB_FONT 1

/**
 * 如果用 freetype 支持 Truetype 字体，请定义本宏。
 *
 * #define WITH_FT_FONT 1
 */

/**
 * 如果支持从文件系统加载资源，请定义本宏
 *
 * #define WITH_FS_RES 1
 */

/**
 * 如果定义本宏，使用标准的 UNICODE 换行算法，除非资源极为有限，请定义本宏。
 *
 * #define WITH_UNICODE_BREAK 1
 */

/**
 * 如果定义本宏，将图片解码成 BGRA8888 格式，否则解码成 RGBA8888 的格式。
 * 当硬件的 2D 加速需要 BGRA 格式时，请启用本宏。
 *
 * #define WITH_BITMAP_BGRA 1
 */

/**
 * 如果定义本宏，将不透明的 PNG 图片解码成 BGR565 格式，建议定义。
 * 另外和 LCD 的格式保存一致，可以大幅度提高性能。
 *
 * #define WITH_BITMAP_BGR565 1
 */

/**
 * 如果不需输入法，请定义本宏
 *
 * #define WITH_NULL_IM 1
 */

/**
 * 如果有标准的 malloc/free/calloc 等函数，请定义本宏
 *
 */
#define HAS_STD_MALLOC 1

/**
 * 如果有标准的 fopen/fclose 等函数，请定义本宏
 *
 * #define HAS_STDIO 1
 */

/**
 * 如果有标准的 pthread 等函数，请定义本宏
 *
 * #define HAS_PTHREAD 1
 */

/**
 * 如果有优化版本的 memcpy 函数，请定义本宏
 *
 * #define HAS_FAST_MEMCPY 1
 */

/**
 * 如果出现 wcsxxx 之类的函数没有定义时，请定义该宏
 *
 * #define WITH_WCSXXX 1
 */

/**
 * 如果启用 STM32 G2D 硬件加速，请定义本宏
 *
 * #define WITH_STM32_G2D 1
 */

/**
 * 如果启用 NXP PXP 硬件加速，请定义本宏
 *
 * #define WITH_PXP_G2D 1
 */

/**
 * 在没有 GPU 时，如果启用 agge 作为 nanovg 的后端（较 agg 作为后端：小，快，图形质量稍差），请定义本宏。
 *
 * #define WITH_NANOVG_AGGE 1
 */

/**
 * 在没有 GPU 时，如果启用 agg 作为 nanovg 的后端（较 agge 作为后端：大，慢，图形质量好），请定义本宏。
 * 注意：agg 是以 GPL 协议开源。
 *
 * #define WITH_NANOVG_AGG 1
 */

/**
 * 如果启用鼠标指针，请定义本宏
 *
 * #define ENABLE_CURSOR 1
 */

/**
 * 对于低端平台，如果不使用控件动画，请定义本宏。
 *
 */
#define WITHOUT_WIDGET_ANIMATORS 1

/**
 * 对于低端平台，如果不使用窗口动画，请定义本宏。
 *
 */
#define WITHOUT_WINDOW_ANIMATORS 1

/**
 * 对于低端平台，如果不使用对话框高亮策略，请定义本宏。
 *
 */
#define WITHOUT_DIALOG_HIGHLIGHTER 1

/**
 * 对于低端平台，如果不使用扩展控件，请定义本宏。
 *
 */
#define WITHOUT_EXT_WIDGETS 1

/**
 * 对于低端平台，如果内存不足以提供完整的 FrameBuffer，请定义本宏启用局部 FrameBuffer，可大幅度提高渲染性能。
 *
 */
#define FRAGMENT_FRAME_BUFFER_SIZE (800 * 600)
