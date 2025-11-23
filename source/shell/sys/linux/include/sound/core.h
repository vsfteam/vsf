#ifndef __VSF_LINUX_SOUND_CORE_H__
#define __VSF_LINUX_SOUND_CORE_H__

#include <linux/device.h>
#include <linux/module.h>

struct module;
struct snd_device;

struct snd_device {

};

struct snd_card {
    int number;
    char id[16];
    char driver[16];
    char shortname[32];
    char longname[80];

    struct module *module;
    void *private_data;
    struct device *dev;
    struct device card_dev;
};

int snd_card_new(struct device *parent, int idx, const char *xid,
        struct module *module, int extra_size,
        struct snd_card **card_ret);
void snd_card_disconnect(struct snd_card *card);
void snd_card_free(struct snd_card *card);
void snd_card_free_when_closed(struct snd_card *card);

int snd_card_register(struct snd_card *card);

#endif
