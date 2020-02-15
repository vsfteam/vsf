#!/usr/bin/env python
#coding=utf-8

import os
from PIL import Image
from cProfile import Profile

PIX_CNT = 4
rgba_index = 0
rgb_index = 0

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
        image_dict['color_size'] = 5
        image_dict['color_type'] = 1
    else:
        image_dict['color_size'] = 6
        image_dict['color_type'] = 0
    return image_dict

def tgui_image_data_buid(image):
    global rgba_index
    global rgb_index

    tile_rgb_size  = ''
    tile_rgba_size  = ''
    tile_rgba_array = ''
    tile_rgb_array = ''

    if image['mode'] == 'RGBA':
        index = rgba_index
        rgba_index += 1
        tile_rgba_size = '    {{.nWidth = {width}, .nHeight = {height}}},\n'.format(width=image['width'], height=image['height'])
        rgba_pixmap = '    '
        tile_rgba_array = '    _{name}_{mode}_maps,\n'.format(name=image['name'], mode=image['mode'])
    else:
        index = rgb_index
        rgb_index += 1
        tile_rgb_size = '    {{.nWidth = {width}, .nHeight = {height}}},\n'.format(width=image['width'], height=image['height'])
        tile_rgb_array = '    _{name}_{mode}_maps,\n'.format(name=image['name'], mode=image['mode'])

    ## tile define
    tile_def = '''const vsf_tgui_tile_idx_root_t {name}_{mode}  = {{
    .use_as__vsf_tgui_tile_core_t = {{
        .tAttribute = {{
            .u2RootTileType = 1,
            .u2ColorType = {color_type},
            .u3ColorSize = {color_size},
            .bIsRootTile = 1,
        }},
    }},
    .chIndex = {index}
}};

'''.format(hex_array=image['data'],
              name=image['name'],
              width=image['width'],
              height=image['height'],
              mode=image['mode'],
              color_size=image['color_size'],
              color_type=image['color_type'],
              index=index)


    ## tile data
    tile_data = '''static const char _{name}_{mode}_maps[] = {{
{hex_array}}};
'''.format(hex_array=image['data'],
              name=image['name'],
              mode=image['mode'])

    ## tile header
    tile_header = '''extern const vsf_tgui_tile_t {name}_{mode}; //{name}:{mode}, width: {width}, height: {height}
'''.format(
              name=image['name'],
              width=image['width'],
              height=image['height'],
              mode=image['mode'])
    return (tile_def, tile_data, tile_rgb_size,  tile_rgba_size, tile_rgb_array, tile_rgba_array, tile_header)

def tgui_code_build(file_path, output_str):
    with open(file_path, 'w') as tgt_f:
        with open(file_path + '.template', 'r') as src_f:
            tgt_f.write(src_f.read().replace('STR_REPLACE', output_str))

def main():
    tiles_def        = ''
    tiles_data       = ''
    tiles_header     = ''
    tiles_rgb_size   = ''
    tiles_rgba_size  = ''
    tiles_rgb_array  = ''
    tiles_rgba_array = ''

    search_dirs = {'RGB':'./rgb-images', 'RGBA':'./rgba-images'}
    for (mode, dirname) in search_dirs.items():
        files = os.listdir(dirname)
        for f in files:
            file_path = os.path.join(dirname, f)
            if os.path.isfile(file_path):
                image = tgui_image_to_dict(mode, file_path)
                tile_def, tile_data, tile_rgb_size, tile_rgba_size, tile_rgb_array, tile_rgba_array, tile_header = tgui_image_data_buid(image)

                tiles_def        += tile_def
                tiles_data       += tile_data
                tiles_rgb_size   += tile_rgb_size
                tiles_rgba_size  += tile_rgba_size
                tiles_rgb_array  += tile_rgb_array
                tiles_rgba_array += tile_rgba_array
                tiles_header     += tile_header

    tiles_def += '\nvsf_tgui_size_t gIdxRootRGBTileSizes[] = {\n' + tiles_rgb_size + '\n};\n'
    tiles_def += '\nvsf_tgui_size_t gIdxRootRGBATileSizes[] = {\n' + tiles_rgba_size + '\n};\n'

    tiles_header += '\nextern vsf_tgui_size_t gIdxRootRGBTileSizes[' + str(rgb_index) + '];\n'
    tiles_header += '\nextern vsf_tgui_size_t gIdxRootRGBATileSizes[' + str(rgba_index) + '];\n'

    tiles_data += '\nstatic const char * rgb_pixmap_array[] = {\n' + tiles_rgb_array + '};\n'
    tiles_data += '\nstatic const char * rgba_pixmap_array[] = {\n' + tiles_rgba_array + '};\n'

    tgui_code_build('./demo_images.c', tiles_def)
    tgui_code_build('./demo_images.h', tiles_header)
    tgui_code_build('./demo_images_data.h', tiles_data)

if __name__ == "__main__":
    #prof = Profile()
    #prof.runcall(main)
    #prof.print_stats()
    main()



