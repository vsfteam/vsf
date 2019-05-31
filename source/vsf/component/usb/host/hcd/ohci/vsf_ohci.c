/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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
#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_HCD_OHCI == ENABLED

#define VSF_USBH_IMPLEMENT_HCD
#include "vsf.h"

/*============================ MACROS ========================================*/

#define OHCI_ISO_DELAY          2

/*******************************************************
 * OHCI config
 *******************************************************/
/*
#ifndef VSF_OHCI_CFG_MAX_TD_NUM
#define VSF_OHCI_CFG_MAX_TD_NUM 0
#endif
*/
/*******************************************************
 * OHCI registers
 *******************************************************/

#define OHCI_NUM_CHAINS         32

/*
 * HcControl (control) register masks
 */
#define OHCI_CTRL_CBSR          (3 << 0)    /* control/bulk service ratio */
#define OHCI_CTRL_PLE           (1 << 2)    /* periodic list enable */
#define OHCI_CTRL_IE            (1 << 3)    /* isochronous enable */
#define OHCI_CTRL_CLE           (1 << 4)    /* control list enable */
#define OHCI_CTRL_BLE           (1 << 5)    /* bulk list enable */
#define OHCI_CTRL_HCFS          (3 << 6)    /* host controller functional state */
#define OHCI_CTRL_IR            (1 << 8)    /* interrupt routing */
#define OHCI_CTRL_RWC           (1 << 9)    /* remote wakeup connected */
#define OHCI_CTRL_RWE           (1 << 10)   /* remote wakeup enable */

/* For initializing controller (mask in an HCFS mode too) */
#define OHCI_CONTROL_INIT \
    (OHCI_CTRL_CBSR & 0x3) | OHCI_CTRL_IE | OHCI_CTRL_PLE | OHCI_CTRL_CLE

/* pre-shifted values for HCFS */
#define OHCI_USB_RESET          (0 << 6)
#define OHCI_USB_RESUME         (1 << 6)
#define OHCI_USB_OPER           (2 << 6)
#define OHCI_USB_SUSPEND        (3 << 6)

/*
 * HcCommandStatus (cmdstatus) register masks
 */
#define OHCI_HCR                (1 << 0)    /* host controller reset */
#define OHCI_CLF                (1 << 1)    /* control list filled */
#define OHCI_BLF                (1 << 2)    /* bulk list filled */
#define OHCI_OCR                (1 << 3)    /* ownership change request */
#define OHCI_SOC                (3 << 16)   /* scheduling overrun count */

/*
 * masks used with interrupt registers:
 * HcInterruptStatus (intrstatus)
 * HcInterruptEnable (intrenable)
 * HcInterruptDisable (intrdisable)
 */
#define OHCI_INTR_SO            (0x1ul << 0)    /* scheduling overrun */
#define OHCI_INTR_WDH           (0x1ul << 1)    /* writeback of done_head */
#define OHCI_INTR_SF            (0x1ul << 2)    /* start frame */
#define OHCI_INTR_RD            (0x1ul << 3)    /* resume detect */
#define OHCI_INTR_UE            (0x1ul << 4)    /* unrecoverable error */
#define OHCI_INTR_FNO           (0x1ul << 5)    /* frame number overflow */
#define OHCI_INTR_RHSC          (0x1ul << 6)    /* root hub status change */
#define OHCI_INTR_OC            (0x1ul << 30)   /* ownership change */
#define OHCI_INTR_MIE           (0x1ul << 31)   /* master interrupt enable */

// ED States
#define ED_IDLE                 0x00
#define ED_UNLINK               0x01
#define ED_OPER                 0x02
#define ED_DEL                  0x04
#define ED_URB_DEL              0x08

/* TD info field */
#define TD_CC                   0xf0000000
#define TD_CC_GET(td_p)         (((td_p) >> 28) & 0x0f)
#define TD_CC_SET(td_p, cc)     (td_p) = ((td_p) & 0x0fffffff) | (((cc) & 0x0f) << 28)
#define TD_EC                   0x0C000000
#define TD_T                    0x03000000
#define TD_T_DATA0              0x02000000
#define TD_T_DATA1              0x03000000
#define TD_T_TOGGLE             0x00000000
#define TD_R                    0x00040000
#define TD_DI                   0x00E00000
#define TD_DI_SET(X)            (((X) & 0x07)<< 21)
#define TD_DP                   0x00180000
#define TD_DP_SETUP             0x00000000
#define TD_DP_IN                0x00100000
#define TD_DP_OUT               0x00080000

#define TD_ISO                  0x00010000
#define TD_DEL                  0x00020000

/* CC Codes */
#define TD_CC_NOERROR           0x00
#define TD_CC_CRC               0x01
#define TD_CC_BITSTUFFING       0x02
#define TD_CC_DATATOGGLEM       0x03
#define TD_CC_STALL             0x04
#define TD_DEVNOTRESP           0x05
#define TD_PIDCHECKFAIL         0x06
#define TD_UNEXPECTEDPID        0x07
#define TD_DATAOVERRUN          0x08
#define TD_DATAUNDERRUN         0x09
#define TD_BUFFEROVERRUN        0x0C
#define TD_BUFFERUNDERRUN       0x0D
#define TD_NOTACCESSED          0x0F


/* Hub port features */
#define RH_PORT_CONNECTION      0x00
#define RH_PORT_ENABLE          0x01
#define RH_PORT_SUSPEND         0x02
#define RH_PORT_OVER_CURRENT    0x03
#define RH_PORT_RESET           0x04
#define RH_PORT_POWER           0x08
#define RH_PORT_LOW_SPEED       0x09

#define RH_C_PORT_CONNECTION    0x10
#define RH_C_PORT_ENABLE        0x11
#define RH_C_PORT_SUSPEND       0x12
#define RH_C_PORT_OVER_CURRENT  0x13
#define RH_C_PORT_RESET         0x14

/* Hub features */
#define RH_C_HUB_LOCAL_POWER    0x00
#define RH_C_HUB_OVER_CURRENT   0x01

#define RH_DEVICE_REMOTE_WAKEUP 0x00
#define RH_ENDPOINT_STALL       0x01

/* OHCI ROOT HUB REGISTER MASKS */

/* roothub.portstatus [i] bits */
#define RH_PS_CCS               0x00000001      /* current connect status */
#define RH_PS_PES               0x00000002      /* port enable status*/
#define RH_PS_PSS               0x00000004      /* port suspend status */
#define RH_PS_POCI              0x00000008      /* port over current indicator */
#define RH_PS_PRS               0x00000010      /* port reset status */
#define RH_PS_PPS               0x00000100      /* port power status */
#define RH_PS_LSDA              0x00000200      /* low speed device attached */
#define RH_PS_CSC               0x00010000      /* connect status change */
#define RH_PS_PESC              0x00020000      /* port enable status change */
#define RH_PS_PSSC              0x00040000      /* port suspend status change */
#define RH_PS_OCIC              0x00080000      /* over current indicator change */
#define RH_PS_PRSC              0x00100000      /* port reset status change */

/* roothub.status bits */
#define RH_HS_LPS               0x00000001      /* local power status */
#define RH_HS_OCI               0x00000002      /* over current indicator */
#define RH_HS_DRWE              0x00008000      /* device remote wakeup enable */
#define RH_HS_LPSC              0x00010000      /* local power status change */
#define RH_HS_OCIC              0x00020000      /* over current indicator change */
#define RH_HS_CRWE              0x80000000      /* clear remote wakeup enable */

/* roothub.b masks */
#define RH_B_DR                 0x0000ffff      /* device removable flags */
#define RH_B_PPCM               0xffff0000      /* port power control mask */

/* roothub.a masks */
#define RH_A_NDP                (0xfful << 0)   /* number of downstream ports */
#define RH_A_PSM                (0x1ul << 8)    /* power switching mode */
#define RH_A_NPS                (0x1ul << 9)    /* no power switching */
#define RH_A_DT                 (0x1ul << 10)   /* device type (mbz) */
#define RH_A_OCPM               (0x1ul << 11)   /* over current protection mode */
#define RH_A_NOCP               (0x1ul << 12)   /* no over current protection */
#define RH_A_POTPGT             (0xfful << 24)  /* power on to power good time */

/*============================ MACROFIED FUNCTIONS ===========================*/

#define usb_gettoggle(dev, ep, out) (((dev)->toggle[out] >> (ep)) & 1)
#define usb_dotoggle(dev, ep, out)  ((dev)->toggle[out] ^= (1 << (ep)))
#define usb_settoggle(dev, ep, out, bit) \
    ((dev)->toggle[out] = ((dev)->toggle[out] & ~(1 << (ep))) | ((bit) << (ep)))

#define CC_TO_ERROR(cc) (cc == 0 ? VSF_ERR_NONE : -cc)

/*============================ TYPES =========================================*/

/* The HCCA (Host Controller Communications Area) structure
 * Must be aligned to 256 bytes address
 */
struct ohci_hcca_t {
    uint32_t int_table[OHCI_NUM_CHAINS];  /* Interrupt ED table */
    uint16_t frame_no;                  /* current frame number */
    uint16_t pad1;                      /* set to 0 on each frame_no change */
    uint32_t done_head;                 /* onfo returned for an interrupt */
    //uint8_t reserved_for_hc[116];
};
typedef struct ohci_hcca_t ohci_hcca_t;

/* OHCI register defination */
struct ohci_regs_t {
    /* control and status registers */
    uint32_t revision;
    uint32_t control;
    uint32_t cmdstatus;
    uint32_t intrstatus;
    uint32_t intrenable;
    uint32_t intrdisable;
    /* memory pointers */
    uint32_t hcca;
    uint32_t ed_periodcurrent;
    uint32_t ed_controlhead;
    uint32_t ed_controlcurrent;
    uint32_t ed_bulkhead;
    uint32_t ed_bulkcurrent;
    uint32_t donehead;
    /* frame counters */
    uint32_t fminterval;
    uint32_t fmremaining;
    uint32_t fmnumber;
    uint32_t periodicstart;
    uint32_t lsthresh;
    /* Root hub ports */
    struct ohci_roothub_regs {
        uint32_t a;
        uint32_t b;
        uint32_t status;
        uint32_t portstatus[0];
    } roothub;
};
typedef struct ohci_regs_t ohci_regs_t;

/* usb OHCI ed, must be aligned to 16 bytes */
typedef struct ohci_td_t ohci_td_t;
struct ohci_ed_t {
    uint32_t hwINFO;
#define ED_DEQUEUE              (0x1ul << 27)
#define ED_ISO                  (0x1ul << 15)
#define ED_SKIP                 (0x1ul << 14)
#define ED_LOWSPEED             (0x1ul << 13)
#define ED_OUT                  (0x1ul << 11)
#define ED_IN                   (0x2ul << 11)
    uint32_t hwTailP;
    uint32_t hwHeadP;
#define ED_C                    0x02ul
#define ED_H                    0x01ul
    uint32_t hwNextED;

    uint8_t rm_frame :1;
    uint8_t busy : 1;
    uint8_t type : 2;
    uint8_t : 4;

    int8_t branch;

    uint16_t load : 10;
    uint16_t interval : 6;

    struct ohci_ed_t *ed_prev;
    vsf_slist_node_t node;
    ohci_td_t *td_dummy;
};
typedef struct ohci_ed_t ohci_ed_t;

/* usb OHCI td, must be aligned to 32 bytes */
typedef struct ohci_urb_t ohci_urb_t;
struct ohci_td_t {
    uint32_t hwINFO;
    uint32_t hwCBP;             /* Current Buffer Pointer */
    uint32_t hwNextTD;          /* Next TD Pointer */
    uint32_t hwBE;              /* Memory Buffer End Pointer */
#ifdef VSFHAL_HCD_ISO_EN
    uint32_t hwPSW[4];
#endif // VSFHAL_HCD_ISO_EN

    uint32_t index;
    vsf_slist_node_t node;
    ohci_urb_t *urb_ohci;
#ifdef VSFHAL_HCD_ISO_EN
    uint32_t dummy[4];
#else
    uint32_t dummy[1];
#endif // VSFHAL_HCD_ISO_EN
};

declare_vsf_pool(ohci_td_pool)
def_vsf_pool(ohci_td_pool, ohci_td_t)

enum ohci_hcd_state_t {
    OHCI_HCD_STATE_INIT,
    OHCI_HCD_STATE_DELAY,
    OHCI_HCD_STATE_READY,
    OHCI_HCD_STATE_DISABLED,
};
typedef enum ohci_hcd_state_t ohci_hcd_state_t;

/* Full ohci controller descriptor */
struct vsf_ohci_t {
    implement(ohci_hcca_t);

    vsf_eda_t eda;
    ohci_regs_t *regs;

    ohci_hcd_state_t state;
    uint32_t hc_control;            /* copy of the hc control reg */
    uint16_t load[OHCI_NUM_CHAINS]; /* load of the 32 Interrupt Chains (for load balancing)*/
    ohci_ed_t *ed_bulktail;
    ohci_ed_t *ed_controltail;
    ohci_ed_t *ed_isotail;

    vsf_slist_t ed_rm_list;
    vsf_slist_t td_dl_list;
};
typedef struct vsf_ohci_t vsf_ohci_t;

struct ohci_urb_t {
    ohci_ed_t *ed;
    uint16_t state;
#define URB_PRIV_INIT           0
#define URB_PRIV_EDLINK         (0x1 << 1)
#define URB_PRIV_EDSKIP         (0x1 << 2)
#define URB_PRIV_TDALLOC        (0x1 << 3)
#define URB_PRIV_TDLINK         (0x1 << 4)
#define URB_PRIV_WAIT_COMPLETE  (0x1 << 5)
#define URB_PRIV_WAIT_DELETE    (0x1 << 6)
    uint16_t length;            /* number of tds associated with this request */
    uint16_t td_num_served;     /* number of tds already serviced */
    uint16_t cur_idx;
    vsf_slist_t td_list;
};



/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static void ohci_free_urb_do(vsf_usbh_hcd_urb_t *urb);

/*============================ IMPLEMENTATION ================================*/

implement_vsf_pool(ohci_td_pool, ohci_td_t);

static NO_INIT vsf_pool(ohci_td_pool) __vsf_ohci_td_pool;

static ohci_td_t *ohci_td_alloc(void)
{
    ohci_td_t *td = VSF_POOL_ALLOC(ohci_td_pool, &__vsf_ohci_td_pool);
    if (td != NULL) {
        memset(td, 0, sizeof(*td));
    }
    return td;
}

static void ohci_td_free(ohci_td_t *td)
{
    VSF_POOL_FREE(ohci_td_pool, &__vsf_ohci_td_pool, td);
}

static int_fast8_t ohci_ed_balance(vsf_ohci_t *ohci, uint_fast8_t interval, uint_fast8_t load)
{
    int_fast8_t i, j, branch = -1;

    // iso periods can be huge; iso tds specify frame numbers
    if (interval > OHCI_NUM_CHAINS) {
        interval = OHCI_NUM_CHAINS;
    }

    // search for the least loaded schedule branch of that period
    // that has enough bandwidth left unreserved.
    for (i = 0; i < interval; i++) {
        if (branch < 0 || ohci->load[branch] < ohci->load[i]) {
            for (j = i; j < OHCI_NUM_CHAINS; j += interval) {
                // usb 1.1 says 90% of one frame
                if (ohci->load[j] + load > 900) {
                    break;
                }
            }
            if (j < OHCI_NUM_CHAINS) {
                continue;
            }
            branch = i;
        }
    }
    return branch;
}

static void ohci_link_periodic(vsf_ohci_t *ohci, ohci_ed_t *ed)
{
    ohci_ed_t *ed_here, **ed_prev;

    for (int_fast8_t i = ed->branch; i < OHCI_NUM_CHAINS; i += ed->interval) {
        ed_prev = (ohci_ed_t **)&ohci->int_table[i];
        ed_here = *ed_prev;

        // sorting each branch by period (slow before fast)
        while (ed_here && (ed != ed_here)) {
            if (ed->interval > ed_here->interval) {
                break;
            }
            ed_prev = (ohci_ed_t **)&ed_here->hwNextED;
            ed_here = *ed_prev;
        }
        if (ed != ed_here) {
            ed->hwNextED = (uint32_t)ed_here;
            *ed_prev = ed;
        }
        ohci->load[i] += ed->load;
    }
}

static void ohci_unlink_periodic(vsf_ohci_t *ohci, ohci_ed_t *ed)
{
    ohci_ed_t **ed_prev;
    for (int_fast8_t i = ed->branch; i < OHCI_NUM_CHAINS; i += ed->interval)
    {
        ed_prev = (ohci_ed_t **)&ohci->int_table[i];

        while (*ed_prev && (*ed_prev != ed)) {
            ed_prev = (ohci_ed_t **)&((*ed_prev)->hwNextED);
        }
        if (*ed_prev) {
            *ed_prev = (ohci_ed_t *)ed->hwNextED;
        }
        ohci->load[i] -= ed->load;
    }
}

// link an ed into one of the HC chains
static vsf_err_t ohci_ed_schedule(vsf_ohci_t *ohci, ohci_urb_t *urb_ohci)
{
    ohci_regs_t *regs = ohci->regs;
    ohci_ed_t *ed = urb_ohci->ed;
    ed->hwNextED = 0;

    switch (ed->type) {
    case USB_ENDPOINT_XFER_CONTROL:
        if (ohci->ed_controltail == NULL) {
            regs->ed_controlhead = (uint32_t)ed;
        } else {
            ohci->ed_controltail->hwNextED = (uint32_t)ed;
        }
        ed->ed_prev = ohci->ed_controltail;
        if (!ohci->ed_controltail && vsf_slist_is_empty(&ohci->ed_rm_list)) {
            ohci->hc_control |= OHCI_CTRL_CLE;
            regs->ed_controlcurrent = NULL;
            regs->control = ohci->hc_control;
        }
        ohci->ed_controltail = ed;
        break;
    case USB_ENDPOINT_XFER_BULK:
        if (ohci->ed_bulktail == NULL) {
            regs->ed_bulkhead = (uint32_t)ed;
        } else {
            ohci->ed_bulktail->hwNextED = (uint32_t)ed;
        }
        ed->ed_prev = ohci->ed_bulktail;
        if (!ohci->ed_bulktail && vsf_slist_is_empty(&ohci->ed_rm_list)) {
            ohci->hc_control |= OHCI_CTRL_BLE;
            regs->ed_bulkcurrent = NULL;
            regs->control = ohci->hc_control;
        }
        ohci->ed_bulktail = ed;
        break;
    case USB_ENDPOINT_XFER_INT:
    case USB_ENDPOINT_XFER_ISOC:
        if ((ed->interval == 0) || (ed->interval > 32))
            ed->interval = 32;
        ed->branch = ohci_ed_balance(ohci, ed->interval, ed->load);
        if (ed->branch < 0) {
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }
        ohci_link_periodic(ohci, ed);
        break;
    }
    urb_ohci->state |= URB_PRIV_EDLINK;
    return VSF_ERR_NONE;
}

static void ohci_ed_deschedule(vsf_ohci_t *ohci, ohci_urb_t *urb_ohci)
{
    ohci_regs_t *regs = ohci->regs;
    ohci_ed_t *ed = urb_ohci->ed;
    ed->hwINFO |= ED_SKIP;
    urb_ohci->state |= URB_PRIV_EDSKIP;

    switch (ed->type) {
    case USB_ENDPOINT_XFER_CONTROL:
        if (ed->ed_prev == NULL) {
            if (!ed->hwNextED) {
                ohci->hc_control &= ~OHCI_CTRL_CLE;
                regs->control = ohci->hc_control;
            } else {
                regs->ed_controlhead = ed->hwNextED;
            }
        } else {
            ed->ed_prev->hwNextED = ed->hwNextED;
        }
        if (ohci->ed_controltail == ed) {
            ohci->ed_controltail = ed->ed_prev;
        } else {
            ((ohci_ed_t *)(ed->hwNextED))->ed_prev = ed->ed_prev;
        }
        break;
    case USB_ENDPOINT_XFER_BULK:
        if (ed->ed_prev == NULL) {
            if (!ed->hwNextED) {
                ohci->hc_control &= ~OHCI_CTRL_BLE;
                regs->control = ohci->hc_control;
            } else {
                regs->ed_bulkhead = ed->hwNextED;
            }
        } else {
            ed->ed_prev->hwNextED = ed->hwNextED;
        }
        if (ohci->ed_bulktail == ed) {
            ohci->ed_bulktail = ed->ed_prev;
        } else {
            ((ohci_ed_t *)(ed->hwNextED))->ed_prev = ed->ed_prev;
        }
        break;
    case USB_ENDPOINT_XFER_INT:
    case USB_ENDPOINT_XFER_ISOC:
        ohci_unlink_periodic(ohci, ed);
        break;
    }
    urb_ohci->state &= ~URB_PRIV_EDLINK;
}

static vsf_err_t ohci_ed_init(ohci_urb_t *urb_ohci, vsf_usbh_hcd_urb_t *urb)
{
    vsf_usbh_eppipe_t pipe;
    ohci_ed_t *ed;
    ohci_td_t *td;

    ASSERT((urb_ohci != NULL) && (urb != NULL));

    /* dummy td; end of td list for ed */
    td = ohci_td_alloc();
    if (!td) {
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    td->urb_ohci = urb_ohci;

    ed = urb_ohci->ed;
    memset(ed, 0, sizeof(ohci_ed_t));
    ed->td_dummy = td;
    ed->hwTailP = (uint32_t)td;
    ed->hwHeadP = ed->hwTailP;

    pipe = urb->pipe;
    if (usb_gettoggle(urb->dev_hcd, pipe.endpoint, !pipe.dir_in1out0)) {
        ed->hwHeadP |= ED_C;
    }
    ed->type = pipe.type;
    ed->hwINFO = pipe.address
            | (pipe.endpoint << 7)
            | ((pipe.type == USB_ENDPOINT_XFER_ISOC) ? ED_ISO : 0)
            | ((pipe.speed == USB_SPEED_LOW) ? ED_LOWSPEED : 0)
            | (pipe.size << 16);

    if (ed->type != USB_ENDPOINT_XFER_CONTROL) {
        ed->hwINFO |= !pipe.dir_in1out0 ? ED_OUT : ED_IN;
        if (ed->type != USB_ENDPOINT_XFER_BULK) {
            uint_fast32_t interval = urb->interval;
            ed->interval = (pipe.type == USB_ENDPOINT_XFER_ISOC) ? interval : min(interval, 32);
            ed->load = 1;
        }
    }
    return VSF_ERR_NONE;
}

static void ohci_ed_fini(ohci_urb_t *urb_ohci)
{
    ohci_ed_t *ed = urb_ohci->ed;
    ohci_td_t *td, *td_next;

    vsf_slist_peek_next(ohci_td_t, node, &urb_ohci->td_list, td);
    // low efficient if call vsf_slist_for_each
    while (td != NULL) {
        vsf_slist_peek_next(ohci_td_t, node, &td->node, td_next);
        ohci_td_free(td);
        td = td_next;
    }
    vsf_slist_init(&urb_ohci->td_list);
    urb_ohci->length = 0;
    urb_ohci->state &= ~URB_PRIV_TDALLOC;

    ohci_td_free(ed->td_dummy);
    ed->td_dummy = NULL;
}

static ohci_td_t * ohci_td_fill(ohci_td_t *td, uint_fast32_t info, void *data,
        uint_fast16_t len, ohci_urb_t *urb_ohci)
{
    ohci_td_t *td_tmp;

    ASSERT(td != NULL);

    td_tmp = urb_ohci->ed->td_dummy;
    urb_ohci->ed->td_dummy = td;
    vsf_slist_set_next(ohci_td_t, node, &td_tmp->node, td);
    td_tmp->hwNextTD = (uint32_t)td;

    td_tmp->index = urb_ohci->cur_idx++;
    td_tmp->hwINFO = info;
    td_tmp->hwCBP = (uint32_t)((!data || !len) ? 0 : data);
    td_tmp->hwBE = (uint32_t)((!data || !len) ? 0 : (uint32_t)data + len - 1);

    urb_ohci->ed->hwTailP = td_tmp->hwNextTD;
    vsf_slist_peek_next(ohci_td_t, node, &td->node, td_tmp);
    return td_tmp;
}

#ifdef VSFHAL_HCD_ISO_EN
static ohci_td_t * ohci_td_fill_iso(ohci_td_t *td, uint_fast32_t info, void *data,
        uint_fast16_t len, uint_fast16_t index, ohci_urb_t *urb_ohci)
{
    ohci_td_t *td_tmp;
    uint_fast32_t bufferStart;
    vsf_usbh_hcd_urb_t *urb = container_of(urb_ohci, vsf_usbh_hcd_urb_t, priv);

    ASSERT(td != NULL);

    td_tmp = urb_ohci->ed->td_dummy;
    urb_ohci->ed->td_dummy = td;
    vsf_slist_set_next(ohci_td_t, node, &td_tmp->node, td);
    td_tmp->hwNextTD = (uint32_t)td;

    bufferStart = (uint32_t)data + urb->iso_packet.frame_desc[index].offset;
    len = urb->iso_packet.frame_desc[index].length;

    td_tmp->index = urb_ohci->cur_idx++;
    td_tmp->hwINFO = info;
    td_tmp->hwCBP = (uint32_t)((!bufferStart || !len) ? 0 : bufferStart) & 0xfffff000;
    td_tmp->hwBE = (uint32_t)((!bufferStart || !len) ? 0 : (uint32_t)bufferStart + len - 1);

    td_tmp->hwPSW[0] = ((uint32_t)data + urb->iso_packet.frame_desc[index].offset) & 0x0FFF | 0xE000;
    td_tmp->hwNextTD = 0;
    urb_ohci->ed->hwTailP = td_tmp->hwNextTD;
    vsf_slist_peek_next(ohci_td_t, node, &td->node, td_tmp);
    return td_tmp;
}
#endif // VSFHAL_HCD_ISO_EN

static void ohci_td_submit_urb(vsf_ohci_t *ohci, struct vsf_usbh_hcd_urb_t *urb)
{
    ohci_urb_t *urb_ohci;
    ohci_regs_t *regs;
    ohci_td_t *td;
    uint_fast8_t isout;
    uint_fast32_t data_len, info, n, m;
    void *data;

    ASSERT((ohci != NULL) && (urb != NULL));
    regs = ohci->regs;
    urb_ohci = (ohci_urb_t *)urb->priv;
    urb_ohci->td_num_served = 0;
    urb_ohci->cur_idx = 0;
    vsf_slist_peek_next(ohci_td_t, node, &urb_ohci->td_list, td);
    vsf_slist_init(&urb_ohci->td_list);

    isout = !urb->pipe.dir_in1out0;
    data = urb->buffer;
    data_len = urb->transfer_length;

    switch (urb->pipe.type) {
    case USB_ENDPOINT_XFER_CONTROL:
        info = TD_CC | TD_DP_SETUP | TD_T_DATA0;
        td = ohci_td_fill(td, info, (void *)&urb->setup_packet, 8, urb_ohci);
        info = isout ? TD_CC | TD_DP_OUT | TD_T_DATA1 :
                    TD_CC | TD_DP_IN | TD_T_DATA1;
        m = data_len;
        while (m) {
            n = min(m, 4096);
            m -= n;

            if (!m) {
                info |= TD_R;
            }
            td = ohci_td_fill(td, info, data, n, urb_ohci);
            data = (void *)((uint32_t)data + n);
            info &= ~TD_T;
        }
        info = (isout || data_len == 0) ? (TD_CC | TD_DP_IN | TD_T_DATA1) :
                    (TD_CC | TD_DP_OUT | TD_T_DATA1);
        td = ohci_td_fill(td, info, NULL, 0, urb_ohci);
        regs->cmdstatus = OHCI_CLF;
        break;
    case USB_ENDPOINT_XFER_INT:
    case USB_ENDPOINT_XFER_BULK:
        info = isout ? (TD_T_TOGGLE | TD_CC | TD_DP_OUT) :
                    (TD_T_TOGGLE | TD_CC | TD_DP_IN);
        while (data_len) {
            n = min(data_len, 4096);
            data_len -= n;

            if (!data_len && !(urb->transfer_flags & URB_SHORT_NOT_OK) && !isout) {
                info |= TD_R;
            }
            td = ohci_td_fill(td, info, data, n, urb_ohci);
            data = (void *)((uint32_t)data + n);
        }
        if ((urb->transfer_flags & URB_ZERO_PACKET) && (td != NULL)) {
            td = ohci_td_fill(td, info, 0, 0, urb_ohci);
        }
        if (urb->pipe.type == USB_ENDPOINT_XFER_BULK) {
            regs->cmdstatus = OHCI_BLF;
        }
        break;
#ifdef VSFHAL_HCD_ISO_EN
    case USB_ENDPOINT_XFER_ISOC:
        for (uint_fast32_t cnt = 0; cnt < urb->iso_packet.number_of_packets; cnt++) {
            td = ohci_td_fill_iso(td, TD_CC | TD_ISO | ((urb->iso_packet.start_frame + cnt) & 0xffff),
                    data, data_len, cnt, urb_ohci);
        }
        break;
#endif // VSFHAL_HCD_ISO_EN
    }
    urb_ohci->state |= URB_PRIV_TDLINK;
}

static void ohci_update_dl(vsf_ohci_t *ohci)
{
    ohci_td_t *td, *td_tmp, *td_next;
    ohci_urb_t *urb_ohci;
    ohci_ed_t *ed;
    uint_fast8_t cc;

    td_next = (ohci_td_t *)(ohci->done_head & 0xfffffff0);
    ohci->done_head = 0;

    while (td_next) {
        td = td_next;
        td_next = (ohci_td_t *)(td->hwNextTD & 0xfffffff0);

        td->hwINFO |= TD_DEL;
        cc = TD_CC_GET(td->hwINFO);

        // cc get error and ed halted
        if ((cc != TD_CC_NOERROR) && (td->urb_ohci->ed->hwHeadP & ED_H)) {
            urb_ohci = td->urb_ohci;
            ed = urb_ohci->ed;

            ed->hwINFO |= ED_SKIP;
            urb_ohci->state |= URB_PRIV_EDSKIP;
            ed->hwHeadP &= ~ED_H;

            urb_ohci->td_num_served += urb_ohci->length - 1 - td->index;

            // add all undone td to done list
            while (td != NULL) {
                vsf_slist_peek_next(ohci_td_t, node, &td->node, td_tmp);
                if (td != ed->td_dummy) {
                    vsf_slist_add_to_head(ohci_td_t, node, &ohci->td_dl_list, td);
                }
                td = td_tmp;
            }
            continue;
        }

        // add to done list
        vsf_slist_add_to_head(ohci_td_t, node, &ohci->td_dl_list, td);
    }
}

static void ohci_finish_unlinks(vsf_ohci_t *ohci)
{
    ohci_regs_t *regs = ohci->regs;
    ohci_urb_t *urb_ohci;
    vsf_usbh_hcd_urb_t *urb;
    uint_fast8_t frame = ohci->frame_no & 0x01;
    ohci_ed_t *ed, **ed_last, *ed_dellist = NULL;
    uint_fast32_t cmd = 0, ctrl = 0;
    uint_fast32_t deleted = 0;

    // TODO: no vsf_slist API to get &ohci->ed_rm_list.head
    for (ed_last = (ohci_ed_t **)&ohci->ed_rm_list.head, ed = *ed_last; ed != NULL; ed = *ed_last) {
        if (ed->rm_frame == frame) {
            urb_ohci = ed->td_dummy->urb_ohci;
            urb = container_of(urb_ohci, vsf_usbh_hcd_urb_t, priv);

            usb_settoggle(urb->dev_hcd, urb->pipe.endpoint,
                        !urb->pipe.dir_in1out0, (ed->hwHeadP & ED_C) >> 1);
            if (urb_ohci->state & URB_PRIV_WAIT_DELETE) {
                vsf_slist_peek_next(ohci_td_t, node, &ed->node, *ed_last);
                vsf_slist_init_node(ohci_ed_t, node, ed);
                ed_dellist = ed;
                deleted++;
            } else if (urb_ohci->state & URB_PRIV_WAIT_COMPLETE) {
                vsf_slist_peek_next(ohci_td_t, node, &ed->node, *ed_last);
                ohci_ed_fini(urb_ohci);
                urb_ohci->state &= ~(URB_PRIV_EDSKIP | URB_PRIV_WAIT_COMPLETE);

                vsf_eda_post_msg(urb->eda_caller, urb);
            }
        } else {
            // TODO: no vsf_slist API to get &ed->node.next
            ed_last = (ohci_ed_t **)&ed->node.next;
        }
    }

    if (deleted) {
        vsf_eda_post_msg(&ohci->eda, ed_dellist);
    }

    if (vsf_slist_is_empty(&ohci->ed_rm_list)) {
        if (ohci->ed_controltail) {
            cmd |= OHCI_CLF;
            if (!(ohci->hc_control & OHCI_CTRL_CLE)) {
                ctrl |= OHCI_CTRL_CLE;
                regs->ed_controlcurrent = 0;
            }
        }
        if (ohci->ed_bulktail) {
            cmd |= OHCI_BLF;
            if (!(ohci->hc_control & OHCI_CTRL_BLE)) {
                ctrl |= OHCI_CTRL_BLE;
                regs->ed_bulkcurrent = 0;
            }
        }
        if (ctrl) {
            ohci->hc_control |= ctrl;
            regs->control = ohci->hc_control;
        }
        if (cmd) {
            regs->cmdstatus = cmd;
        }
    }
}

static vsf_err_t ohci_td_done(vsf_usbh_hcd_urb_t *urb, ohci_td_t *td)
{
    ohci_urb_t *urb_ohci = (ohci_urb_t *)urb->priv;
    int_fast32_t cc = 0;
    vsf_err_t err = VSF_ERR_NONE;

#ifdef VSFHAL_HCD_ISO_EN
    if (td->hwINFO & TD_ISO) {
        uint_fast16_t tdPSW = td->hwPSW[0] & 0xffff;
        uint_fast32_t dlen;

        if (td->hwINFO & TD_CC) {
            return VSF_ERR_FAIL;
        }

        cc = (tdPSW >> 12) & 0xf;
        if (!urb->pipe.dir_in1out0) {
            dlen = urb->iso_packet.frame_desc[td->index].length;
        } else {
            if (cc == TD_DATAUNDERRUN) {
                cc = TD_CC_NOERROR;
            }
            dlen = tdPSW & 0x3ff;
        }
        urb->actual_length += dlen;
        urb->iso_packet.frame_desc[td->index].actual_length = dlen;
        urb->iso_packet.frame_desc[td->index].status = CC_TO_ERROR(cc);
    } else
#endif // VSFHAL_HCD_ISO_EN
    {
        cc = TD_CC_GET(td->hwINFO);
        if ((cc == TD_DATAUNDERRUN) && !(urb->transfer_flags & URB_SHORT_NOT_OK)) {
            cc = TD_CC_NOERROR;
        }
        // clear toggle carry if stalled, endpoint will be reset later
        if (cc == TD_CC_STALL) {
            urb_ohci->ed->hwHeadP &= ~ED_C;
        }
        if ((cc != TD_CC_NOERROR) && (cc < 0x0e)) {
            err = CC_TO_ERROR(cc);
        }

        if (((urb->pipe.type != USB_ENDPOINT_XFER_CONTROL) || (td->index != 0)) && (td->hwBE != 0)) {
            if (td->hwCBP == 0) {
                urb->actual_length = td->hwBE - (uint32_t)urb->buffer + 1;
            } else {
                urb->actual_length = td->hwCBP - (uint32_t)urb->buffer;
            }
        }
    }

    // add finished td to td_list
    vsf_slist_add_to_head(ohci_td_t, node, &urb_ohci->td_list, td);
    return err;
}

static void ohci_ed_start_unlink(vsf_ohci_t *ohci, ohci_urb_t *urb_ohci)
{
    ohci_regs_t *regs = ohci->regs;
    ohci_ed_t *ed = urb_ohci->ed;

    if (ed->hwINFO & ED_DEQUEUE) {
        return;
    }

    ed->hwINFO |= ED_DEQUEUE;
    ohci_ed_deschedule(ohci, urb_ohci);

    vsf_slist_add_to_head(ohci_ed_t, node, &ohci->ed_rm_list, ed);
    ed->ed_prev = NULL;

    regs->intrstatus = OHCI_INTR_SF;
    regs->intrenable = OHCI_INTR_SF;

    ed->rm_frame = (ohci->frame_no + 1) & 0x1;
}

static void ohci_td_takeback(vsf_ohci_t *ohci, ohci_td_t *td)
{
    vsf_err_t err;
    ohci_urb_t *urb_ohci = td->urb_ohci;
    vsf_usbh_hcd_urb_t *urb = container_of(urb_ohci, vsf_usbh_hcd_urb_t, priv);

    err = ohci_td_done(urb, td);
    urb_ohci->td_num_served++;

    if (urb_ohci->td_num_served >= urb_ohci->length) {
        urb_ohci->state &= ~URB_PRIV_TDLINK;
        urb->status = err;

        if (urb_ohci->state & URB_PRIV_WAIT_DELETE) {
            ohci_ed_start_unlink(ohci, urb_ohci);
        } else if ((urb->pipe.type == USB_ENDPOINT_XFER_BULK)
                ||  (urb->pipe.type == USB_ENDPOINT_XFER_CONTROL)) {
            // pend complete event after ed unlink
            urb_ohci->state |= URB_PRIV_WAIT_COMPLETE;
            ohci_ed_start_unlink(ohci, urb_ohci);
        } else {
            vsf_eda_post_msg(urb->eda_caller, urb);
        }
    }
}

static void ohci_work(vsf_ohci_t *ohci)
{
    ohci_td_t *td;

    while (!vsf_slist_is_empty(&ohci->td_dl_list)) {
        vsf_slist_remove_from_head_unsafe(ohci_td_t, node, &ohci->td_dl_list, td);
        ohci_td_takeback(ohci, td);
    }

    if (!vsf_slist_is_empty(&ohci->ed_rm_list)) {
        ohci_finish_unlinks(ohci);
    }
}

static void ohci_interrupt(void *param)
{
    vsf_ohci_t *ohci = (vsf_ohci_t *)param;
    ohci_regs_t *regs = ohci->regs;
    uint_fast32_t intrstatus = regs->intrstatus & regs->intrenable;

    if (intrstatus & OHCI_INTR_UE) {
        ohci->state = OHCI_HCD_STATE_DISABLED;
        regs->intrdisable = OHCI_INTR_MIE;
        regs->control = 0;
        regs->cmdstatus = OHCI_HCR;
        ohci->hc_control = OHCI_USB_RESET;
        return;
    }

    if (intrstatus & OHCI_INTR_RHSC) {
        regs->intrstatus = OHCI_INTR_RHSC | OHCI_INTR_RD;
        regs->intrdisable = OHCI_INTR_RHSC;
    } else if (intrstatus & OHCI_INTR_RD) {
        regs->intrstatus = OHCI_INTR_RD;
        regs->intrdisable = OHCI_INTR_RD;
    }

    if (intrstatus & OHCI_INTR_WDH) {
        ohci_update_dl(ohci);
    }

    ohci_work(ohci);

    if ((intrstatus & OHCI_INTR_SF) && vsf_slist_is_empty(&ohci->ed_rm_list)) {
        regs->intrdisable = OHCI_INTR_SF;
    }

    regs->intrstatus = intrstatus;
    regs->intrenable = OHCI_INTR_MIE;
}

static vsf_ohci_t *ohci_alloc_resource(void)
{
    uint_fast16_t ohci_size = (sizeof(vsf_ohci_t) + 31) & ~31;
    vsf_ohci_t *ohci = VSF_USBH_MALLOC_ALIGNED(ohci_size, 256);
    if (ohci == NULL) { return NULL; }
    memset(ohci, 0, sizeof(vsf_ohci_t));
    return ohci;
}

// return ms for PowerOn to PowerGood
static uint_fast32_t ohci_start(vsf_ohci_t *ohci)
{
    ohci_regs_t *regs = ohci->regs;

    regs->ed_controlhead = 0;
    regs->ed_bulkhead = 0;
    regs->hcca = (uint32_t)ohci;
    ohci->hc_control = OHCI_CONTROL_INIT | OHCI_USB_OPER;

    regs->control = ohci->hc_control;
    regs->fminterval = 0x2edf | (((0x2edf - 210) * 6 / 7) << 16);
    regs->periodicstart = (0x2edf * 9) / 10;
    regs->lsthresh = 0x628;
    regs->intrstatus = regs->intrenable =
            OHCI_INTR_MIE | OHCI_INTR_UE | OHCI_INTR_WDH | OHCI_INTR_SO;

    regs->roothub.a = regs->roothub.a & ~(RH_A_PSM | RH_A_OCPM);
    regs->roothub.status = RH_HS_LPSC;
    return (regs->roothub.a >> 23) & 0x1fe;
}

static void ohci_ed_free_evthanlder(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
        case VSF_EVT_MESSAGE:
        {
            ohci_ed_t *ed, *ed_dellist = vsf_eda_get_cur_msg();
            vsf_usbh_hcd_urb_t *urb;
            ohci_urb_t *urb_ohci;

            while (ed_dellist != NULL) {
                ed = ed_dellist;
                vsf_slist_peek_next(ohci_ed_t, node, &ed->node, ed_dellist);

                urb_ohci = ed->td_dummy->urb_ohci;
                urb = container_of(urb_ohci, vsf_usbh_hcd_urb_t, priv);
                vsf_usbh_hcd_urb_free_buffer(urb);
                ohci_ed_fini(urb_ohci);
                ohci_free_urb_do(urb);
            }
        }
        break;
    }
}

static vsf_err_t ohci_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vsf_usbh_hcd_t *hcd)
{
    vsf_ohci_t *ohci;
    vsf_ohci_param_t *ohci_param;
    ohci_regs_t *regs;

    ASSERT(hcd != NULL);
    ohci = hcd->priv;
    regs = ohci->regs;
    ohci_param = hcd->param;

    switch (evt) {
    case VSF_EVT_INIT:
        hcd->rh_speed = USB_SPEED_FULL;
        ohci = hcd->priv = ohci_alloc_resource();
        if (!ohci) {
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }

        do {
            usb_hc_cfg_t cfg = {
                .priority       = ohci_param->priority,
                .irq_handler    = ohci_interrupt,
                .param          = ohci,
            };
            vsf_usb_hc_init(ohci_param->hc, &cfg);
        } while (0);
        regs = ohci->regs = (ohci_regs_t *)vsf_usb_hc_get_regbase(ohci_param->hc);

        regs->intrdisable = OHCI_INTR_MIE;
        regs->control = 0;
        regs->cmdstatus = OHCI_HCR;
        ohci->state = OHCI_HCD_STATE_INIT;
    case VSF_EVT_TIMER:
        switch (ohci->state) {
        case OHCI_HCD_STATE_INIT:
            if (regs->cmdstatus & OHCI_HCR) {
                vsf_teda_set_timer_ms(1);
            } else {
                ohci->hc_control = OHCI_USB_RESET;
                vsf_teda_set_timer_ms(100);
                ohci->state++;
            }
            break;
        case OHCI_HCD_STATE_DELAY:
            vsf_teda_set_timer_ms(ohci_start(ohci));
            ohci->state++;
            break;
        case OHCI_HCD_STATE_READY:
            ohci->eda.evthandler = ohci_ed_free_evthanlder;
            vsf_eda_init(&ohci->eda, vsf_priority_inherit, false);
            return VSF_ERR_NONE;
        }
    }
    return VSF_ERR_NOT_READY;
}

static vsf_err_t ohci_fini(vsf_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t ohci_suspend(vsf_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t ohci_resume(vsf_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_usbh_hcd_urb_t *ohci_alloc_urb(vsf_usbh_hcd_t *hcd)
{
    uint_fast32_t size;
    vsf_usbh_hcd_urb_t *urb = NULL;
    ohci_urb_t *urb_ohci;
    ohci_ed_t *ed;

    ASSERT(hcd != NULL);

    size = sizeof(ohci_ed_t) + sizeof(vsf_usbh_hcd_urb_t) + sizeof(ohci_urb_t);
    ed = VSF_USBH_MALLOC_ALIGNED(size, 16);
    if (ed) {
        memset(ed, 0, size);
        urb = (vsf_usbh_hcd_urb_t *)(ed + 1);
        urb_ohci = (ohci_urb_t *)urb->priv;
        urb_ohci->ed = ed;
    }
    return urb;
}

static void ohci_free_urb_do(vsf_usbh_hcd_urb_t *urb)
{
    ohci_urb_t *urb_ohci = (ohci_urb_t *)urb->priv;
    VSF_USBH_FREE(urb_ohci->ed);
}

static void ohci_free_urb(vsf_usbh_hcd_t *hcd, vsf_usbh_hcd_urb_t *urb)
{
    vsf_ohci_t *ohci;
    ohci_urb_t *urb_ohci;

    ASSERT((hcd != NULL) && (urb != NULL));
    ohci = (vsf_ohci_t *)hcd->priv;
    urb_ohci = (ohci_urb_t *)urb->priv;
    ASSERT((ohci != NULL) && (urb_ohci != NULL));

    if (urb_ohci->state) {
        urb_ohci->state &= ~URB_PRIV_WAIT_COMPLETE;
        urb_ohci->state |= URB_PRIV_WAIT_DELETE;

        if ((urb_ohci->state & (URB_PRIV_EDLINK | URB_PRIV_TDALLOC)) ==
                (URB_PRIV_EDLINK | URB_PRIV_TDALLOC)) {
            ohci_ed_start_unlink(ohci, urb_ohci);
        }
    } else {
        vsf_usbh_hcd_urb_free_buffer(urb);
        ohci_free_urb_do(urb);
    }
}

static vsf_err_t ohci_submit_urb(vsf_usbh_hcd_t *hcd, vsf_usbh_hcd_urb_t *urb)
{
    uint_fast32_t size = 0, datablock;
    vsf_ohci_t *ohci;
    ohci_urb_t *urb_ohci;
    ohci_ed_t *ed;
    ohci_td_t *td;

    ASSERT((hcd != NULL) && (urb != NULL));
    ohci = (vsf_ohci_t *)hcd->priv;
    urb_ohci = (ohci_urb_t *)urb->priv;
    ASSERT((ohci != NULL) && (urb_ohci != NULL));

    if (ohci->state != OHCI_HCD_STATE_READY) {
        return VSF_ERR_FAIL;
    }

    ohci_ed_init(urb_ohci, urb);
    datablock = (urb->transfer_length + 4095) / 4096;
    switch (urb->pipe.type) {
    case USB_ENDPOINT_XFER_CONTROL:
        size = 2;
    case USB_ENDPOINT_XFER_BULK:
    case USB_ENDPOINT_XFER_INT:
        size += datablock;
        if (!size || ((urb->transfer_flags & URB_ZERO_PACKET) &&
                      !(urb->transfer_length % urb->pipe.size))) {
            size++;
        }
        break;
#ifdef VSFHAL_HCD_ISO_EN
    case USB_ENDPOINT_XFER_ISOC:
        size = urb->iso_packet.number_of_packets;
        if (!size || (size > VSFHAL_HCD_ISO_PACKET_LIMIT)) {
            return VSF_ERR_FAIL;
        }
        for (uint_fast32_t i = 0; i < size; i++) {
            urb->iso_packet.frame_desc[i].actual_length = 0;
            urb->iso_packet.frame_desc[i].status = URB_PENDING;
        }
        break;
#endif // VSFHAL_HCD_ISO_EN
    }

    ed = urb_ohci->ed;
    memset(urb_ohci, 0, sizeof(ohci_urb_t));
    urb_ohci->ed = ed;
    urb_ohci->length = size;

    for (uint_fast32_t i = 0; i < size; i++) {
        td = ohci_td_alloc();
        if (NULL == td) {
            ohci_ed_fini(urb_ohci);
            return VSF_ERR_FAIL;
        } else {
            td->urb_ohci = urb_ohci;
            vsf_slist_add_to_head(ohci_td_t, node, &urb_ohci->td_list, td);
        }
    }
    urb_ohci->state |= URB_PRIV_TDALLOC;

#ifdef VSFHAL_HCD_ISO_EN
    if (urb->pipe.type == USB_ENDPOINT_XFER_ISOC) {
        urb->iso_packet.start_frame += (ohci->frame_no + OHCI_ISO_DELAY) & 0xffff;
    }
#endif // VSFHAL_HCD_ISO_EN

    urb->actual_length = 0;
    urb->status = URB_PENDING;

    ohci_ed_schedule(ohci, urb_ohci);
    ohci_td_submit_urb(ohci, urb);
    return VSF_ERR_NONE;
}

static vsf_err_t ohci_relink_urb(vsf_usbh_hcd_t *hcd, vsf_usbh_hcd_urb_t *urb)
{
    vsf_ohci_t *ohci;
    ohci_urb_t *urb_ohci;

    ASSERT((hcd != NULL) && (urb != NULL));
    ohci = (vsf_ohci_t *)hcd->priv;
    ASSERT(ohci != NULL);
    urb_ohci = (ohci_urb_t *)urb->priv;
    ASSERT(urb_ohci != NULL);

    if (urb_ohci->state != (URB_PRIV_EDLINK | URB_PRIV_TDALLOC)) {
        return VSF_ERR_FAIL;
    }

    switch (urb->pipe.type) {
#ifdef VSFHAL_HCD_ISO_EN
    case USB_ENDPOINT_XFER_ISOC:
        // NOTE: iso transfer interval fixed to 1
        urb->iso_packet.start_frame = (ohci->hcca->frame_no + 1) & 0xffff;
        for (int i = 0; i < urb->iso_packet.number_of_packets; i++)
            urb->iso_packet.frame_desc[i].actual_length = 0;
#endif
    case USB_ENDPOINT_XFER_INT:
        break;
    default:
        ASSERT(false);
    }

    urb->actual_length = 0;
    urb->status = URB_PENDING;
    ohci_td_submit_urb(ohci, urb);
    return VSF_ERR_NONE;
}

static int ohci_rh_control(vsf_usbh_hcd_t *hcd, vsf_usbh_hcd_urb_t *urb)
{
    uint_fast16_t typeReq, wValue, wIndex, wLength;
    vsf_ohci_t *ohci;
    ohci_regs_t *regs;
    struct usb_ctrlrequest_t *req = &urb->setup_packet;
    uint_fast32_t temp;
    uint32_t datadw[4];
    uint8_t *data = (uint8_t*)datadw;
    uint_fast8_t len = 0;

    ASSERT((hcd != NULL) && (urb != NULL));
    ohci = (vsf_ohci_t *)hcd->priv;
    ASSERT(ohci != NULL);
    regs = ohci->regs;

    typeReq = (req->bRequestType << 8) | req->bRequest;
    wValue = req->wValue;
    wIndex = req->wIndex;
    wLength = req->wLength;

    switch (typeReq) {
    case GetHubStatus:
        datadw[0] = regs->roothub.status;
        len = 4;
        break;
    case GetPortStatus:
        datadw[0] = regs->roothub.portstatus[wIndex - 1];
        len = 4;
        break;
    case SetPortFeature:
        switch (wValue) {
        case RH_PORT_SUSPEND:
            regs->roothub.portstatus[wIndex - 1] = RH_PS_PSS;
            break;
        case RH_PORT_RESET:    /* BUG IN HUP CODE *********/
            if (regs->roothub.portstatus[wIndex - 1] & RH_PS_CCS) {
                regs->roothub.portstatus[wIndex - 1] = RH_PS_PRS;
            }
            break;
        case RH_PORT_POWER:
            regs->roothub.portstatus[wIndex - 1] = RH_PS_PPS;
            break;
        case RH_PORT_ENABLE:    /* BUG IN HUP CODE *********/
            if (regs->roothub.portstatus[wIndex - 1] & RH_PS_CCS) {
                regs->roothub.portstatus[wIndex - 1] = RH_PS_PES;
            }
            break;
        default:
            goto error;
        }
        break;
    case ClearPortFeature:
        switch (wValue) {
        case RH_PORT_ENABLE:
            regs->roothub.portstatus[wIndex - 1] = RH_PS_CCS;
            break;
        case RH_PORT_SUSPEND:
            regs->roothub.portstatus[wIndex - 1] = RH_PS_POCI;
            break;
        case RH_PORT_POWER:
            regs->roothub.portstatus[wIndex - 1] = RH_PS_LSDA;
            break;
        case RH_C_PORT_CONNECTION:
            regs->roothub.portstatus[wIndex - 1] = RH_PS_CSC;
            break;
        case RH_C_PORT_ENABLE:
            regs->roothub.portstatus[wIndex - 1] = RH_PS_PESC;
            break;
        case RH_C_PORT_SUSPEND:
            regs->roothub.portstatus[wIndex - 1] = RH_PS_PSSC;
            break;
        case RH_C_PORT_OVER_CURRENT:
            regs->roothub.portstatus[wIndex - 1] = RH_PS_OCIC;
            break;
        case RH_C_PORT_RESET:
            regs->roothub.portstatus[wIndex - 1] = RH_PS_PRSC;
            break;
        default:
            goto error;
        }
        break;
    case GetHubDescriptor:
        temp = regs->roothub.a;
        data[0] = 9;                    // min length;
        data[1] = 0x29;
        data[2] = temp & RH_A_NDP;
        data[3] = 0;
        if (temp & RH_A_PSM) {          /* per-port power switching? */
            data[3] |= 0x1;
        }
        if (temp & RH_A_NOCP) {         /* no over current reporting? */
            data[3] |= 0x10;
        } else if (temp & RH_A_OCPM) {  /* per-port over current reporting? */
            data[3] |= 0x8;
        }
        datadw[1] = 0;
        data[5] = (temp & RH_A_POTPGT) >> 24;
        temp = regs->roothub.b;
        data[7] = temp & RH_B_DR;
        if (data[2] < 7) {
            data[8] = 0xff;
        } else {
            data[0] += 2;
            data[8] = (temp & RH_B_DR) >> 8;
            data[10] = data[9] = 0xff;
        }
        len = min(data[0], wLength);
        break;
    default:
        goto error;
    }
    if (len) {
        if (urb->transfer_length < len) {
            len = urb->transfer_length;
        }
        urb->actual_length = len;
        memcpy(urb->buffer, data, len);
    }
    return len;

error:
    urb->status = URB_FAIL;
    return -1;
}

void vsf_ohci_init(void)
{
    /*
    vsf_pool_cfg_t cfg = {
        NULL,
        (code_region_t *)&DEFAULT_CODE_REGION_NONE,
    };
    ohci_td_pool_pool_init(&__vsf_ohci_td_pool, &cfg);
    */


#if defined(VSF_OHCI_CFG_MAX_TD_NUM)
    VSF_POOL_INIT_EX(ohci_td_pool, &__vsf_ohci_td_pool, VSF_OHCI_CFG_MAX_TD_NUM, 32,
        .pTarget = NULL,
        .ptRegion = (code_region_t *)&DEFAULT_CODE_REGION_NONE, 
    );
#else
    VSF_POOL_PREPARE_EX(ohci_td_pool, &__vsf_ohci_td_pool, 32,
        .pTarget = NULL,
        .ptRegion = (code_region_t *)&DEFAULT_CODE_REGION_NONE,
    );
/*
    void *td_buffer = VSF_USBH_MALLOC_ALIGNED(
            sizeof(ohci_td_t) * VSF_OHCI_CFG_MAX_TD_NUM, 32);
    VSF_POOL_ADD_BUFFER(    ohci_td_pool,
                            &__vsf_ohci_td_pool,
                            td_buffer,
                            VSF_OHCI_CFG_MAX_TD_NUM * sizeof(ohci_td_t));
*/
#endif
}

const vsf_usbh_hcd_drv_t vsf_ohci_drv = {
    .init_evthandler = ohci_init_evthandler,
    .fini = ohci_fini,
    .suspend = ohci_suspend,
    .resume = ohci_resume,
    .alloc_urb = ohci_alloc_urb,
    .free_urb = ohci_free_urb,
    .submit_urb = ohci_submit_urb,
    .relink_urb = ohci_relink_urb,
    .rh_control = ohci_rh_control,
};

#endif