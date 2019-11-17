#!/usr/bin/env python
#coding=utf-8

import os
from PIL import Image
from cProfile import Profile

PIX_CNT = 4

def tgui_get_data_from_image(mode, file_path):
    im = Image.open(file_path).convert(mode)
    width, height = im.size
    pixels = im.load()

    cnt = 0
    data = ''
    for y in range(height):
        for x in range(width):
            if cnt % PIX_CNT == 0:
                data += '    '

            if mode == 'RGBA':
                data += '0x%02x, 0x%02x, 0x%02x, 0x%02x,' % pixels[x, y]
            else:
                data += '0x%02x, 0x%02x, 0x%02x,' % pixels[x, y]

            if cnt % PIX_CNT == PIX_CNT - 1:
                data += '\n'
            else:
                data += ' '

            cnt += 1
    return (width, height, data)

def tgui_image_to_dict(mode, file_path):
    image_dict = dict()
    width, height, data = tgui_get_data_from_image(mode, file_path)
    image_dict['name'] = os.path.splitext(os.path.basename(file_path))[0]
    image_dict['data'] = data
    image_dict['width'] = width
    image_dict['height'] = height
    image_dict['mode'] = mode
    if mode == 'RGBA':
        image_dict['color_size'] = 'VSF_TGUI_COLOR_ARGB_8888'
    else:
        image_dict['color_size'] = 'VSF_TGUI_COLOR_RGB8_USER_TEMPLATE'
    return image_dict

def tgui_image_data_buid(image):
    source_output = '''static const char _{name}_{mode}_maps[] = {{
{hex_array}}};
const vsf_tgui_tile_t {name}_{mode}  = {{
    tgui_size({width}, {height}),
    .tTile = {{
        .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)_{name}_{mode}_maps,
        .tAttribute = {{
            .u2ColorSize = {color_size},
            .bIsNoParent = 1,
        }},
    }},
}};

'''.format(hex_array=image['data'],
              name=image['name'],
              width=image['width'],
              height=image['height'],
              mode=image['mode'],
              color_size=image['color_size'])

    header_output = '''extern const vsf_tgui_tile_t {name}_{mode}; //{name}:{mode}, width: {width}, height: {height}
'''.format(
              name=image['name'],
              width=image['width'],
              height=image['height'],
              mode=image['mode'])
    return (source_output, header_output)

def tgui_code_build(file_path, output_str):
    with open(file_path, 'w') as tgt_f:
        with open(file_path + '.template', 'r') as src_f:
            tgt_f.write(src_f.read().replace('STR_REPLACE', output_str))

def main():
    source_output = ''
    header_output = ''

    search_dirs = {'RGB':'./rgb-images', 'RGBA':'./rgba-images'}
    for (mode, dirname) in search_dirs.items():
        files = os.listdir(dirname)
        for f in files:
            file_path = os.path.join(dirname, f)
            if os.path.isfile(file_path):
                image = tgui_image_to_dict(mode, file_path)
                image_source, image_header = tgui_image_data_buid(image)
                source_output += image_source
                header_output += image_header

    tgui_code_build('./demo_images.c', source_output)
    tgui_code_build('./demo_images.h', header_output)

if __name__ == "__main__":
    #prof = Profile()
    #prof.runcall(main)
    #prof.print_stats()
    main()


