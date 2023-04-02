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
#include "service/vsf_service_cfg.h"

#if VSF_USE_LOADER == ENABLED && VSF_LOADER_USE_ELF == ENABLED

#define __VSF_ELFLOADER_CLASS_IMPLEMENT
#define __VSF_LOADER_CLASS_INHERIT__
#include "../vsf_loader.h"

// for vsf_trace
#include "service/vsf_service.h"

/*============================ MACROS ========================================*/

#ifndef VSF_ELFLOADER_CFG_MAX_SYM_LEN
#   define VSF_ELFLOADER_CFG_MAX_SYM_LEN        (32 + 1)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_ELFLOADER_CFG_DEBUG == ENABLED
#   define vsf_elfloader_trace(...)             vsf_trace_debug(__VA_ARGS__)
#   define vsf_elfloader_trace_buffer(...)      vsf_trace_buffer(VSF_TRACE_DEBUG, __VA_ARGS__)
#else
#   define vsf_elfloader_trace(...)
#   define vsf_elfloader_trace_buffer(...)
#endif

/*============================ TYPES =========================================*/

typedef struct vsf_elfloader_info_t {
    Elf_Hdr elf_hdr;
    Elf_Word memsz;

    uintptr_t entry_offset_in_mem;
    uintptr_t entry_offset_in_file;
    bool load_zero_addr;
} vsf_elfloader_info_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

WEAK(vsf_elfloader_relocate_sym)
vsf_err_t vsf_elfloader_relocate_sym(Elf_Addr tgtaddr, int type, Elf_Addr tgtvalue)
{
    switch (type) {
    case R_ARM_JUMP_SLOT:
    case R_ARM_RELATIVE:
        *(uint32_t *)tgtaddr = tgtvalue;
        return VSF_ERR_NONE;
    }
    return VSF_ERR_FAIL;
}

#if 0
static vsf_err_t __vsf_elfloader_relocate(vsf_elfloader_t *elfloader, vsf_loader_target_t *target,
        Elf_Shdr *header, vsf_elfloader_info_t *linfo)
{
    Elf_Word relnum = header->sh_size / header->sh_entsize;
    Elf_Off offset = header->sh_offset;
    Elf_Rel rel;
    Elf_Word relsym;
    Elf_Word reltype;
    Elf_Sym sym;
    Elf_Addr tgtvalue;

    char symname[VSF_ELFLOADER_CFG_MAX_SYM_LEN];
    vsf_elfloader_section_info_t *tgt_sinfo;
    vsf_elfloader_section_info_t *sym_sinfo;

    if (header->sh_entsize != sizeof(rel)) {
        return VSF_ERR_FAIL;
    }

    for (int i = 0; i < relnum; i++, offset += sizeof(rel)) {
        if (vsf_loader_read(target, offset, &rel, sizeof(rel)) != sizeof(rel)) {
            return VSF_ERR_FAIL;
        }

        relsym = ELF_R_SYM(rel.r_info);
        reltype = ELF_R_TYPE(rel.r_info);
        tgt_sinfo = __vsf_elfloader_get_section_by_addr(linfo, rel.r_offset);
        if (NULL == tgt_sinfo) {
            vsf_trace_error("fail to get rel target" VSF_TRACE_CFG_LINEEND);
            return VSF_ERR_FAIL;
        }

        if (0 == relsym) {
            if (tgt_sinfo->buffer == elfloader->got) {
                tgtvalue = *(Elf_Addr *)((Elf_Addr)tgt_sinfo->buffer + rel.r_offset - tgt_sinfo->addr);
                sym_sinfo = __vsf_elfloader_get_section_by_addr(linfo, tgtvalue);
                if (NULL == sym_sinfo) {
                    vsf_trace_error("fail to get rel value" VSF_TRACE_CFG_LINEEND);
                    return VSF_ERR_FAIL;
                }

                if (sym_sinfo->buffer != NULL) {
                    tgtvalue = (Elf_Addr)sym_sinfo->buffer + tgtvalue - sym_sinfo->addr;
                } else if (target->is_xip) {
                    tgtvalue = (Elf_Addr)target->object + tgtvalue;
                } else {
                    vsf_trace_error("fail to get rel value" VSF_TRACE_CFG_LINEEND);
                    return VSF_ERR_FAIL;
                }
            } else {
                vsf_trace_error("unknown rel" VSF_TRACE_CFG_LINEEND);
                return VSF_ERR_FAIL;
            }
        } else {
            if (vsf_loader_read(target, elfloader->symtbl_off + relsym * sizeof(sym), &sym, sizeof(sym)) != sizeof(sym)) {
                return VSF_ERR_FAIL;
            }
            if (!vsf_loader_read(target, elfloader->symstrtbl_off + sym.st_name, symname, sizeof(symname))) {
                return VSF_ERR_FAIL;
            }

            vsf_elfloader_trace("relocate %s" VSF_TRACE_CFG_LINEEND, symname);

            if (SHN_UNDEF == sym.st_shndx) {
                tgtvalue = (Elf_Addr)vsf_loader_link((vsf_loader_t *)elfloader, symname);
                if (0 == tgtvalue) {
                    vsf_trace_error("fail to get %s" VSF_TRACE_CFG_LINEEND, symname);
                    return VSF_ERR_FAIL;
                }
            } else {
                sym_sinfo = __vsf_elfloader_get_section_by_index(linfo, sym.st_shndx);
                if (NULL == sym_sinfo) {
                    vsf_trace_error("fail to get section of %s" VSF_TRACE_CFG_LINEEND, symname);
                    return VSF_ERR_FAIL;
                }
                if (sym_sinfo->buffer != NULL) {
                    tgtvalue = (Elf_Addr)sym_sinfo->buffer + sym.st_value;
                } else if (target->is_xip) {
                    tgtvalue = (Elf_Addr)target->object + sym.st_value;
                } else {
                    vsf_trace_error("fail to get rel value" VSF_TRACE_CFG_LINEEND);
                    return VSF_ERR_FAIL;
                }
            }
        }

        if (VSF_ERR_NONE != vsf_elfloader_relocate_sym((Elf_Addr)tgt_sinfo->buffer + rel.r_offset - tgt_sinfo->addr, reltype, tgtvalue)) {
            vsf_trace_error("fail to link rel" VSF_TRACE_CFG_LINEEND);
        }
    }
    return VSF_ERR_NONE;
}
#endif

void vsf_elfloader_cleanup(vsf_elfloader_t *elfloader)
{
    elfloader->target = NULL;
    if (elfloader->ram_base != NULL) {
        vsf_loader_free(elfloader, VSF_LOADER_MEM_RW, elfloader->ram_base);
        elfloader->ram_base = NULL;
    }
}

static vsf_err_t __vsf_elfloader_load_elfhdr(vsf_loader_target_t *target, Elf_Hdr *header)
{
    if (vsf_loader_read(target, 0, header, sizeof(*header)) != sizeof(*header)) {
        vsf_trace_error("fail to read elf header" VSF_TRACE_CFG_LINEEND);
        return VSF_ERR_FAIL;
    }

    const char Elf_Magic[4] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};
    if (    memcmp(&header->e_ident, Elf_Magic, sizeof(Elf_Magic))
        ||  (header->e_version != EV_CURRENT)
        ||  (header->e_shentsize != sizeof(Elf_Shdr))) {
        vsf_trace_error("invalid elf header" VSF_TRACE_CFG_LINEEND);
        return VSF_ERR_FAIL;
    }
    return VSF_ERR_NONE;
}

int vsf_elfloader_foreach_section(vsf_elfloader_t *elfloader, vsf_loader_target_t *target, void *param,
        int (*callback)(vsf_elfloader_t *, vsf_loader_target_t *, Elf_Shdr *header, char *name, int index, void *param))
{
    VSF_SERVICE_ASSERT(callback != NULL);
    VSF_SERVICE_ASSERT(elfloader != NULL);
    if (NULL == target) {
        target = elfloader->target;
    }
    VSF_SERVICE_ASSERT(target != NULL);

    Elf_Hdr elf_hdr;
    if (VSF_ERR_NONE != __vsf_elfloader_load_elfhdr(target, &elf_hdr)) {
        return -1;
    }

    Elf_Shdr elf_shdr;
    if (vsf_loader_read(target, elf_hdr.e_shoff + elf_hdr.e_shstrndx * elf_hdr.e_shentsize,
            &elf_shdr, sizeof(elf_shdr)) != sizeof(elf_shdr)) {
        vsf_trace_error("fail to read string section" VSF_TRACE_CFG_LINEEND);
        return -1;
    }
    Elf_Word shstrtbl_off = elf_shdr.sh_offset;

    char section_name[VSF_ELFLOADER_CFG_MAX_SYM_LEN];
    int result;
    for (Elf_Half i = 1; i < elf_hdr.e_shnum; i++) {
        if (vsf_loader_read(target, elf_hdr.e_shoff + i * elf_hdr.e_shentsize,
                &elf_shdr, sizeof(elf_shdr)) != sizeof(elf_shdr)) {
            return -1;
        }
        if (!vsf_loader_read(target, shstrtbl_off + elf_shdr.sh_name,
                section_name, sizeof(section_name))) {
            return -1;
        }

        result = callback(elfloader, target, &elf_shdr, section_name, i, param);
        if (result < 0) { return result; } else if (result > 0) { break; }
    }
    return 0;
}

int vsf_elfloader_foreach_program_header(vsf_elfloader_t *elfloader, vsf_loader_target_t *target, void *param,
        int (*callback)(vsf_elfloader_t *, vsf_loader_target_t *, Elf_Phdr *pheader, int index, void *param))
{
    VSF_SERVICE_ASSERT(callback != NULL);
    VSF_SERVICE_ASSERT(elfloader != NULL);
    if (NULL == target) {
        target = elfloader->target;
    }
    VSF_SERVICE_ASSERT(target != NULL);

    Elf_Hdr elf_hdr;
    if (VSF_ERR_NONE != __vsf_elfloader_load_elfhdr(target, &elf_hdr)) {
        return -1;
    }

    Elf_Phdr phdr;
    int result;
    for (Elf_Half i = 0; i < elf_hdr.e_phnum; i++) {
        if (vsf_loader_read(target, elf_hdr.e_phoff + i * elf_hdr.e_phentsize, &phdr, sizeof(phdr)) != sizeof(phdr)) {
            return -1;
        }

        result = callback(elfloader, target, &phdr, i, param);
        if (result < 0) { return result; } else if (result > 0) { break; }
    }
    return 0;
}

int vsf_elfloader_call_init_array(vsf_elfloader_t *elfloader)
{
    if (elfloader->initarr != 0) {
        // TODO: call init
        VSF_SERVICE_ASSERT(false);

        vsf_loader_free(elfloader, VSF_LOADER_MEM_RW, elfloader->initarr);
        elfloader->initarr = NULL;
        return -1;
    }
    return 0;
}

void vsf_elfloader_call_fini_array(vsf_elfloader_t *elfloader)
{
    if (elfloader->initarr != 0) {
        // TODO: call fini
        VSF_SERVICE_ASSERT(false);

        vsf_loader_free(elfloader, VSF_LOADER_MEM_RW, elfloader->finiarr);
        elfloader->finiarr = NULL;
    }
}

static int __vsf_elfloader_load_cb(vsf_elfloader_t *elfloader, vsf_loader_target_t *target,
        Elf_Phdr *header, int index, void *param)
{
    vsf_elfloader_info_t *linfo = param;
    if (header->p_type == PT_LOAD) {
        if (header->p_memsz < header->p_filesz) {
            return VSF_ELFLOADER_CB_FAIL;
        }

        if (    (linfo->elf_hdr.e_entry >= header->p_vaddr)
            &&  (linfo->elf_hdr.e_entry < (header->p_vaddr + header->p_memsz))) {
            linfo->entry_offset_in_mem = header->p_vaddr + linfo->elf_hdr.e_entry;
            linfo->entry_offset_in_file = header->p_offset + linfo->elf_hdr.e_entry;
        }
        if (0 == header->p_vaddr) {
            linfo->load_zero_addr = true;
        }

        if (target->is_xip && !(header->p_flags & PF_W)) {
            return VSF_ELFLOADER_CB_GOON;
        }

        if (elfloader->ram_base != NULL) {
            if (0 == header->p_filesz) {
                return VSF_ELFLOADER_CB_GOON;
            }

            if (vsf_loader_read(target, header->p_offset, (uint8_t *)elfloader->ram_base + header->p_vaddr, header->p_filesz) != header->p_filesz) {
                return VSF_ELFLOADER_CB_FAIL;
            }
        } else {
            Elf_Word mem_max = header->p_vaddr + header->p_memsz;
            if (linfo->memsz < mem_max) {
                linfo->memsz = mem_max;
            }
        }
    } else if (header->p_type == PT_DYNAMIC) {
        vsf_trace_error("relocating is not supported yet" VSF_TRACE_CFG_LINEEND);
        return VSF_ELFLOADER_CB_FAIL;
    }
    return VSF_ELFLOADER_CB_GOON;
}

void * vsf_elfloader_load(vsf_elfloader_t *elfloader, vsf_loader_target_t *target)
{
    VSF_SERVICE_ASSERT((elfloader != NULL) && (target != NULL));
    VSF_SERVICE_ASSERT(NULL == elfloader->target);
    VSF_SERVICE_ASSERT(target->fn_read != NULL);

    vsf_elfloader_info_t linfo;
    if (VSF_ERR_NONE != __vsf_elfloader_load_elfhdr(target, &linfo.elf_hdr)) {
        return NULL;
    }

    linfo.memsz = 0;
    linfo.load_zero_addr = false;

second_round_for_ram_base:
    if (vsf_elfloader_foreach_program_header(elfloader, target, &linfo, __vsf_elfloader_load_cb) < 0) {
        goto cleanup_and_fail;
    }
    if (!linfo.load_zero_addr) {
        vsf_trace_error("is the image position independent?" VSF_TRACE_CFG_LINEEND);
        goto cleanup_and_fail;
    }

    if ((linfo.memsz > 0) && (NULL == elfloader->ram_base)) {
        elfloader->ram_base = vsf_loader_malloc(elfloader, VSF_LOADER_MEM_RWX, linfo.memsz, 0);
        if (NULL == elfloader->ram_base) {
            vsf_trace_error("fail to allocate ram_base" VSF_TRACE_CFG_LINEEND);
            goto cleanup_and_fail;
        }
        memset(elfloader->ram_base, 0, linfo.memsz);
        goto second_round_for_ram_base;
    }

    // todo: when should static_base be set to got base?
    elfloader->static_base = elfloader->ram_base;
    elfloader->target = target;
    if (target->is_xip) {
        return (void *)(target->object + linfo.entry_offset_in_file);
    } else {
        return (void *)((uintptr_t)elfloader->ram_base + linfo.entry_offset_in_mem);
    }

cleanup_and_fail:
    vsf_elfloader_cleanup(elfloader);
    return NULL;
}

// vsf_elfloader_get_section
typedef struct __vsf_elfloader_get_section_t {
    const char *name;
    Elf_Shdr *header;
    uint32_t offset;
} __vsf_elfloader_get_section_t;

static int __vsf_elfloader_get_section_cb(vsf_elfloader_t *elfloader, vsf_loader_target_t *target,
        Elf_Shdr *header, char *name, int index, void *param)
{
    __vsf_elfloader_get_section_t *p = (__vsf_elfloader_get_section_t *)param;
    if (!strcmp(p->name, name)) {
        if (p->header != NULL) {
            *(p->header) = *header;
        }
        p->offset = header->sh_offset;
        return VSF_ELFLOADER_CB_DONE;
    }
    return VSF_ELFLOADER_CB_GOON;
}

uint32_t vsf_elfloader_get_section(vsf_elfloader_t *elfloader, vsf_loader_target_t *target,
        const char *name, Elf_Shdr *header)
{
    __vsf_elfloader_get_section_t param = {
        .name = name,
    };
    vsf_elfloader_foreach_section(elfloader, target, &param, __vsf_elfloader_get_section_cb);
    return param.offset;
}

#endif      // VSF_USE_LOADER && VSF_LOADER_USE_ELF
