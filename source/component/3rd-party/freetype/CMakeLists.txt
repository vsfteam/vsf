if(NOT VSF_FREETYPE_PATH)
    set(VSF_FREETYPE_PATH ${VSF_SRC_PATH}/component/3rd-party/freetype/raw)
endif()

target_include_directories(${VSF_LIB_NAME} INTERFACE
    port
    ${VSF_FREETYPE_PATH}/include
)
target_compile_definitions(${VSF_LIB_NAME} INTERFACE
    "FT2_BUILD_LIBRARY"
)

target_sources(${VSF_LIB_NAME} INTERFACE
    ${VSF_FREETYPE_PATH}/src/base/ftbase.c
    ${VSF_FREETYPE_PATH}/src/base/ftbbox.c
    ${VSF_FREETYPE_PATH}/src/base/ftbdf.c
    ${VSF_FREETYPE_PATH}/src/base/ftbitmap.c
    ${VSF_FREETYPE_PATH}/src/base/ftcid.c
    ${VSF_FREETYPE_PATH}/src/base/ftdebug.c
    ${VSF_FREETYPE_PATH}/src/base/ftfstype.c
    ${VSF_FREETYPE_PATH}/src/base/ftgasp.c
    ${VSF_FREETYPE_PATH}/src/base/ftglyph.c
    ${VSF_FREETYPE_PATH}/src/base/ftgxval.c
    ${VSF_FREETYPE_PATH}/src/base/ftinit.c
    ${VSF_FREETYPE_PATH}/src/base/ftmm.c
    ${VSF_FREETYPE_PATH}/src/base/ftotval.c
    ${VSF_FREETYPE_PATH}/src/base/ftpfr.c
    ${VSF_FREETYPE_PATH}/src/base/ftstroke.c
    ${VSF_FREETYPE_PATH}/src/base/ftsynth.c
    ${VSF_FREETYPE_PATH}/src/base/ftsystem.c
    ${VSF_FREETYPE_PATH}/src/base/fttype1.c
    ${VSF_FREETYPE_PATH}/src/base/ftwinfnt.c

    ${VSF_FREETYPE_PATH}/src/autofit/autofit.c
    ${VSF_FREETYPE_PATH}/src/bdf/bdf.c
    ${VSF_FREETYPE_PATH}/src/bzip2/ftbzip2.c
    ${VSF_FREETYPE_PATH}/src/cache/ftcache.c
    ${VSF_FREETYPE_PATH}/src/cff/cff.c
    ${VSF_FREETYPE_PATH}/src/cid/type1cid.c
    ${VSF_FREETYPE_PATH}/src/gxvalid/gxvalid.c
    ${VSF_FREETYPE_PATH}/src/gzip/ftgzip.c
    ${VSF_FREETYPE_PATH}/src/lzw/ftlzw.c
    ${VSF_FREETYPE_PATH}/src/otvalid/otvalid.c
    ${VSF_FREETYPE_PATH}/src/pcf/pcf.c
    ${VSF_FREETYPE_PATH}/src/pfr/pfr.c
    ${VSF_FREETYPE_PATH}/src/psaux/psaux.c
    ${VSF_FREETYPE_PATH}/src/pshinter/pshinter.c
    ${VSF_FREETYPE_PATH}/src/psnames/psnames.c
    ${VSF_FREETYPE_PATH}/src/raster/raster.c
    ${VSF_FREETYPE_PATH}/src/sfnt/sfnt.c
    ${VSF_FREETYPE_PATH}/src/smooth/smooth.c
    ${VSF_FREETYPE_PATH}/src/truetype/truetype.c
    ${VSF_FREETYPE_PATH}/src/type1/type1.c
    ${VSF_FREETYPE_PATH}/src/type42/type42.c
    ${VSF_FREETYPE_PATH}/src/winfonts/winfnt.c
)
