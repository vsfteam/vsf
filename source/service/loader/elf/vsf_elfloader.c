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
#   define vsf_elfloader_debug(...)             vsf_trace_debug(__VA_ARGS__)
#   define vsf_elfloader_debug_buffer(...)      vsf_trace_buffer(VSF_TRACE_DEBUG, __VA_ARGS__)
#else
#   define vsf_elfloader_debug(...)
#   define vsf_elfloader_debug_buffer(...)
#endif

/*============================ TYPES =========================================*/

typedef struct vsf_elfloader_info_t {
    Elf_Hdr elf_hdr;
    Elf_Word memsz;
    Elf_Word memstart_xip;
    Elf_Word memsz_xip;

    // for .dynamic
    struct {
        Elf_Addr pltrelsz;
        Elf_Addr strtbl;
        Elf_Addr symtbl;
        Elf_Addr rela;
        Elf_Addr relasz;
        Elf_Addr strsz;
        Elf_Addr rel;
        Elf_Addr relsz;
        Elf_Addr pltrel;
        Elf_Addr jmprel;

        uint32_t external_fn_num;
        uint32_t export_num;
        uint32_t export_strlen;
        vsf_vplt_entry_t *export_vplt_entry;
        char *export_str;
    } dynamic;

    uintptr_t entry_offset_in_file;
    bool load_zero_addr;
    bool has_dynamic;
} vsf_elfloader_info_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

// vsf_elfloader_arch_relocate_sym will be over-written by the same function in
//  the source code for specified arch.
WEAK(vsf_elfloader_arch_relocate_sym)
int vsf_elfloader_arch_relocate_sym(Elf_Addr tgtaddr, int type, Elf_Addr tgtvalue)
{
    return -1;
}

// vsf_elfloader_arch_init_plt is used to allocate necessary resources before linking
WEAK(vsf_elfloader_arch_init_plt)
int vsf_elfloader_arch_init_plt(vsf_elfloader_t *elfloader, int num)
{
    return 0;
}

// vsf_elfloader_arch_fini_plt is used to free necessary resources after linking
WEAK(vsf_elfloader_arch_fini_plt)
void vsf_elfloader_arch_fini_plt(vsf_elfloader_t *elfloader) { }

// vsf_elfloader_link should be over-written by user to do syjmbol linking
WEAK(vsf_elfloader_link)
int vsf_elfloader_link(vsf_elfloader_t *elfloader, char *symname, Elf_Addr *target)
{
#if VSF_USE_APPLET == ENABLED && VSF_APPLET_CFG_LINKABLE == ENABLED
    if (NULL == elfloader->vplt) {
        return -1;
    }

    void *fn = vsf_vplt_link(elfloader->vplt, symname);
    *target = (Elf_Addr)fn;
    return NULL == fn ? -1 : 0;
#else
    vsf_trace_error("relocating is not supported yet" VSF_TRACE_CFG_LINEEND);
    return -1;
#endif
}

WEAK(vsf_elfloader_arch_link)
int vsf_elfloader_arch_link(vsf_elfloader_t *elfloader, char *symname, Elf_Addr *target)
{
    return vsf_elfloader_link(elfloader, symname, target);
}

void vsf_elfloader_cleanup(vsf_elfloader_t *elfloader)
{
    elfloader->target = NULL;
    vsf_elfloader_arch_fini_plt(elfloader);
    if (elfloader->ram_base != NULL) {
        vsf_loader_free(elfloader, VSF_LOADER_MEM_RW, elfloader->ram_base);
        elfloader->ram_base = NULL;
    }
}

static int __vsf_elfloader_load_elfhdr(vsf_loader_target_t *target, Elf_Hdr *header)
{
    if (vsf_loader_read(target, 0, header, sizeof(*header)) != sizeof(*header)) {
        vsf_trace_error("fail to read elf header" VSF_TRACE_CFG_LINEEND);
        return -1;
    }

    const char Elf_Magic[4] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};
    if (    memcmp(&header->e_ident, Elf_Magic, sizeof(Elf_Magic))
        ||  (header->e_version != EV_CURRENT)
        ||  (header->e_shentsize != sizeof(Elf_Shdr))) {
        vsf_trace_error("invalid elf header" VSF_TRACE_CFG_LINEEND);
        return -1;
    }
    return 0;
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
            linfo->entry_offset_in_file = header->p_offset + linfo->elf_hdr.e_entry;
        }
        if (0 == header->p_vaddr) {
            linfo->load_zero_addr = true;
        }

        if (elfloader->ram_base != NULL) {
            bool is_xip = target->support_xip && !linfo->has_dynamic;
            if ((0 == header->p_filesz) || (is_xip && (0 == (header->p_flags & PF_W)))) {
                return VSF_ELFLOADER_CB_GOON;
            }

            Elf_Word memstart = is_xip ? linfo->memstart_xip : 0;
            if (vsf_loader_read(target, header->p_offset, (uint8_t *)elfloader->ram_base + header->p_vaddr - memstart, header->p_filesz) != header->p_filesz) {
                return VSF_ELFLOADER_CB_FAIL;
            }
        } else {
            Elf_Word mem_max = header->p_vaddr + header->p_memsz;
            if (linfo->memsz < mem_max) {
                linfo->memsz = mem_max;
            }
            if (target->support_xip && ((header->p_flags & PF_W) != 0)) {
                linfo->memsz_xip += header->p_memsz;
                if (linfo->memstart_xip > header->p_vaddr) {
                    linfo->memstart_xip = header->p_vaddr;
                }
            }
        }
    } else if (header->p_type == PT_DYNAMIC) {
        Elf_Dyn dyn;
        int i = 0;
        do {
            if (vsf_loader_read(target, header->p_offset + i * sizeof(dyn), &dyn, sizeof(dyn)) != sizeof(dyn)) {
                return VSF_ELFLOADER_CB_FAIL;
            }
            switch (dyn.d_tag) {
            case DT_PLTRELSZ:   linfo->dynamic.pltrelsz = dyn.d_un.d_ptr;   break;
            case DT_STRTAB:     linfo->dynamic.strtbl = dyn.d_un.d_ptr;     break;
            case DT_SYMTAB:     linfo->dynamic.symtbl = dyn.d_un.d_ptr;     break;
            case DT_RELA:       linfo->dynamic.rela = dyn.d_un.d_ptr;       break;
            case DT_RELASZ:     linfo->dynamic.relasz = dyn.d_un.d_ptr;     break;
            case DT_STRSZ:      linfo->dynamic.strsz = dyn.d_un.d_ptr;      break;
            case DT_REL:        linfo->dynamic.rel = dyn.d_un.d_ptr;        break;
            case DT_RELSZ:      linfo->dynamic.relsz = dyn.d_un.d_ptr;      break;
            case DT_PLTREL:     linfo->dynamic.pltrel = dyn.d_un.d_ptr;     break;
            case DT_JMPREL:     linfo->dynamic.jmprel = dyn.d_un.d_ptr;     break;
            }
            i++;
        } while (dyn.d_tag != DT_NULL);
        linfo->has_dynamic = true;
    }
    return VSF_ELFLOADER_CB_GOON;
}

static int __vsf_elfloader_rel_rela(vsf_elfloader_t *elfloader, vsf_elfloader_info_t *linfo, int type, void *rel_rela, Elf_Addr size, bool parse_only)
{
    Elf_Word relsym, reltype;
    Elf_Sym sym;
    Elf_Addr tgtvalue;
    char *symname;
    union {
        Elf_Rel *rel;
        Elf_Rela *rela;
        uint8_t *ptr;
    } u;
    int ent = (type == DT_REL) ? sizeof(Elf_Rel) : sizeof(Elf_Rela);
    u.ptr = rel_rela;

    for (Elf_Addr i = 0; i < size; i += sizeof(Elf_Rela), u.ptr += ent) {
        relsym = ELF_R_SYM(u.rel->r_info);
        reltype = ELF_R_TYPE(u.rel->r_info);
        sym = ((Elf_Sym *)linfo->dynamic.symtbl)[relsym];
        symname = (char *)(linfo->dynamic.strtbl + sym.st_name);

        if (0 == sym.st_shndx) {
            if (STT_FUNC != ELF_ST_TYPE(sym.st_info)) {
                vsf_trace_error("only support relocate external functions" VSF_TRACE_CFG_LINEEND);
                return -1;
            }
            linfo->dynamic.external_fn_num++;
            if (parse_only) { continue; }

            if (vsf_elfloader_arch_link(elfloader, symname, &tgtvalue) < 0) {
                vsf_trace_error("fail to locate %s" VSF_TRACE_CFG_LINEEND, symname);
                return -1;
            }
            if (type == DT_RELA) {
                tgtvalue += u.rela->r_addend;
            }
        } else {
            tgtvalue = (Elf_Addr)elfloader->ram_base + sym.st_value;
            if (type == DT_RELA) {
                tgtvalue += u.rela->r_addend;
            }

            if (STB_GLOBAL == ELF_ST_BIND(sym.st_info)) {
                int symnamelen = strlen(symname) + 1;
                linfo->dynamic.export_num++;
                linfo->dynamic.export_strlen += symnamelen;

                if ((linfo->dynamic.export_vplt_entry != NULL) && (linfo->dynamic.export_str != NULL)) {
                    linfo->dynamic.export_vplt_entry->name = linfo->dynamic.export_str;
                    linfo->dynamic.export_vplt_entry->ptr = (void *)tgtvalue;
                    strcpy(linfo->dynamic.export_str, symname);
                    linfo->dynamic.export_str += symnamelen;
                    linfo->dynamic.export_vplt_entry++;
                }
            }
            if (parse_only) { continue; }
        }

        vsf_elfloader_debug("relocate %s to 0x%X" VSF_TRACE_CFG_LINEEND, symname, tgtvalue);
        if (vsf_elfloader_arch_relocate_sym((Elf_Addr)elfloader->ram_base + u.rel->r_offset, reltype, tgtvalue) < 0) {
            vsf_trace_error("fail to relocate %s" VSF_TRACE_CFG_LINEEND, symname);
            return -1;
        }
    }
    return 0;
}

int vsf_elfloader_load(vsf_elfloader_t *elfloader, vsf_loader_target_t *target)
{
    VSF_SERVICE_ASSERT((elfloader != NULL) && (target != NULL));
    VSF_SERVICE_ASSERT(NULL == elfloader->target);
    VSF_SERVICE_ASSERT(target->fn_read != NULL);

    vsf_elfloader_info_t linfo;
    if (VSF_ERR_NONE != __vsf_elfloader_load_elfhdr(target, &linfo.elf_hdr)) {
        return -1;
    }

    linfo.memsz = 0;
    linfo.memsz_xip = 0;
    linfo.memstart_xip = (Elf_Word)-1;
    memset(&linfo.dynamic, 0, sizeof(linfo.dynamic));
    linfo.load_zero_addr = false;
    linfo.has_dynamic = false;

second_round_for_ram_base:
    if (vsf_elfloader_foreach_program_header(elfloader, target, &linfo, __vsf_elfloader_load_cb) < 0) {
        goto cleanup_and_fail;
    }
    if (!linfo.load_zero_addr) {
        vsf_trace_error("is the image position independent?" VSF_TRACE_CFG_LINEEND);
        goto cleanup_and_fail;
    }

    Elf_Word memsz;
    vsf_loader_mem_attr_t memattr;
    bool is_xip = target->support_xip && !linfo.has_dynamic;
    if (is_xip) {
        memsz = linfo.memsz_xip;
        memattr = VSF_LOADER_MEM_RW;
    } else {
        memsz = linfo.memsz;
        memattr = VSF_LOADER_MEM_RWX;
    }

    if ((memsz > 0) && (NULL == elfloader->ram_base)) {
        elfloader->ram_base = vsf_loader_malloc(elfloader, memattr, memsz, 0);
        if (NULL == elfloader->ram_base) {
            vsf_trace_error("fail to allocate ram_base" VSF_TRACE_CFG_LINEEND);
            goto cleanup_and_fail;
        }
        memset(elfloader->ram_base, 0, memsz);
        goto second_round_for_ram_base;
    }

    // relocating
    if (linfo.has_dynamic) {
        linfo.dynamic.symtbl += (Elf_Addr)elfloader->ram_base;
        linfo.dynamic.strtbl += (Elf_Addr)elfloader->ram_base;
        if (linfo.dynamic.pltrelsz > 0) {
            linfo.dynamic.jmprel += (Elf_Addr)elfloader->ram_base;
            if (__vsf_elfloader_rel_rela(elfloader, &linfo, linfo.dynamic.pltrel, (void *)linfo.dynamic.jmprel, linfo.dynamic.pltrelsz, true) < 0) {
                goto cleanup_and_fail;
            }
        }
        if (linfo.dynamic.relasz > 0) {
            linfo.dynamic.rela += (Elf_Addr)elfloader->ram_base;
            if (__vsf_elfloader_rel_rela(elfloader, &linfo, DT_RELA, (void *)linfo.dynamic.rela, linfo.dynamic.relasz, true) < 0) {
                goto cleanup_and_fail;
            }
        }
        if (linfo.dynamic.relsz > 0) {
            linfo.dynamic.rel += (Elf_Addr)elfloader->ram_base;
            if (__vsf_elfloader_rel_rela(elfloader, &linfo, DT_REL, (void *)linfo.dynamic.rel, linfo.dynamic.relsz, true) < 0) {
                goto cleanup_and_fail;
            }
        }

        if (vsf_elfloader_arch_init_plt(elfloader, linfo.dynamic.external_fn_num) < 0) {
            goto cleanup_and_fail;
        }
        if ((linfo.dynamic.export_num > 0) && (elfloader->allocate_vplt != NULL)) {
            int total_byte_size = sizeof(vsf_vplt_info_t) + linfo.dynamic.export_num * sizeof(vsf_vplt_entry_t) + linfo.dynamic.export_strlen;
            elfloader->vplt_out = elfloader->allocate_vplt(total_byte_size);
            if (NULL == elfloader->vplt_out) {
                goto cleanup_and_fail;
            }

            vsf_vplt_info_t *vplt_info = elfloader->vplt_out;
            vplt_info->entry_num = linfo.dynamic.export_num;
            vplt_info->final = true;
            vplt_info->major = 0;
            vplt_info->minor = 0;
            linfo.dynamic.export_vplt_entry = (vsf_vplt_entry_t *)&vplt_info[1];
            linfo.dynamic.export_str = (char *)linfo.dynamic.export_vplt_entry + linfo.dynamic.export_num * sizeof(vsf_vplt_entry_t);
        }
        linfo.dynamic.export_num = 0;
        linfo.dynamic.external_fn_num = 0;
        linfo.dynamic.export_strlen = 0;

        if (linfo.dynamic.pltrelsz > 0) {
            if (__vsf_elfloader_rel_rela(elfloader, &linfo, linfo.dynamic.pltrel, (void *)linfo.dynamic.jmprel, linfo.dynamic.pltrelsz, false) < 0) {
                goto cleanup_and_fail;
            }
        }
        if (linfo.dynamic.relasz > 0) {
            if (__vsf_elfloader_rel_rela(elfloader, &linfo, DT_RELA, (void *)linfo.dynamic.rela, linfo.dynamic.relasz, false) < 0) {
                goto cleanup_and_fail;
            }
        }
        if (linfo.dynamic.relsz > 0) {
            if (__vsf_elfloader_rel_rela(elfloader, &linfo, DT_REL, (void *)linfo.dynamic.rel, linfo.dynamic.relsz, false) < 0) {
                goto cleanup_and_fail;
            }
        }
    }

    // todo: when should static_base be set to got base?
    elfloader->static_base = elfloader->ram_base;
    elfloader->target = target;
    elfloader->entry = is_xip ? (void *)(target->object + linfo.entry_offset_in_file)
                            :   (void *)((uintptr_t)elfloader->ram_base + linfo.elf_hdr.e_entry);
    return 0;

cleanup_and_fail:
    vsf_elfloader_cleanup(elfloader);
    return -1;
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
        const char *name)
{
    __vsf_elfloader_get_section_t param = {
        .name = name,
    };
    vsf_elfloader_foreach_section(elfloader, target, &param, __vsf_elfloader_get_section_cb);
    return param.offset;
}

#endif      // VSF_USE_LOADER && VSF_LOADER_USE_ELF
