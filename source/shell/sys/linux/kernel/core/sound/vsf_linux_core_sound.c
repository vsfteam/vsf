/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED

#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

int snd_card_new(struct device *parent, int idx, const char *id,
                 struct module *module, int extra_size,
                 struct snd_card **card_ret)
{
    VSF_LINUX_ASSERT(card_ret != NULL);

    struct snd_card *card = kzalloc(sizeof(struct snd_card) + (extra_size < 0 ? 0 : extra_size), GFP_KERNEL);
    if (!card) {
        return -ENOMEM;
    }

    card->number = idx;
    card->module = module;
    card->dev = parent;
    card->private_data = (void *)&card[1];
    if (id != NULL) {
        strscpy(card->id, id, sizeof(card->id));
    }

    card->card_dev.parent = parent;

    *card_ret = card;
    return 0;
}

static void __snd_pcm_free(struct snd_pcm *pcm)
{
    __vsf_slist_foreach_next_unsafe(struct snd_pcm_substream, list_node, &pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream_list) {
        vsf_slist_remove(struct snd_pcm_substream, list_node, &pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream_list, _);
        kfree(_);
    }

    __vsf_slist_foreach_next_unsafe(struct snd_pcm_substream, list_node, &pcm->streams[SNDRV_PCM_STREAM_CAPTURE].substream_list) {
        vsf_slist_remove(struct snd_pcm_substream, list_node, &pcm->streams[SNDRV_PCM_STREAM_CAPTURE].substream_list, _);
        kfree(_);
    }

    kfree(pcm);
}

int snd_pcm_new_stream(struct snd_pcm *pcm, int stream, int substream_count)
{
    struct snd_pcm_str *pstr = &pcm->streams[stream];
    int start_idx = pstr->substream_count;

    pstr->stream = stream;
    pstr->pcm = pcm;
    pstr->substream_count += substream_count;
    if (!substream_count) {
        return 0;
    }

    struct snd_pcm_substream *substream;
    for (int idx = 0; idx < substream_count; idx++) {
        substream = kzalloc(sizeof(struct snd_pcm_substream), GFP_KERNEL);
        if (!substream) {
            return -ENOMEM;
        }
        substream->pcm = pcm;
        substream->pstr = pstr;
        substream->stream = stream;
        substream->number = start_idx + idx;

        vsf_slist_add_to_head(struct snd_pcm_substream, list_node, &pstr->substream_list, substream);
    }
    return 0;
}

int snd_pcm_new(struct snd_card *card, const char *id, int device, int playback_count, int capture_count, struct snd_pcm **rpcm)
{
    struct snd_pcm *pcm = kzalloc(sizeof(struct snd_pcm), GFP_KERNEL);
    if (!pcm) {
        return -ENOMEM;
    }

    pcm->card = card;
    pcm->device = device;
    if (id != NULL) {
        strscpy(pcm->id, id, sizeof(pcm->id));
    }

    int err = snd_pcm_new_stream(pcm, SNDRV_PCM_STREAM_PLAYBACK, playback_count);
    if (err < 0) {
        goto free_pcm;
    }
    err = snd_pcm_new_stream(pcm, SNDRV_PCM_STREAM_CAPTURE, capture_count);
    if (err < 0) {
        goto free_pcm;
    }

    if (rpcm != NULL) {
        *rpcm = pcm;
    }
    return 0;
free_pcm:
    __snd_pcm_free(pcm);
    return err;
}

void snd_pcm_set_ops(struct snd_pcm *pcm, int direction, const struct snd_pcm_ops *ops)
{
    struct snd_pcm_str *stream = &pcm->streams[direction];

    __vsf_slist_foreach_next_unsafe(struct snd_pcm_substream, list_node, &stream->substream_list) {
        _->ops = ops;
    }
}

int snd_card_register(struct snd_card *card)
{
    return 0;
}

void snd_card_disconnect(struct snd_card *card)
{
}

void snd_card_free(struct snd_card *card)
{
}

void snd_card_free_when_closed(struct snd_card *card)
{
    snd_card_disconnect(card);
    put_device(&card->card_dev);
}

void snd_pcm_period_elapsed_under_stream_lock(struct snd_pcm_substream *substream)
{
}

void snd_pcm_period_elapsed(struct snd_pcm_substream *substream)
{
    unsigned long flags;
    snd_pcm_stream_lock_irqsave(substream, flags);
        snd_pcm_period_elapsed_under_stream_lock(substream);
    snd_pcm_stream_unlock_irqrestore(substream, flags);
}

int snd_pcm_lib_alloc_vmalloc_buffer(struct snd_pcm_substream *substream, size_t size)
{
    struct snd_pcm_runtime *runtime;
    if ((NULL == substream) || (NULL == (runtime = substream->runtime))) {
        return -EINVAL;
    }

    if (runtime->dma_area != NULL) {
        if (runtime->dma_bytes >= size) {
            return 0;
        }
        kfree(runtime->dma_area);
    }
    runtime->dma_area = kmalloc(size, GFP_KERNEL);
    if (!runtime->dma_area) {
        return -ENOMEM;
    }
    runtime->dma_bytes = size;
    return 1;
}

int snd_pcm_lib_free_vmalloc_buffer(struct snd_pcm_substream *substream)
{
    struct snd_pcm_runtime *runtime;
    if ((NULL == substream) || (NULL == (runtime = substream->runtime))) {
        return -EINVAL;
    }

    kfree(runtime->dma_area);
    runtime->dma_area = NULL;
    return 0;
}

struct page * snd_pcm_lib_get_vmalloc_page(struct snd_pcm_substream *substream, unsigned long offset)
{
    return NULL;
}

int snd_pcm_lib_ioctl(struct snd_pcm_substream *substream, unsigned int cmd, void *arg)
{
	return -ENXIO;
}

#endif
