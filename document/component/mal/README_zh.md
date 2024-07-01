# 《 VSF mal 块设备》

## 1. 简介
&emsp;&emsp;MAL 是 memory abstraction layer 的缩写，也就是一般所说的块设备。

## 2. 配置

```c
// 使能块设备
#define VSF_USE_MAL                     ENABLED
// 使能 fakefat32_mal 模块
#   define VSF_MAL_USE_FAKEFAT32_MAL    ENABLED
// 使能 file_mal 模块
#   define VSF_MAL_USE_FILE_MAL         ENABLED
// 使能 mem_mal 模块
#   define VSF_MAL_USE_MEM_MAL          ENABLED
// 使能 mim_mal 模块
#   define VSF_MAL_USE_MIM_MAL          ENABLED
// 使能 scsi_mal 模块
#   define VSF_MAL_USE_SCSI_MAL         ENABLED
// 使能 mmc_mal 模块，需要 hal 层使能 VSF_HAL_USE_MMC
#   define VSF_MAL_USE_MMC_MAL          ENABLED
// 使能 flash_mal 模块，需要 hal 层使能 VSF_HAL_USE_FLASH
#   define VSF_MAL_USE_FLASH_MAL        ENABLED
```

## 3. API 接口以及调用方式
- vsf_err_t vk_mal_init(vk_mal_t *pthis);
vk_mal_init 用于初始化对应的 mal 实例。
返回值表示初始化子调用的启动状态， VSF_ERR_NONE 表示正常启动，其他小于 0 的返回值，表示启动错误码。
可以通过 vsf_eda_get_return_value 得到实际执行初始化的结果，类型为 vsf_err_t 。

注意，即使 vk_mal_init 函数返回 VSF_ERR_NONE 也并不代表已经完成了初始化操作，这里需要分 2 种情况：
  - 独立堆栈任务中调用
  初始化子调用正常启动，并且执行完成。可通过 vsf_eda_get_return_value 获得执行结果。
```c
static uint8_t __membuffer[512];
static vk_mem_mal_t __mem_mal = {
    .drv            = &vk_mem_mal_drv,
    .mem            = {
        .buffer     = __membuffer,
        .size       = sizeof(__membuffer),
    },
    .blksz          = sizeof(__membuffer),
};
implement_vsf_thread(user_thread_t)
{
    if (VSF_ERR_NONE != vk_mal_init(&__mem_mal)) {
        vsf_trace_error("fail to start init subcall\n");
    }
    if (VSF_ERR_NONE != vsf_eda_get_return_value()) {
        vsf_trace_error("fail to initialize mal\n");
    }
}
```
  - 非独立堆栈任务中调用
  初始化子调用正常启动，但是还未开始执行，需要等调用任务退出当前的事件处理之后，才会开始执行。执行完成后会给调用任务发送 VSF_EVT_RETURN 事件；在 VSF_EVT_RETURN 事件里，可以通过 vsf_eda_get_return_value 获得执行结果。
```c
static uint8_t __membuffer[512];
static vk_mem_mal_t __mem_mal = {
    .drv            = &vk_mem_mal_drv,
    .mem            = {
        .buffer     = __membuffer,
        .size       = sizeof(__membuffer),
    },
    .blksz          = sizeof(__membuffer),
};
static void __user_eda_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_err_t err;
    switch (evt) {
    case VSF_EVT_INIT:
        err = vk_mal_init(&__mem_mal.use_as__vk_mal_t);
        vsf_trace_error("vk_mal_init returned\n");
        if (VSF_ERR_NONE != err) {
            vsf_trace_error("fail to start init subcall\n");
        }
        break;
    case VSF_EVT_RETURN:
        vsf_trace_error("subcall returned\n");
        err = vsf_eda_get_return_value();
        if (VSF_ERR_NONE != err) {
            vsf_trace_error("fail to initialize mal\n");
        }
        break;
    }
}
```

- vsf_err_t vk_mal_fini(vk_mal_t *pthis);
vk_mal_fini 用于终止化对应的 mal 实例，调用方式等同于 vk_mal_init 。

- uint_fast32_t vk_mal_blksz(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
vk_mal_blksz 用于获得块大小。块大小和当前操作的地址 addr 、需要操作的 size 字节大小、以及操作类型 op 有关。
有一些存储芯片，擦除时候的块大小和写入时候的块大小是不同的；也有一些存储芯片，不同区域的块大小也是不同的。
对于不支持的操作，返回的块大小是 0 。

- bool vk_mal_prepare_buffer(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
有些块设备具备本地缓冲， vk_mal_prepare_buffer 用于根据操作的类型、地址等参数，获得本地缓冲。
如果设备支持本地缓冲，那么返回 true ，并且 mem 里保存本地缓冲的地址和大小。如果不支持本地缓冲，返回 false 。

- vsf_err_t vk_mal_erase(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size);
vk_mal_erase 用于擦除 addr 开始 size 字节大小的储存器区间，调用方式等同于 vk_mal_init 。
实际执行结果的返回值通过 vsf_eda_get_return_value 来获得，类型为 vsf_err_t 。
注意，块设备在写入的时候会自动擦除，所以不需要在写入前特地调用 vk_mal_erase 来擦除对应储存区间。

- vsf_err_t vk_mal_read(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff);
vk_mal_read 用于读取 addr 开始 size 字节大小的储存器区间，调用方式等同于 vk_mal_init 。
实际执行结果的返回值通过 vsf_eda_get_return_value 来获得，类型为 int32_t 。
如果实际执行结果小于 0 ，表示执行失败。如果执行结果返回值大于等于 0 ，表示执行成功，返回实际读取的字节数。

- vsf_err_t vk_mal_write(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff);
vk_mal_write 用于写入 addr 开始 size 字节大小的储存器区间，调用方式等同于 vk_mal_init 。
实际执行结果的返回值通过 vsf_eda_get_return_value 来获得，类型为 int32_t 。
如果实际执行结果小于 0 ，表示执行失败。如果执行结果返回值大于等于 0 ，表示执行成功，返回实际写入的字节数。

- vsf_err_t vk_mal_read_stream(vk_mal_stream_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream);
需要使能 VSF_USE_SIMPLE_STREAM ，调用方式等同于 vk_mal_init 。
实际执行结果的返回值通过 vsf_eda_get_return_value 来获得，类型为 int32_t 。
如果实际执行结果小于 0 ，表示执行失败。如果执行结果返回值大于等于 0 ，表示执行成功，返回实际读取的字节数。

- vsf_err_t vk_mal_write_stream(vk_mal_stream_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream);
需要使能 VSF_USE_SIMPLE_STREAM ，调用方式等同于 vk_mal_init 。
实际执行结果的返回值通过 vsf_eda_get_return_value 来获得，类型为 int32_t 。
如果实际执行结果小于 0 ，表示执行失败。如果执行结果返回值大于等于 0 ，表示执行成功，返回实际写入的字节数。

## 4. mal 块设备驱动接口
```c
vsf_class(vk_mal_drv_t) {
    protected_member(
        uint_fast32_t (*blksz)(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
        bool (*buffer)(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
        vsf_peda_evthandler_t init;
        vsf_peda_evthandler_t fini;
        vsf_peda_evthandler_t erase;
        vsf_peda_evthandler_t read;
        vsf_peda_evthandler_t write;
    )
};
```

## 5. mal 块设备驱动
### 5.0 vk_mal_t
```c
vsf_class(vk_mal_t) {
    public_member(
        const vk_mal_drv_t *drv;
        // 参数指针，根据不同的子类，设置对应的参数结构。
        void *param;
        // 块设备的大小，单位字节。对于一些动态获得大小的驱动，可以设置为 0 。
        uint64_t size;
        // 块设备的特性， VSF_MAL_READABLE/VSF_MAL_WRITABLE/VSF_MAL_ERASABLE/VSF_MAL_NON_UNIFIED_BLOCK
        //  分别表示：可读/可写/可擦出/非单一块（不同的地址，块大小不同）
        uint8_t feature;
    )
};
```
一般类的 public 成员都需要初始化。除非一些块设备驱动里，会自动识别参数，那就只需要初始化 drv 驱动指针。

### 5.1 reentrant_mal
继承自 vk_mal_t ，由于依赖内核互斥量，所以需要使能 VSF_KERNEL_CFG_SUPPORT_SYNC 。 vk_mal_t 本身不具备可重入特性，当多个任务会操作同一个块设备实例的时候，就会有冲突。可以使用可重入块设备来实现可重入的块设备。具体方法如下：
0. 假设多个任务会访问 mal0
1. 每个会访问 mal0 的任务里，各自封装出自己可以访问的 vk_reentrant_mal_t 类型的 maln 。其中， maln 的 mal 成员指向 mal0 、 mutex 成员指向同一个内核互斥量、 offset 成员为 0 、 size 成员为 mal0 大小。
2. 每个任务通过自己封装出的 maln 来访问 mal0 ，这个时候，可重入块设备会通过同一个 mutex 来执行保护。

为了便于理解，这里举例一个使用场景，文件系统 mount 一个 MBR 的块设备。 MBR 可以支持多个分区，每个分区其实也会封装成一个块设备，提供给 malfs 文件系统驱动使用。在多任务环境下，多个分区的访问会有重入的问题。这个时候，就可以为每个分区提供一个可重入块设备来解决可重入问题。每个可重入块设备的 mal 成员指向 MBR 块设备、 mutex 成员指向同一个互斥量、 offset/size 为 MBR 块设备中，对应的分区的偏移和字节大小。

```c
vsf_class(vk_reentrant_mal_t) {
    public_member(
        implement(vk_mal_t)

        vk_mal_t *mal;
        vsf_mutex_t *mutex;
        uint64_t offset;
    )
};
```
需要初始化的成员：
1. mal -- 指向实际的块设备
2. mutex -- 指向同一个块设备的可重入块设备， mutex 也都指向同一个互斥量。如果为 NULL 的话，择不具备可重入能力。
3. offset/size（ vk_mal_t 的成员） -- 指定实际块设备中的一个存储区间

### 5.2 mem_mal
继承自 vk_mal_t ，可以把一个 IO 空间，模拟成一个块设备。

```c
vsf_class(vk_mem_mal_t) {
    public_member(
        implement(vk_mal_t)
        vsf_mem_t mem;
        uint32_t blksz;
    )
};
```
需要初始化的成员：
1. mem -- IO 空间，包括 buffer 成员和 size 成员
2. blksz -- 模拟的块设备的块大小

### 5.3 fakefat32_mal
继承自 vk_mal_t ，用于模拟一个 fat32 的块设备。内容由 root 成员决定。

```c
vsf_class(vk_fakefat32_mal_t) {
    public_member(
        implement(vk_mal_t)

        uint32_t sector_number;
        uint16_t sector_size;
        uint8_t sectors_per_cluster;

        uint32_t volume_id;
        uint32_t disk_id;

        vk_fakefat32_file_t root;
        vsf_err_t err;
    )
};
```

### 5.4 file_mal
继承自 vk_mal_t ，用于把一个文件模拟成块设备。一般用于文件系统的二进制镜像文件。

```c
vsf_class(vk_file_mal_t) {
    public_member(
        implement(vk_mal_t)
        vk_file_t *file;
        uint32_t block_size;
    )
};
```
需要初始化的成员：
1. file -- 镜像文件
2. block_size -- 模拟的块设备的块大小

### 5.5 flash_mal
继承自 vk_mal_t ，用于把芯片内部 flash 的一部分（或者全部），模拟成一个块设备。
需要使能 HAL 层中的 VSF_HAL_USE_FLASH 。

```c
vsf_class(vk_flash_mal_t) {
    public_member(
        implement(vk_mal_t)
        vsf_hw_flash_t *flash;
    )

    private_member(
        vsf_flash_capability_t cap;
    )
};
```
需要初始化的成员：
1. file -- 镜像文件
2. block_size -- 模拟的块设备的块大小

### 5.6 mmc_mal
继承自 vk_mal_t ，用于支持 SDIO/MMC 接口的储存设备。
需要使能 HAL 层中的 VSF_HAL_USE_MMC 。

```c
vsf_class(vk_mmc_mal_t) {
    public_member(
        implement(vk_mal_t)
        vsf_mmc_t              *mmc;
        vsf_arch_prio_t         hw_priority;
        implement(vsf_mmc_probe_t)
    )
    private_member(
        vsf_callback_timer_t    timer;
        vsf_eda_t              *eda;
        bool                    is_probing;
    )
};
```
需要初始化的成员：
1. mmc -- mmc 接口实例
2. hw_priority -- 硬件中断优先级
3. working_clock_hz（ vsf_mmc_probe_t 成员） -- sdio/mmc 工作频率
4. voltage（ vsf_mmc_probe_t 成员） -- sdio/mmc 工作电压
5. bus_width（ vsf_mmc_probe_t 成员） -- sdio/mmc 的总线位宽

### 5.7 scsi_mal
继承自 vk_mal_t ，用于把 SCSI 设备转成块设备。比如 USB 接口的 U 盘，是一个 scsi 设备，通过 scsi_mal 可以模拟成一个块设备。

```c
vsf_class(vk_scsi_mal_t) {
    public_member(
        implement(vk_mal_t)
        vk_scsi_t *scsi;
    )
    private_member(
        uint32_t block_size;
        uint8_t cbd[16];
        vsf_mem_t mem;
        union {
            scsi_inquiry_t inquiry;
            struct {
                uint32_t block_number;
                uint32_t block_size;
            } capacity;
        } buffer;
    )
};
```
需要初始化的成员：
1. scsi -- scsi 设备实例
