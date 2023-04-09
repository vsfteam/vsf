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

#if VSF_USE_LOADER == ENABLED && VSF_LOADER_USE_PE == ENABLED

#define __VSF_PELOADER_CLASS_IMPLEMENT
#define __VSF_LOADER_CLASS_INHERIT__
#include "../vsf_loader.h"

// for vsf_trace
#include "service/vsf_service.h"

#include <Windows.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_PELOADER_CFG_DEBUG ENABLED
#if VSF_PELOADER_CFG_DEBUG == ENABLED
#   define vsf_peloader_debug(...)              vsf_trace_debug(__VA_ARGS__)
#   define vsf_peloader_debug_buffer(...)       vsf_trace_buffer(VSF_TRACE_DEBUG, __VA_ARGS__)
#else
#   define vsf_peloader_debug(...)
#   define vsf_peloader_debug_buffer(...)
#endif

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const struct vsf_loader_op_t vsf_peloader_op = {
    .fn_load                = (int (*)(vsf_loader_t *, vsf_loader_target_t *))vsf_peloader_load,
    .fn_cleanup             = (void (*)(vsf_loader_t *))vsf_peloader_cleanup,
    .fn_call_init_array     = (int (*)(vsf_loader_t *))vsf_peloader_call_init_array,
    .fn_call_fini_array     = (void (*)(vsf_loader_t *))vsf_peloader_call_fini_array,
};

/*============================ IMPLEMENTATION ================================*/

int vsf_peloader_load(vsf_peloader_t *peloader, vsf_loader_target_t *target)
{
    IMAGE_DOS_HEADER dosheader;
    if (vsf_loader_read(target, 0, &dosheader, sizeof(dosheader)) != sizeof(dosheader)) {
        vsf_trace_error("fail to read pe dosheader" VSF_TRACE_CFG_LINEEND);
        return -1;
    }
    if (dosheader.e_magic != IMAGE_DOS_SIGNATURE) {
        vsf_trace_error("invalid magic for dosheader 0x%02X" VSF_TRACE_CFG_LINEEND, dosheader.e_magic);
        return -1;
    }

    IMAGE_NT_HEADERS ntheader;
    if (vsf_loader_read(target, dosheader.e_lfanew, &ntheader, sizeof(ntheader)) != sizeof(ntheader)) {
        vsf_trace_error("fail to read ntheader" VSF_TRACE_CFG_LINEEND);
        return -1;
    }
    if ((ntheader.Signature != IMAGE_NT_SIGNATURE) || (ntheader.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)) {
        vsf_trace_error("invalid signature for pe 0x%04X" VSF_TRACE_CFG_LINEEND, ntheader.Signature);
        return -1;
    }

    peloader->ram_base = vsf_loader_malloc(peloader, VSF_LOADER_MEM_RWX, ntheader.OptionalHeader.SizeOfImage, 0);
    if (NULL == peloader->ram_base) {
        vsf_trace_error("fail to allocate memory to load target" VSF_TRACE_CFG_LINEEND);
        return -1;
    }
    char *ram_base = peloader->ram_base;

    if (vsf_loader_read(target, 0, ram_base, ntheader.OptionalHeader.SizeOfHeaders) != ntheader.OptionalHeader.SizeOfHeaders) {
        vsf_trace_error("fail to load headers" VSF_TRACE_CFG_LINEEND);
        goto free_and_fail;
    }
    IMAGE_NT_HEADERS *ntheader_ram = (IMAGE_NT_HEADERS *)(ram_base + dosheader.e_lfanew);
    ntheader_ram->OptionalHeader.ImageBase = (uintptr_t)ram_base;

    IMAGE_SECTION_HEADER *secheader = (IMAGE_SECTION_HEADER *)((char *)&ntheader_ram->OptionalHeader + ntheader_ram->FileHeader.SizeOfOptionalHeader);
    for (WORD i = 0; i < ntheader_ram->FileHeader.NumberOfSections; i++, secheader++) {
        if ((secheader->VirtualAddress + secheader->SizeOfRawData) > ntheader_ram->OptionalHeader.SizeOfImage) {
            vsf_trace_error("section %d overflows" VSF_TRACE_CFG_LINEEND, i);
            goto free_and_fail;
        }
        if (vsf_loader_read(target, secheader->PointerToRawData, ram_base + secheader->VirtualAddress, secheader->SizeOfRawData) != secheader->SizeOfRawData) {
            vsf_trace_error("fail to load section %d" VSF_TRACE_CFG_LINEEND, i);
            goto free_and_fail;
        }
    }

    IMAGE_DATA_DIRECTORY *data_dir = &ntheader_ram->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    DWORD vaddr = data_dir->VirtualAddress;
    if (vaddr != 0) {
        IMAGE_IMPORT_DESCRIPTOR *import_desc = (IMAGE_IMPORT_DESCRIPTOR *)(ram_base + vaddr);
        IMAGE_THUNK_DATA *thunk_data_out;
        IMAGE_THUNK_DATA *thunk_data_in;
        IMAGE_IMPORT_BY_NAME *import_name;

        while (import_desc->Name != 0) {
            vsf_peloader_debug("loading %s" VSF_TRACE_CFG_LINEEND, (char *)(ram_base + import_desc->Name));

            thunk_data_out = (IMAGE_THUNK_DATA *)(ram_base + import_desc->FirstThunk);
            thunk_data_in = import_desc->OriginalFirstThunk ?
                            (IMAGE_THUNK_DATA *)(ram_base + import_desc->OriginalFirstThunk)
                        :   (IMAGE_THUNK_DATA *)(ram_base + import_desc->FirstThunk);

            while (thunk_data_in->u1.AddressOfData != 0) {
                if (thunk_data_in->u1.Ordinal & IMAGE_ORDINAL_FLAG) {
                    vsf_trace_error("not supported" VSF_TRACE_CFG_LINEEND);
                    goto free_and_fail;
                } else {
                    import_name = (IMAGE_IMPORT_BY_NAME *)(ram_base + thunk_data_in->u1.AddressOfData);
                    vsf_peloader_debug("  reloate %s" VSF_TRACE_CFG_LINEEND, import_name->Name);
                    thunk_data_out->u1.Function = (uintptr_t)0;
                }
                thunk_data_out++;
                thunk_data_in++;
            }
            import_desc++;
        }
    }

    peloader->entry = (void *)((uintptr_t)peloader->ram_base + ntheader.OptionalHeader.AddressOfEntryPoint);
    return 0;

free_and_fail:
    vsf_loader_free(peloader, VSF_LOADER_MEM_RWX, peloader->ram_base);
    peloader->ram_base = NULL;
    return -1;
}

void vsf_peloader_cleanup(vsf_peloader_t *peloader)
{
    peloader->target = NULL;
    if (peloader->ram_base != NULL) {
        vsf_loader_free(peloader, VSF_LOADER_MEM_RWX, peloader->ram_base);
        peloader->ram_base = NULL;
    }
    if ((peloader->vplt_out != NULL) && (peloader->free_vplt != NULL)) {
        peloader->free_vplt(peloader->vplt_out);
    }
}

int vsf_peloader_call_init_array(vsf_peloader_t *peloader)
{
    return 0;
}

void vsf_peloader_call_fini_array(vsf_peloader_t *peloader)
{
}

#endif      // VSF_USE_LOADER && VSF_LOADER_USE_PE
