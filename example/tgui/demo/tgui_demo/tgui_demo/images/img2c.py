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
    im_bytes = im.tobytes()
    im_byte_strs = ['0x%02x,' % b for b in im_bytes]

    chunks, chunk_size = len(im_byte_strs), 12
    im_byte_strs = [ '    ' + ' '.join(im_byte_strs[i: i + chunk_size]) + '\n' for i in range(0, chunks, chunk_size) ]
    data = ''.join(im_byte_strs)

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
        image_dict['size'] = width * height * 4
    else:
        image_dict['color_size'] = 6
        image_dict['color_type'] = 0
        image_dict['size'] = width * height * 3
    return image_dict

def tgui_image_data_buid(image, offset):
    global rgba_index
    global rgb_index

    if image['mode'] == 'RGBA':
        index = rgba_index
        rgba_index += 1
        rgba_pixmap = '    '
    else:
        index = rgb_index
        rgb_index += 1

    ## tile define
    tile_def = '''const vsf_tgui_tile_buf_root_t {name}_{mode}  = {{
    .use_as__vsf_tgui_tile_core_t = {{
        .Attribute = {{
            .u2RootTileType = 0,
            .u2ColorType = {color_type},
            .u3ColorSize = {color_size},
            .bIsRootTile = 1,
        }},
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
        .name_ptr = "{name}_{mode}",
#endif
    }},
    .tSize = {{
        .iWidth = {width},
        .iHeight = {height},
    }},
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0x{offset:02X},
}};

'''.format(hex_array=image['data'],
              name=image['name'],
              width=image['width'],
              height=image['height'],
              mode=image['mode'],
              color_size=image['color_size'],
              color_type=image['color_type'],
              offset=offset)


    ## tile data
    tile_data = '''    /**** {name}_{mode} ****/
{hex_array}
'''.format(hex_array=image['data'],
              name=image['name'],
              mode=image['mode'])

    ## tile header
    tile_ref = '''    (const vsf_tgui_tile_t *)&{name}_{mode}, //{name}:{mode}, width: {width}, height: {height}
'''.format(name=image['name'], width=image['width'],
           height=image['height'], mode=image['mode'])

    ## tile header
    tile_header = '''extern const vsf_tgui_tile_buf_root_t {name}_{mode}; //{name}:{mode}, width: {width}, height: {height}
'''.format(
              name=image['name'],
              width=image['width'],
              height=image['height'],
              mode=image['mode'])

    ## tile list for text list
    tile_text_list = '    "{name}\\n" /*width: {width}, height: {height}, {mode}*/ \\\n'.format(
              name=image['name'],
              width=image['width'],
              height=image['height'],
              mode=image['mode'])
    return (tile_def, tile_ref, tile_data, tile_header, tile_text_list)

def tgui_code_build(file_path, replace_list):
    with open(file_path, 'w') as tgt_f:
        with open(file_path + '.template', 'r') as src_f:
            src_buf = src_f.read()
            for (replace_str, output_str) in replace_list:
                src_buf = src_buf.replace(replace_str, output_str)
            tgt_f.write(src_buf)

def main():
    tiles_def        = ''
    tiles_data       = ''
    tiles_header     = ''
    tiles_def_list   = ''
    tiles_text_list  = ''
    offset = 0
    counter = 0

    search_dirs = {'RGB':'./rgb-images', 'RGBA':'./rgba-images'}
    for (mode, dirname) in search_dirs.items():
        files = os.listdir(dirname)
        for f in files:
            file_path = os.path.join(dirname, f)
            if os.path.isfile(file_path):
                image = tgui_image_to_dict(mode, file_path)
                tile_def, tile_ref, tile_data, tile_header, tile_text_list = tgui_image_data_buid(image, offset)

                tiles_def        += tile_def
                tiles_header     += tile_header

                tiles_data       += tile_data
                offset           += image['size']

                tiles_def_list   += tile_ref
                tiles_text_list  += tile_text_list

                counter          += 1

    tiles_data = '''static const char __tiles_data[] = {{
{hex_array}}};
'''.format(hex_array=tiles_data)

    tiles_def_list = '''const vsf_tgui_tile_t* tiles_list[{counter}] = {{
{tiles_def_list}}};
'''.format(counter=counter, tiles_def_list=tiles_def_list)
    tiles_def += tiles_def_list

    tiles_header += '\nextern const vsf_tgui_tile_t *tiles_list[{counter}];\n'.format(counter=counter)

    tiles_text_list = '#define TILES_TEXT_LIST \\\n{tiles_text_list}    "None"\n'.format(tiles_text_list=tiles_text_list)

    tgui_code_build('./demo_images.c', [('GLOBAL_REPLACE', tiles_def)])
    tgui_code_build('./demo_images.h', [('GLOBAL_REPLACE', tiles_header), ('MACROS_REPLACE', tiles_text_list)])
    tgui_code_build('./demo_images_data.h', [('IMAGES_DATAS', tiles_data)])

if __name__ == "__main__":
    #prof = Profile()
    #prof.runcall(main)
    #prof.print_stats()
    main()



