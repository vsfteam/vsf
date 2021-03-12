#ifndef __DWCOTG_REGS_H__
#define __DWCOTG_REGS_H__

/**
 * @file
 *
 * This file contains the data structures for accessing the DWC_otg core registers.
 *
 * The application interfaces with the HS OTG core by reading from and
 * writing to the Control and Status Register (CSR) space through the
 * AHB Slave interface. These registers are 32 bits wide, and the
 * addresses are 32-bit-block aligned.
 * CSRs are classified as follows:
 * - Core Global Registers
 * - Device Mode Registers
 * - Device Global Registers
 * - Device Endpoint Specific Registers
 * - Host Mode Registers
 * - Host Global Registers
 * - Host Port CSRs
 * - Host Channel Specific Registers
 *
 * Only the Core Global registers can be accessed in both Device and
 * Host modes. When the HS OTG core is operating in one mode, either
 * Device or Host, the application must not access registers from the
 * other mode. When the core switches from one mode to another, the
 * registers in the new mode of operation must be reprogrammed as they
 * would be after a power-on reset.
 */

/** Maximum number of Periodic FIFOs */
#define MAX_PERIO_FIFOS 15
/** Maximum number of Periodic FIFOs */
#define MAX_TX_FIFOS 15
/** Maximum number of Endpoints/HostChannels */
#define MAX_EPS_CHANNELS 16

/****************************************************************************/
/** DWC_otg Core registers .
 * The dwcotg_core_global_regs structure defines the size
 * and relative field offsets for the Core Global registers.
 */
struct dwcotg_core_global_regs_t {
	/** OTG Control and Status Register.  <i>Offset: 000h</i> */
	volatile uint32_t gotgctl;
	/** OTG Interrupt Register.	 <i>Offset: 004h</i> */
	volatile uint32_t gotgint;
	/**Core AHB Configuration Register.	 <i>Offset: 008h</i> */
	volatile uint32_t gahbcfg;
	/**Core USB Configuration Register.	 <i>Offset: 00Ch</i> */
	volatile uint32_t gusbcfg;
	/**Core Reset Register.	 <i>Offset: 010h</i> */
	volatile uint32_t grstctl;
	/**Core Interrupt Register.	 <i>Offset: 014h</i> */
	volatile uint32_t gintsts;
	/**Core Interrupt Mask Register.  <i>Offset: 018h</i> */
	volatile uint32_t gintmsk;
	/**Receive Status Queue Read Register (Read Only).	<i>Offset: 01Ch</i> */
	volatile uint32_t grxstsr;
	/**Receive Status Queue Read & POP Register (Read Only).  <i>Offset: 020h</i>*/
	volatile uint32_t grxstsp;
	/**Receive FIFO Size Register.	<i>Offset: 024h</i> */
	volatile uint32_t grxfsiz;
	/**Non Periodic Transmit FIFO Size Register.  <i>Offset: 028h</i> */
	volatile uint32_t gnptxfsiz;
	/**Non Periodic Transmit FIFO/Queue Status Register (Read
	 * Only). <i>Offset: 02Ch</i> */
	volatile uint32_t gnptxsts;
	/**I2C Access Register.	 <i>Offset: 030h</i> */
	volatile uint32_t gi2cctl;
	/**PHY Vendor Control Register.	 <i>Offset: 034h</i> */
	volatile uint32_t gpvndctl;
	/**General Purpose Input/Output Register.  <i>Offset: 038h</i> */
	volatile uint32_t gccfg;
	/**User ID Register.  <i>Offset: 03Ch</i> */
	volatile uint32_t guid;
	/**Synopsys ID Register (Read Only).  <i>Offset: 040h</i> */
	volatile uint32_t gsnpsid;
	/**User HW Config1 Register (Read Only).  <i>Offset: 044h</i> */
	volatile uint32_t ghwcfg1;
	/**User HW Config2 Register (Read Only).  <i>Offset: 048h</i> */
	volatile uint32_t ghwcfg2;
	/**User HW Config3 Register (Read Only).  <i>Offset: 04Ch</i> */
	volatile uint32_t ghwcfg3;
	/**User HW Config4 Register (Read Only).  <i>Offset: 050h</i>*/
	volatile uint32_t ghwcfg4;
	/** Core LPM Configuration register <i>Offset: 054h</i>*/
	volatile uint32_t glpmcfg;
	/** Global PowerDn Register <i>Offset: 058h</i> */
	volatile uint32_t gpwrdn;
	/** Global DFIFO SW Config Register  <i>Offset: 05Ch</i> */
	volatile uint32_t gdfifocfg;
	/** ADP Control Register  <i>Offset: 060h</i> */
	volatile uint32_t adpctl;
	/** Reserved  <i>Offset: 064h-0FFh</i> */
	volatile uint32_t reserved39[39];
	/** Host Periodic Transmit FIFO Size Register. <i>Offset: 100h</i> */
	volatile uint32_t hptxfsiz;
	/** Device Periodic Transmit FIFO#n Register if dedicated fifos are disabled,
		otherwise Device Transmit FIFO#n Register.
	 * <i>Offset: 104h + (FIFO_Number-1)*04h, 1 <= FIFO Number <= 15 (1<=n<=15).</i> */
	volatile uint32_t dtxfsiz[15];
};

/********************  Bit definition for USB_OTG_GOTGCTL register  ********************/
#define USB_OTG_GOTGCTL_SRQSCS                  ((uint32_t)0x00000001)         /*!< Session request success */
#define USB_OTG_GOTGCTL_SRQ                     ((uint32_t)0x00000002)         /*!< Session request */
#define USB_OTG_GOTGCTL_VBVALOEN                ((uint32_t)0x00000004)         /*!< VBUS valid override enable */
#define USB_OTG_GOTGCTL_VBVALOVAL               ((uint32_t)0x00000008)         /*!< VBUS valid override value */
#define USB_OTG_GOTGCTL_AVALOEN                 ((uint32_t)0x00000010)         /*!< A-peripheral session valid override enable */
#define USB_OTG_GOTGCTL_AVALOVAL                ((uint32_t)0x00000020)         /*!< A-peripheral session valid override value */
#define USB_OTG_GOTGCTL_BVALOEN                 ((uint32_t)0x00000040)         /*!< B-peripheral session valid override enable */
#define USB_OTG_GOTGCTL_BVALOVAL                ((uint32_t)0x00000080)         /*!< B-peripheral session valid override value  */
#define USB_OTG_GOTGCTL_HNGSCS                  ((uint32_t)0x00000100)         /*!< Host set HNP enable */
#define USB_OTG_GOTGCTL_HNPRQ                   ((uint32_t)0x00000200)         /*!< HNP request */
#define USB_OTG_GOTGCTL_HSHNPEN                 ((uint32_t)0x00000400)         /*!< Host set HNP enable */
#define USB_OTG_GOTGCTL_DHNPEN                  ((uint32_t)0x00000800)         /*!< Device HNP enabled */
#define USB_OTG_GOTGCTL_EHEN                    ((uint32_t)0x00001000)         /*!< Embedded host enable */
#define USB_OTG_GOTGCTL_CIDSTS                  ((uint32_t)0x00010000)         /*!< Connector ID status */
#define USB_OTG_GOTGCTL_DBCT                    ((uint32_t)0x00020000)         /*!< Long/short debounce time */
#define USB_OTG_GOTGCTL_ASVLD                   ((uint32_t)0x00040000)         /*!< A-session valid  */
#define USB_OTG_GOTGCTL_BSESVLD                 ((uint32_t)0x00080000)         /*!< B-session valid */
#define USB_OTG_GOTGCTL_OTGVER                  ((uint32_t)0x00100000)         /*!< OTG version  */
#define USB_OTG_GOTGCTL_MULTVALIDBC				((uint32_t)0x07c00000)
#define USB_OTG_GOTGCTL_CHIRPEN					((uint32_t)0x08000000)

/********************  Bit definition for USB_OTG_GOTGINT register  ********************/
#define USB_OTG_GOTGINT_SEDET                   ((uint32_t)0x00000004)            /*!< Session end detected                   */
#define USB_OTG_GOTGINT_SRSSCHG                 ((uint32_t)0x00000100)            /*!< Session request success status change  */
#define USB_OTG_GOTGINT_HNSSCHG                 ((uint32_t)0x00000200)            /*!< Host negotiation success status change */
#define USB_OTG_GOTGINT_HNGDET                  ((uint32_t)0x00020000)            /*!< Host negotiation detected              */
#define USB_OTG_GOTGINT_ADTOCHG                 ((uint32_t)0x00040000)            /*!< A-device timeout change                */
#define USB_OTG_GOTGINT_DBCDNE                  ((uint32_t)0x00080000)            /*!< Debounce done                          */
#define USB_OTG_GOTGINT_IDCHNG                  ((uint32_t)0x00100000)            /*!< Change in ID pin input value           */

/********************  Bit definition for USB_OTG_GAHBCFG register  ********************/
#define USB_OTG_GAHBCFG_GINT                    ((uint32_t)0x00000001)            /*!< Global interrupt mask */
#define USB_OTG_GAHBCFG_HBSTLEN                 ((uint32_t)0x0000001E)            /*!< Burst length/type */
#define USB_OTG_GAHBCFG_HBSTLEN_0               ((uint32_t)0x00000002)            /*!<Bit 0 */
#define USB_OTG_GAHBCFG_HBSTLEN_1               ((uint32_t)0x00000004)            /*!<Bit 1 */
#define USB_OTG_GAHBCFG_HBSTLEN_2               ((uint32_t)0x00000008)            /*!<Bit 2 */
#define USB_OTG_GAHBCFG_HBSTLEN_3               ((uint32_t)0x00000010)            /*!<Bit 3 */
#define USB_OTG_GAHBCFG_DMAEN                   ((uint32_t)0x00000020)            /*!< DMA enable */
#define USB_OTG_GAHBCFG_TXFELVL                 ((uint32_t)0x00000080)            /*!< TxFIFO empty level */
#define USB_OTG_GAHBCFG_PTXFELVL                ((uint32_t)0x00000100)            /*!< Periodic TxFIFO empty level */
#define USB_OTG_GAHBCFG_REMMEMSUPP				((uint32_t)0x00200000)
#define USB_OTG_GAHBCFG_NOTIALLDMAWRIT			((uint32_t)0x00400000)
#define USB_OTG_GAHBCFG_AHBSINGLE				((uint32_t)0x00800000)

/********************  Bit definition for USB_OTG_GUSBCFG register  ********************/
#define USB_OTG_GUSBCFG_TOCAL                   ((uint32_t)0x00000007)            /*!< FS timeout calibration */
#define USB_OTG_GUSBCFG_TOCAL_0                 ((uint32_t)0x00000001)            /*!<Bit 0 */
#define USB_OTG_GUSBCFG_TOCAL_1                 ((uint32_t)0x00000002)            /*!<Bit 1 */
#define USB_OTG_GUSBCFG_TOCAL_2                 ((uint32_t)0x00000004)            /*!<Bit 2 */
#define USB_OTG_GUSBCFG_PHYIF                   ((uint32_t)0x00000008)
#define USB_OTG_GUSBCFG_ULPI_UTMI_SEL           ((uint32_t)0x00000010)
#define USB_OTG_GUSBCFG_FSINTF                  ((uint32_t)0x00000020)
#define USB_OTG_GUSBCFG_PHYSEL                  ((uint32_t)0x00000040)            /*!< USB 2.0 high-speed ULPI PHY or USB 1.1 full-speed serial transceiver select */
#define USB_OTG_GUSBCFG_DDRSEL                  ((uint32_t)0x00000080)
#define USB_OTG_GUSBCFG_SRPCAP                  ((uint32_t)0x00000100)            /*!< SRP-capable */
#define USB_OTG_GUSBCFG_HNPCAP                  ((uint32_t)0x00000200)            /*!< HNP-capable */
#define USB_OTG_GUSBCFG_TRDT                    ((uint32_t)0x00003C00)            /*!< USB turnaround time */
#define USB_OTG_GUSBCFG_TRDT_0                  ((uint32_t)0x00000400)            /*!<Bit 0 */
#define USB_OTG_GUSBCFG_TRDT_1                  ((uint32_t)0x00000800)            /*!<Bit 1 */
#define USB_OTG_GUSBCFG_TRDT_2                  ((uint32_t)0x00001000)            /*!<Bit 2 */
#define USB_OTG_GUSBCFG_TRDT_3                  ((uint32_t)0x00002000)            /*!<Bit 3 */
#define USB_OTG_GUSBCFG_PHYLPCS                 ((uint32_t)0x00008000)            /*!< PHY Low-power clock select */
#define USB_OTG_GUSBCFG_OTGUTMIFSSEL            ((uint32_t)0x00010000)
#define USB_OTG_GUSBCFG_ULPIFSLS                ((uint32_t)0x00020000)            /*!< ULPI FS/LS select               */
#define USB_OTG_GUSBCFG_ULPIAR                  ((uint32_t)0x00040000)            /*!< ULPI Auto-resume                */
#define USB_OTG_GUSBCFG_ULPICSM                 ((uint32_t)0x00080000)            /*!< ULPI Clock SuspendM             */
#define USB_OTG_GUSBCFG_ULPIEVBUSD              ((uint32_t)0x00100000)            /*!< ULPI External VBUS Drive        */
#define USB_OTG_GUSBCFG_ULPIEVBUSI              ((uint32_t)0x00200000)            /*!< ULPI external VBUS indicator    */
#define USB_OTG_GUSBCFG_TSDPS                   ((uint32_t)0x00400000)            /*!< TermSel DLine pulsing selection */
#define USB_OTG_GUSBCFG_PCCI                    ((uint32_t)0x00800000)            /*!< Indicator complement            */
#define USB_OTG_GUSBCFG_PTCI                    ((uint32_t)0x01000000)            /*!< Indicator pass through          */
#define USB_OTG_GUSBCFG_ULPIIPD                 ((uint32_t)0x02000000)            /*!< ULPI interface protect disable  */
#define USB_OTG_GUSBCFG_IC_USB_CAP              ((uint32_t)0x04000000)
#define USB_OTG_GUSBCFG_IC_TRAFFIC_PULL_REMOVE  ((uint32_t)0x08000000)
#define USB_OTG_GUSBCFG_TX_END_DELAY            ((uint32_t)0x10000000)
#define USB_OTG_GUSBCFG_FHMOD                   ((uint32_t)0x20000000)            /*!< Forced host mode                */
#define USB_OTG_GUSBCFG_FDMOD                   ((uint32_t)0x40000000)            /*!< Forced peripheral mode          */
#define USB_OTG_GUSBCFG_CTXPKT                  ((uint32_t)0x80000000)            /*!< Corrupt Tx packet               */

/********************  Bit definition for USB_OTG_GRSTCTL register  ********************/
#define USB_OTG_GRSTCTL_CSRST                   ((uint32_t)0x00000001)            /*!< Core soft reset          */
#define USB_OTG_GRSTCTL_HSRST                   ((uint32_t)0x00000002)            /*!< HCLK soft reset          */
#define USB_OTG_GRSTCTL_FCRST                   ((uint32_t)0x00000004)            /*!< Host frame counter reset */
#define USB_OTG_GRSTCTL_INTKNGFLSH              ((uint32_t)0x00000008)
#define USB_OTG_GRSTCTL_RXFFLSH                 ((uint32_t)0x00000010)            /*!< RxFIFO flush             */
#define USB_OTG_GRSTCTL_TXFFLSH                 ((uint32_t)0x00000020)            /*!< TxFIFO flush             */
#define USB_OTG_GRSTCTL_TXFNUM                  ((uint32_t)0x000007C0)            /*!< TxFIFO number */
#define USB_OTG_GRSTCTL_TXFNUM_0                ((uint32_t)0x00000040)            /*!<Bit 0 */
#define USB_OTG_GRSTCTL_TXFNUM_1                ((uint32_t)0x00000080)            /*!<Bit 1 */
#define USB_OTG_GRSTCTL_TXFNUM_2                ((uint32_t)0x00000100)            /*!<Bit 2 */
#define USB_OTG_GRSTCTL_TXFNUM_3                ((uint32_t)0x00000200)            /*!<Bit 3 */
#define USB_OTG_GRSTCTL_TXFNUM_4                ((uint32_t)0x00000400)            /*!<Bit 4 */
#define USB_OTG_GRSTCTL_DMAREQ                  ((uint32_t)0x40000000)            /*!< DMA request signal */
#define USB_OTG_GRSTCTL_AHBIDL                  ((uint32_t)0x80000000)            /*!< AHB master idle */

/********************  Bit definition for USB_OTG_GINTSTS register  ********************/
#define USB_OTG_GINTSTS_CMOD                    ((uint32_t)0x00000001)            /*!< Current mode of operation                      */
#define USB_OTG_GINTSTS_MMIS                    ((uint32_t)0x00000002)            /*!< Mode mismatch interrupt                        */
#define USB_OTG_GINTSTS_OTGINT                  ((uint32_t)0x00000004)            /*!< OTG interrupt                                  */
#define USB_OTG_GINTSTS_SOF                     ((uint32_t)0x00000008)            /*!< Start of frame                                 */
#define USB_OTG_GINTSTS_RXFLVL                  ((uint32_t)0x00000010)            /*!< RxFIFO nonempty                                */
#define USB_OTG_GINTSTS_NPTXFE                  ((uint32_t)0x00000020)            /*!< Nonperiodic TxFIFO empty                       */
#define USB_OTG_GINTSTS_GINAKEFF                ((uint32_t)0x00000040)            /*!< Global IN nonperiodic NAK effective            */
#define USB_OTG_GINTSTS_BOUTNAKEFF              ((uint32_t)0x00000080)            /*!< Global OUT NAK effective                       */
#define USB_OTG_GINTSTS_ULPICKINT               ((uint32_t)0x00000100)
#define USB_OTG_GINTSTS_I2CINTR                 ((uint32_t)0x00000200)
#define USB_OTG_GINTSTS_ESUSP                   ((uint32_t)0x00000400)            /*!< Early suspend                                  */
#define USB_OTG_GINTSTS_USBSUSP                 ((uint32_t)0x00000800)            /*!< USB suspend                                    */
#define USB_OTG_GINTSTS_USBRST                  ((uint32_t)0x00001000)            /*!< USB reset                                      */
#define USB_OTG_GINTSTS_ENUMDNE                 ((uint32_t)0x00002000)            /*!< Enumeration done                               */
#define USB_OTG_GINTSTS_ISOODRP                 ((uint32_t)0x00004000)            /*!< Isochronous OUT packet dropped interrupt       */
#define USB_OTG_GINTSTS_EOPF                    ((uint32_t)0x00008000)            /*!< End of periodic frame interrupt                */
#define USB_OTG_GINTSTS_RESTOREDONE             ((uint32_t)0x00010000)
#define USB_OTG_GINTSTS_EPMISMATCH              ((uint32_t)0x00020000)
#define USB_OTG_GINTSTS_IEPINT                  ((uint32_t)0x00040000)            /*!< IN endpoint interrupt                          */
#define USB_OTG_GINTSTS_OEPINT                  ((uint32_t)0x00080000)            /*!< OUT endpoint interrupt                         */
#define USB_OTG_GINTSTS_IISOIXFR                ((uint32_t)0x00100000)            /*!< Incomplete isochronous IN transfer             */
#define USB_OTG_GINTSTS_PXFR_INCOMPISOOUT       ((uint32_t)0x00200000)            /*!< Incomplete periodic transfer                   */
#define USB_OTG_GINTSTS_DATAFSUSP               ((uint32_t)0x00400000)            /*!< Data fetch suspended                           */
#define USB_OTG_GINTSTS_RSTDET                  ((uint32_t)0x00800000)            /*!< Reset detected interrupt                       */
#define USB_OTG_GINTSTS_HPRTINT                 ((uint32_t)0x01000000)            /*!< Host port interrupt                            */
#define USB_OTG_GINTSTS_HCINT                   ((uint32_t)0x02000000)            /*!< Host channels interrupt                        */
#define USB_OTG_GINTSTS_PTXFE                   ((uint32_t)0x04000000)            /*!< Periodic TxFIFO empty                          */
#define USB_OTG_GINTSTS_LPMINT                  ((uint32_t)0x08000000)            /*!< LPM interrupt                                  */
#define USB_OTG_GINTSTS_CIDSCHG                 ((uint32_t)0x10000000)            /*!< Connector ID status change                     */
#define USB_OTG_GINTSTS_DISCINT                 ((uint32_t)0x20000000)            /*!< Disconnect detected interrupt                  */
#define USB_OTG_GINTSTS_SRQINT                  ((uint32_t)0x40000000)            /*!< Session request/new session detected interrupt */
#define USB_OTG_GINTSTS_WKUINT                  ((uint32_t)0x80000000)            /*!< Resume/remote wakeup detected interrupt        */

/********************  Bit definition for USB_OTG_GINTMSK register  ********************/
#define USB_OTG_GINTMSK_MMISM                   ((uint32_t)0x00000002)            /*!< Mode mismatch interrupt mask                        */
#define USB_OTG_GINTMSK_OTGINT                  ((uint32_t)0x00000004)            /*!< OTG interrupt mask                                  */
#define USB_OTG_GINTMSK_SOFM                    ((uint32_t)0x00000008)            /*!< Start of frame mask                                 */
#define USB_OTG_GINTMSK_RXFLVLM                 ((uint32_t)0x00000010)            /*!< Receive FIFO nonempty mask                          */
#define USB_OTG_GINTMSK_NPTXFEM                 ((uint32_t)0x00000020)            /*!< Nonperiodic TxFIFO empty mask                       */
#define USB_OTG_GINTMSK_GINAKEFFM               ((uint32_t)0x00000040)            /*!< Global nonperiodic IN NAK effective mask            */
#define USB_OTG_GINTMSK_GONAKEFFM               ((uint32_t)0x00000080)            /*!< Global OUT NAK effective mask                       */
#define USB_OTG_GINTMSK_ESUSPM                  ((uint32_t)0x00000400)            /*!< Early suspend mask                                  */
#define USB_OTG_GINTMSK_USBSUSPM                ((uint32_t)0x00000800)            /*!< USB suspend mask                                    */
#define USB_OTG_GINTMSK_USBRST                  ((uint32_t)0x00001000)            /*!< USB reset mask                                      */
#define USB_OTG_GINTMSK_ENUMDNEM                ((uint32_t)0x00002000)            /*!< Enumeration done mask                               */
#define USB_OTG_GINTMSK_ISOODRPM                ((uint32_t)0x00004000)            /*!< Isochronous OUT packet dropped interrupt mask       */
#define USB_OTG_GINTMSK_EOPFM                   ((uint32_t)0x00008000)            /*!< End of periodic frame interrupt mask                */
#define USB_OTG_GINTMSK_EPMISM                  ((uint32_t)0x00020000)            /*!< Endpoint mismatch interrupt mask                    */
#define USB_OTG_GINTMSK_IEPINT                  ((uint32_t)0x00040000)            /*!< IN endpoints interrupt mask                         */
#define USB_OTG_GINTMSK_OEPINT                  ((uint32_t)0x00080000)            /*!< OUT endpoints interrupt mask                        */
#define USB_OTG_GINTMSK_IISOIXFRM               ((uint32_t)0x00100000)            /*!< Incomplete isochronous IN transfer mask             */
#define USB_OTG_GINTMSK_PXFRM_IISOOXFRM         ((uint32_t)0x00200000)            /*!< Incomplete periodic transfer mask                   */
#define USB_OTG_GINTMSK_FSUSPM                  ((uint32_t)0x00400000)            /*!< Data fetch suspended mask                           */
#define USB_OTG_GINTMSK_RSTDEM                  ((uint32_t)0x00800000)            /*!< Reset detected interrupt mask                      */
#define USB_OTG_GINTMSK_PRTIM                   ((uint32_t)0x01000000)            /*!< Host port interrupt mask                            */
#define USB_OTG_GINTMSK_HCIM                    ((uint32_t)0x02000000)            /*!< Host channels interrupt mask                        */
#define USB_OTG_GINTMSK_PTXFEM                  ((uint32_t)0x04000000)            /*!< Periodic TxFIFO empty mask                          */
#define USB_OTG_GINTMSK_LPMINTM                 ((uint32_t)0x08000000)            /*!< LPM interrupt Mask                                  */
#define USB_OTG_GINTMSK_CIDSCHGM                ((uint32_t)0x10000000)            /*!< Connector ID status change mask                     */
#define USB_OTG_GINTMSK_DISCINT                 ((uint32_t)0x20000000)            /*!< Disconnect detected interrupt mask                  */
#define USB_OTG_GINTMSK_SRQIM                   ((uint32_t)0x40000000)            /*!< Session request/new session detected interrupt mask */
#define USB_OTG_GINTMSK_WUIM                    ((uint32_t)0x80000000)            /*!< Resume/remote wakeup detected interrupt mask        */

/********************  Bit definition for USB_OTG_GRXSTSP register  ********************/
#define USB_OTG_GRXSTSP_EPNUM                   ((uint32_t)0x0000000F)            /*!< IN EP interrupt mask bits  */
#define USB_OTG_GRXSTSP_BCNT                    ((uint32_t)0x00007FF0)            /*!< OUT EP interrupt mask bits */
#define USB_OTG_GRXSTSP_DPID                    ((uint32_t)0x00018000)            /*!< OUT EP interrupt mask bits */
#define USB_OTG_GRXSTSP_PKTSTS                  ((uint32_t)0x001E0000)            /*!< OUT EP interrupt mask bits */

/********************  Bit definition for USB_OTG_GRXFSIZ register  ********************/
#define USB_OTG_GRXFSIZ_RXFD                     ((uint32_t)0x0000FFFF)            /*!< RxFIFO depth */

/********************  Bit definition for USB_OTG_GNPTXSTS register  ********************/
#define USB_OTG_GNPTXSTS_NPTXFSAV                ((uint32_t)0x0000FFFF)            /*!< Nonperiodic TxFIFO space available */

#define USB_OTG_GNPTXSTS_NPTQXSAV                ((uint32_t)0x00FF0000)            /*!< Nonperiodic transmit request queue space available */
#define USB_OTG_GNPTXSTS_NPTQXSAV_0              ((uint32_t)0x00010000)            /*!<Bit 0 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_1              ((uint32_t)0x00020000)            /*!<Bit 1 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_2              ((uint32_t)0x00040000)            /*!<Bit 2 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_3              ((uint32_t)0x00080000)            /*!<Bit 3 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_4              ((uint32_t)0x00100000)            /*!<Bit 4 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_5              ((uint32_t)0x00200000)            /*!<Bit 5 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_6              ((uint32_t)0x00400000)            /*!<Bit 6 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_7              ((uint32_t)0x00800000)            /*!<Bit 7 */

#define USB_OTG_GNPTXSTS_NPTXQTOP                ((uint32_t)0x7F000000)            /*!< Top of the nonperiodic transmit request queue */
#define USB_OTG_GNPTXSTS_NPTXQTOP_0              ((uint32_t)0x01000000)            /*!<Bit 0 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_1              ((uint32_t)0x02000000)            /*!<Bit 1 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_2              ((uint32_t)0x04000000)            /*!<Bit 2 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_3              ((uint32_t)0x08000000)            /*!<Bit 3 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_4              ((uint32_t)0x10000000)            /*!<Bit 4 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_5              ((uint32_t)0x20000000)            /*!<Bit 5 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_6              ((uint32_t)0x40000000)            /*!<Bit 6 */

/********************  Bit definition for USB_OTG_GI2CCTL register  *****************/
// TODO
/********************  Bit definition for USB_OTG_GPVNDCTL register  ****************/
// TODO

/********************  Bit definition for USB_OTG_GCCFG register  ********************/
#define USB_OTG_GCCFG_PWRDWN                 ((uint32_t)0x00010000)              /*!< Power down */
#define USB_OTG_GCCFG_VBUSACEN               ((uint32_t)0x00040000)              /*!< USB VBUS A-device Comparer Enable */
#define USB_OTG_GCCFG_VBUSBCEN               ((uint32_t)0x00080000)              /*!< USB VBUS B-device Comparer Enable */
#define USB_OTG_GCCFG_VBDEN                  ((uint32_t)0x00200000)              /*!< USB VBUS Detection Enable */

/********************  Bit definition for USB_OTG_GLPMCFG register  ********************/
#define  USB_OTG_GLPMCFG_LPMEN               ((uint32_t)0x00000001)            /*!< LPM support enable                                     */
#define  USB_OTG_GLPMCFG_LPMACK              ((uint32_t)0x00000002)            /*!< LPM Token acknowledge enable                           */
#define  USB_OTG_GLPMCFG_BESL                ((uint32_t)0x0000003C)            /*!< BESL value received with last ACKed LPM Token          */
#define  USB_OTG_GLPMCFG_REMWAKE             ((uint32_t)0x00000040)            /*!< bRemoteWake value received with last ACKed LPM Token   */
#define  USB_OTG_GLPMCFG_L1SSEN              ((uint32_t)0x00000080)            /*!< L1 shallow sleep enable                                */
#define  USB_OTG_GLPMCFG_BESLTHRS            ((uint32_t)0x00000F00)            /*!< BESL threshold                                         */
#define  USB_OTG_GLPMCFG_L1DSEN              ((uint32_t)0x00001000)            /*!< L1 deep sleep enable                                   */
#define  USB_OTG_GLPMCFG_LPMRSP              ((uint32_t)0x00006000)            /*!< LPM response                                           */
#define  USB_OTG_GLPMCFG_SLPSTS              ((uint32_t)0x00008000)            /*!< Port sleep status                                      */
#define  USB_OTG_GLPMCFG_L1RSMOK             ((uint32_t)0x00010000)            /*!< Sleep State Resume OK                                  */
#define  USB_OTG_GLPMCFG_LPMCHIDX            ((uint32_t)0x001E0000)            /*!< LPM Channel Index                                      */
#define  USB_OTG_GLPMCFG_LPMRCNT             ((uint32_t)0x00E00000)            /*!< LPM retry count                                        */
#define  USB_OTG_GLPMCFG_SNDLPM              ((uint32_t)0x01000000)            /*!< Send LPM transaction                                   */
#define  USB_OTG_GLPMCFG_LPMRCNTSTS          ((uint32_t)0x0E000000)            /*!< LPM retry count status                                 */
#define  USB_OTG_GLPMCFG_ENBESL              ((uint32_t)0x10000000)            /*!< Enable best effort service latency                     */

/********************  Bit definition for USB_OTG_GPWRDN) register  ********************/
#define USB_OTG_GPWRDN_ADPMEN                 ((uint32_t)0x00000001)             /*!< ADP module enable */
#define USB_OTG_GPWRDN_ADPIF                  ((uint32_t)0x00800000)             /*!< ADP Interrupt flag */

/********************  Bit definition for USB_OTG_HPTXFSIZ register  ********************/
#define USB_OTG_HPTXFSIZ_PTXSA                   ((uint32_t)0x0000FFFF)            /*!< Host periodic TxFIFO start address            */
#define USB_OTG_HPTXFSIZ_PTXFD                   ((uint32_t)0xFFFF0000)            /*!< Host periodic TxFIFO depth                    */

/********************  Bit definition for USB_OTG_DVBUSDIS register  ********************/
#define USB_OTG_DVBUSDIS_VBUSDT         ((uint32_t)0x0000FFFF)            /*!< Device VBUS discharge time */

/********************  Bit definition for USB_OTG_DVBUSPULSE register  ********************/
#define USB_OTG_DVBUSPULSE_DVBUSP                  ((uint32_t)0x00000FFF)            /*!< Device VBUS pulsing time */

/********************  Bit definition for USB_OTG_DIEPTXF register  ********************/
#define USB_OTG_DIEPTXF_INEPTXSA                 ((uint32_t)0x0000FFFF)            /*!< IN endpoint FIFOx transmit RAM start address */
#define USB_OTG_DIEPTXF_INEPTXFD                 ((uint32_t)0xFFFF0000)            /*!< IN endpoint TxFIFO depth */



/**
 * Device Global Registers. <i>Offsets 800h-BFFh</i>
 *
 * The following structures define the size and relative field offsets
 * for the Device Mode Registers.
 *
 * <i>These registers are visible only in Device mode and must not be
 * accessed in Host mode, as the results are unknown.</i>
 */
struct dwcotg_dev_global_regs_t {
	/** Device Configuration Register. <i>Offset 800h</i> */
	volatile uint32_t dcfg;
	/** Device Control Register. <i>Offset: 804h</i> */
	volatile uint32_t dctl;
	/** Device Status Register (Read Only). <i>Offset: 808h</i> */
	volatile uint32_t dsts;
	/** Reserved. <i>Offset: 80Ch</i> */
	uint32_t unused;
	/** Device IN Endpoint Common Interrupt Mask
	 * Register. <i>Offset: 810h</i> */
	volatile uint32_t diepmsk;
	/** Device OUT Endpoint Common Interrupt Mask
	 * Register. <i>Offset: 814h</i> */
	volatile uint32_t doepmsk;
	/** Device All Endpoints Interrupt Register.  <i>Offset: 818h</i> */
	volatile uint32_t daint;
	/** Device All Endpoints Interrupt Mask Register.  <i>Offset:
	 * 81Ch</i> */
	volatile uint32_t daintmsk;
	/** Device IN Token Queue Read Register-1 (Read Only).
	 * <i>Offset: 820h</i> */
	volatile uint32_t dtknqr1;
	/** Device IN Token Queue Read Register-2 (Read Only).
	 * <i>Offset: 824h</i> */
	volatile uint32_t dtknqr2;
	/** Device VBUS	 discharge Register.  <i>Offset: 828h</i> */
	volatile uint32_t dvbusdis;
	/** Device VBUS Pulse Register.	 <i>Offset: 82Ch</i> */
	volatile uint32_t dvbuspulse;
	/** Device IN Token Queue Read Register-3 (Read Only). /
	 *	Device Thresholding control register (Read/Write)
	 * <i>Offset: 830h</i> */
	volatile uint32_t dtknqr3_dthrctl;
	/** Device IN Token Queue Read Register-4 (Read Only). /
	 *	Device IN EPs empty Inr. Mask Register (Read/Write)
	 * <i>Offset: 834h</i> */
	volatile uint32_t dtknqr4_fifoemptymsk;
	/** Device Each Endpoint Interrupt Register (Read Only). /
	 * <i>Offset: 838h</i> */
	volatile uint32_t deachint;
	/** Device Each Endpoint Interrupt mask Register (Read/Write). /
	 * <i>Offset: 83Ch</i> */
	volatile uint32_t deachintmsk;
	/** Device Each In Endpoint Interrupt mask Register (Read/Write). /
	 * <i>Offset: 840h</i> */
	volatile uint32_t diepeachintmsk[MAX_EPS_CHANNELS];
	/** Device Each Out Endpoint Interrupt mask Register (Read/Write). /
	 * <i>Offset: 880h</i> */
	volatile uint32_t doepeachintmsk[MAX_EPS_CHANNELS];
};

/********************  Bit definition for USB_OTG_DCFG register  ********************/
#define USB_OTG_DCFG_DSPD                    ((uint32_t)0x00000003)            /*!< Device speed */
#define USB_OTG_DCFG_DSPD_0                  ((uint32_t)0x00000001)            /*!<Bit 0 */
#define USB_OTG_DCFG_DSPD_1                  ((uint32_t)0x00000002)            /*!<Bit 1 */
#define USB_OTG_DCFG_NZLSOHSK                ((uint32_t)0x00000004)            /*!< Nonzero-length status OUT handshake */

#define USB_OTG_DCFG_DAD                     ((uint32_t)0x000007F0)            /*!< Device address */
#define USB_OTG_DCFG_DAD_0                   ((uint32_t)0x00000010)            /*!<Bit 0 */
#define USB_OTG_DCFG_DAD_1                   ((uint32_t)0x00000020)            /*!<Bit 1 */
#define USB_OTG_DCFG_DAD_2                   ((uint32_t)0x00000040)            /*!<Bit 2 */
#define USB_OTG_DCFG_DAD_3                   ((uint32_t)0x00000080)            /*!<Bit 3 */
#define USB_OTG_DCFG_DAD_4                   ((uint32_t)0x00000100)            /*!<Bit 4 */
#define USB_OTG_DCFG_DAD_5                   ((uint32_t)0x00000200)            /*!<Bit 5 */
#define USB_OTG_DCFG_DAD_6                   ((uint32_t)0x00000400)            /*!<Bit 6 */

#define USB_OTG_DCFG_PFIVL                   ((uint32_t)0x00001800)            /*!< Periodic (micro)frame interval */
#define USB_OTG_DCFG_PFIVL_0                 ((uint32_t)0x00000800)            /*!<Bit 0 */
#define USB_OTG_DCFG_PFIVL_1                 ((uint32_t)0x00001000)            /*!<Bit 1 */

#define USB_OTG_DCFG_PERSCHIVL               ((uint32_t)0x03000000)            /*!< Periodic scheduling interval */
#define USB_OTG_DCFG_PERSCHIVL_0             ((uint32_t)0x01000000)            /*!<Bit 0 */
#define USB_OTG_DCFG_PERSCHIVL_1             ((uint32_t)0x02000000)            /*!<Bit 1 */

/********************  Bit definition for USB_OTG_DCTL register  ********************/
#define USB_OTG_DCTL_RWUSIG                  ((uint32_t)0x00000001)            /*!< Remote wakeup signaling */
#define USB_OTG_DCTL_SDIS                    ((uint32_t)0x00000002)            /*!< Soft disconnect         */
#define USB_OTG_DCTL_GINSTS                  ((uint32_t)0x00000004)            /*!< Global IN NAK status    */
#define USB_OTG_DCTL_GONSTS                  ((uint32_t)0x00000008)            /*!< Global OUT NAK status   */

#define USB_OTG_DCTL_TCTL                    ((uint32_t)0x00000070)            /*!< Test control */
#define USB_OTG_DCTL_TCTL_0                  ((uint32_t)0x00000010)            /*!<Bit 0 */
#define USB_OTG_DCTL_TCTL_1                  ((uint32_t)0x00000020)            /*!<Bit 1 */
#define USB_OTG_DCTL_TCTL_2                  ((uint32_t)0x00000040)            /*!<Bit 2 */
#define USB_OTG_DCTL_SGINAK                  ((uint32_t)0x00000080)            /*!< Set global IN NAK         */
#define USB_OTG_DCTL_CGINAK                  ((uint32_t)0x00000100)            /*!< Clear global IN NAK       */
#define USB_OTG_DCTL_SGONAK                  ((uint32_t)0x00000200)            /*!< Set global OUT NAK        */
#define USB_OTG_DCTL_CGONAK                  ((uint32_t)0x00000400)            /*!< Clear global OUT NAK      */
#define USB_OTG_DCTL_POPRGDNE                ((uint32_t)0x00000800)            /*!< Power-on programming done */

/********************  Bit definition for USB_OTG_DSTS register  ********************/
#define USB_OTG_DSTS_SUSPSTS                 ((uint32_t)0x00000001)            /*!< Suspend status   */

#define USB_OTG_DSTS_ENUMSPD                 ((uint32_t)0x00000006)            /*!< Enumerated speed */
#define USB_OTG_DSTS_ENUMSPD_0               ((uint32_t)0x00000002)            /*!<Bit 0 */
#define USB_OTG_DSTS_ENUMSPD_1               ((uint32_t)0x00000004)            /*!<Bit 1 */
#define USB_OTG_DSTS_EERR                    ((uint32_t)0x00000008)            /*!< Erratic error     */
#define USB_OTG_DSTS_FNSOF                   ((uint32_t)0x003FFF00)            /*!< Frame number of the received SOF */

/********************  Bit definition for USB_OTG_DIEPMSK register  ********************/
#define USB_OTG_DIEPMSK_XFRCM                   ((uint32_t)0x00000001)            /*!< Transfer completed interrupt mask                 */
#define USB_OTG_DIEPMSK_EPDM                    ((uint32_t)0x00000002)            /*!< Endpoint disabled interrupt mask                  */
#define USB_OTG_DIEPMSK_TOM                     ((uint32_t)0x00000008)            /*!< Timeout condition mask (nonisochronous endpoints) */
#define USB_OTG_DIEPMSK_ITTXFEMSK               ((uint32_t)0x00000010)            /*!< IN token received when TxFIFO empty mask          */
#define USB_OTG_DIEPMSK_INEPNMM                 ((uint32_t)0x00000020)            /*!< IN token received with EP mismatch mask           */
#define USB_OTG_DIEPMSK_INEPNEM                 ((uint32_t)0x00000040)            /*!< IN endpoint NAK effective mask                    */
#define USB_OTG_DIEPMSK_TXFURM                  ((uint32_t)0x00000100)            /*!< FIFO underrun mask                                */
#define USB_OTG_DIEPMSK_BIM                     ((uint32_t)0x00000200)            /*!< BNA interrupt mask                                */

/********************  Bit definition for USB_OTG_DOEPMSK register  ********************/
#define USB_OTG_DOEPMSK_XFRCM                   ((uint32_t)0x00000001)            /*!< Transfer completed interrupt mask */
#define USB_OTG_DOEPMSK_EPDM                    ((uint32_t)0x00000002)            /*!< Endpoint disabled interrupt mask               */
#define USB_OTG_DOEPMSK_STUPM                   ((uint32_t)0x00000008)            /*!< SETUP phase done mask                          */
#define USB_OTG_DOEPMSK_OTEPDM                  ((uint32_t)0x00000010)            /*!< OUT token received when endpoint disabled mask */
#define USB_OTG_DOEPMSK_B2BSTUP                 ((uint32_t)0x00000040)            /*!< Back-to-back SETUP packets received mask       */
#define USB_OTG_DOEPMSK_OPEM                    ((uint32_t)0x00000100)            /*!< OUT packet error mask                          */
#define USB_OTG_DOEPMSK_BOIM                    ((uint32_t)0x00000200)            /*!< BNA interrupt mask                             */

/********************  Bit definition for USB_OTG_DAINT register  ********************/
#define USB_OTG_DAINT_IEPINT                  ((uint32_t)0x0000FFFF)            /*!< IN endpoint interrupt bits  */
#define USB_OTG_DAINT_OEPINT                  ((uint32_t)0xFFFF0000)            /*!< OUT endpoint interrupt bits */

/********************  Bit definition for USB_OTG_DAINTMSK register  ********************/
#define USB_OTG_DAINTMSK_IEPM                    ((uint32_t)0x0000FFFF)            /*!< IN EP interrupt mask bits */
#define USB_OTG_DAINTMSK_OEPM                    ((uint32_t)0xFFFF0000)            /*!< OUT EP interrupt mask bits */

/********************  Bit definition for USB_OTG_DTHRCTL register  ********************/
#define USB_OTG_DTHRCTL_NONISOTHREN             ((uint32_t)0x00000001)            /*!< Nonisochronous IN endpoints threshold enable */
#define USB_OTG_DTHRCTL_ISOTHREN                ((uint32_t)0x00000002)            /*!< ISO IN endpoint threshold enable */

#define USB_OTG_DTHRCTL_TXTHRLEN                ((uint32_t)0x000007FC)            /*!< Transmit threshold length */
#define USB_OTG_DTHRCTL_TXTHRLEN_0              ((uint32_t)0x00000004)            /*!<Bit 0 */
#define USB_OTG_DTHRCTL_TXTHRLEN_1              ((uint32_t)0x00000008)            /*!<Bit 1 */
#define USB_OTG_DTHRCTL_TXTHRLEN_2              ((uint32_t)0x00000010)            /*!<Bit 2 */
#define USB_OTG_DTHRCTL_TXTHRLEN_3              ((uint32_t)0x00000020)            /*!<Bit 3 */
#define USB_OTG_DTHRCTL_TXTHRLEN_4              ((uint32_t)0x00000040)            /*!<Bit 4 */
#define USB_OTG_DTHRCTL_TXTHRLEN_5              ((uint32_t)0x00000080)            /*!<Bit 5 */
#define USB_OTG_DTHRCTL_TXTHRLEN_6              ((uint32_t)0x00000100)            /*!<Bit 6 */
#define USB_OTG_DTHRCTL_TXTHRLEN_7              ((uint32_t)0x00000200)            /*!<Bit 7 */
#define USB_OTG_DTHRCTL_TXTHRLEN_8              ((uint32_t)0x00000400)            /*!<Bit 8 */
#define USB_OTG_DTHRCTL_RXTHREN                 ((uint32_t)0x00010000)            /*!< Receive threshold enable */

#define USB_OTG_DTHRCTL_RXTHRLEN                ((uint32_t)0x03FE0000)            /*!< Receive threshold length */
#define USB_OTG_DTHRCTL_RXTHRLEN_0              ((uint32_t)0x00020000)            /*!<Bit 0 */
#define USB_OTG_DTHRCTL_RXTHRLEN_1              ((uint32_t)0x00040000)            /*!<Bit 1 */
#define USB_OTG_DTHRCTL_RXTHRLEN_2              ((uint32_t)0x00080000)            /*!<Bit 2 */
#define USB_OTG_DTHRCTL_RXTHRLEN_3              ((uint32_t)0x00100000)            /*!<Bit 3 */
#define USB_OTG_DTHRCTL_RXTHRLEN_4              ((uint32_t)0x00200000)            /*!<Bit 4 */
#define USB_OTG_DTHRCTL_RXTHRLEN_5              ((uint32_t)0x00400000)            /*!<Bit 5 */
#define USB_OTG_DTHRCTL_RXTHRLEN_6              ((uint32_t)0x00800000)            /*!<Bit 6 */
#define USB_OTG_DTHRCTL_RXTHRLEN_7              ((uint32_t)0x01000000)            /*!<Bit 7 */
#define USB_OTG_DTHRCTL_RXTHRLEN_8              ((uint32_t)0x02000000)            /*!<Bit 8 */
#define USB_OTG_DTHRCTL_ARPEN                   ((uint32_t)0x08000000)            /*!< Arbiter parking enable */

/********************  Bit definition for USB_OTG_DIEPEMPMSK register  ********************/
#define USB_OTG_DIEPEMPMSK_INEPTXFEM               ((uint32_t)0x0000FFFF)         /*!< IN EP Tx FIFO empty interrupt mask bits */

/********************  Bit definition for USB_OTG_DEACHINT register  ********************/
#define USB_OTG_DEACHINT_IEP1INT                 ((uint32_t)0x00000002)           /*!< IN endpoint 1interrupt bit   */
#define USB_OTG_DEACHINT_OEP1INT                 ((uint32_t)0x00020000)           /*!< OUT endpoint 1 interrupt bit */

/********************  Bit definition for USB_OTG_DEACHINT register  ********************/
#define USB_OTG_DEACHINT_IEP1INT                 ((uint32_t)0x00000002)           /*!< IN endpoint 1interrupt bit   */
#define USB_OTG_DEACHINT_OEP1INT                 ((uint32_t)0x00020000)           /*!< OUT endpoint 1 interrupt bit */

/********************  Bit definition for USB_OTG_DEACHINTMSK register  ********************/
#define USB_OTG_DEACHINTMSK_IEP1INTM          ((uint32_t)0x00000002)            /*!< IN Endpoint 1 interrupt mask bit  */
#define USB_OTG_DEACHINTMSK_OEP1INTM          ((uint32_t)0x00020000)            /*!< OUT Endpoint 1 interrupt mask bit */
















/**
 * The Host Global Registers structure defines the size and relative
 * field offsets for the Host Mode Global Registers.  Host Global
 * Registers offsets 400h-7FFh.
*/
struct dwcotg_host_global_regs_t {
	/** Host Configuration Register.   <i>Offset: 400h</i> */
	volatile uint32_t hcfg;
	/** Host Frame Interval Register.	<i>Offset: 404h</i> */
	volatile uint32_t hfir;
	/** Host Frame Number / Frame Remaining Register. <i>Offset: 408h</i> */
	volatile uint32_t hfnum;
	/** Reserved.	<i>Offset: 40Ch</i> */
	uint32_t reserved40C;
	/** Host Periodic Transmit FIFO/ Queue Status Register. <i>Offset: 410h</i> */
	volatile uint32_t hptxsts;
	/** Host All Channels Interrupt Register. <i>Offset: 414h</i> */
	volatile uint32_t haint;
	/** Host All Channels Interrupt Mask Register. <i>Offset: 418h</i> */
	volatile uint32_t haintmsk;
	/** Host Frame List Base Address Register . <i>Offset: 41Ch</i> */
	volatile uint32_t hflbaddr;
};

/********************  Bit definition for USB_OTG_HCFG register  ********************/
#define USB_OTG_HCFG_FSLSPCS                 ((uint32_t)0x00000003)            /*!< FS/LS PHY clock select  */
#define USB_OTG_HCFG_FSLSPCS_0               ((uint32_t)0x00000001)            /*!<Bit 0 */
#define USB_OTG_HCFG_FSLSPCS_1               ((uint32_t)0x00000002)            /*!<Bit 1 */
#define USB_OTG_HCFG_FSLSS                   ((uint32_t)0x00000004)            /*!< FS- and LS-only support */

/********************  Bit definition for USB_OTG_HFIR register  ********************/
#define USB_OTG_HFIR_FRIVL                   ((uint32_t)0x0000FFFF)            /*!< Frame interval */

/********************  Bit definition for USB_OTG_HFNUM register  ********************/
#define USB_OTG_HFNUM_FRNUM                   ((uint32_t)0x0000FFFF)            /*!< Frame number         */
#define USB_OTG_HFNUM_FTREM                   ((uint32_t)0xFFFF0000)            /*!< Frame time remaining */

/********************  Bit definition for USB_OTG_HPTXSTS register  ********************/
#define USB_OTG_HPTXSTS_PTXFSAVL                ((uint32_t)0x0000FFFF)            /*!< Periodic transmit data FIFO space available     */
#define USB_OTG_HPTXSTS_PTXQSAV                 ((uint32_t)0x00FF0000)            /*!< Periodic transmit request queue space available */
#define USB_OTG_HPTXSTS_PTXQSAV_0               ((uint32_t)0x00010000)            /*!<Bit 0 */
#define USB_OTG_HPTXSTS_PTXQSAV_1               ((uint32_t)0x00020000)            /*!<Bit 1 */
#define USB_OTG_HPTXSTS_PTXQSAV_2               ((uint32_t)0x00040000)            /*!<Bit 2 */
#define USB_OTG_HPTXSTS_PTXQSAV_3               ((uint32_t)0x00080000)            /*!<Bit 3 */
#define USB_OTG_HPTXSTS_PTXQSAV_4               ((uint32_t)0x00100000)            /*!<Bit 4 */
#define USB_OTG_HPTXSTS_PTXQSAV_5               ((uint32_t)0x00200000)            /*!<Bit 5 */
#define USB_OTG_HPTXSTS_PTXQSAV_6               ((uint32_t)0x00400000)            /*!<Bit 6 */
#define USB_OTG_HPTXSTS_PTXQSAV_7               ((uint32_t)0x00800000)            /*!<Bit 7 */

#define USB_OTG_HPTXSTS_PTXQTOP                 ((uint32_t)0xFF000000)            /*!< Top of the periodic transmit request queue */
#define USB_OTG_HPTXSTS_PTXQTOP_0               ((uint32_t)0x01000000)            /*!<Bit 0 */
#define USB_OTG_HPTXSTS_PTXQTOP_1               ((uint32_t)0x02000000)            /*!<Bit 1 */
#define USB_OTG_HPTXSTS_PTXQTOP_2               ((uint32_t)0x04000000)            /*!<Bit 2 */
#define USB_OTG_HPTXSTS_PTXQTOP_3               ((uint32_t)0x08000000)            /*!<Bit 3 */
#define USB_OTG_HPTXSTS_PTXQTOP_4               ((uint32_t)0x10000000)            /*!<Bit 4 */
#define USB_OTG_HPTXSTS_PTXQTOP_5               ((uint32_t)0x20000000)            /*!<Bit 5 */
#define USB_OTG_HPTXSTS_PTXQTOP_6               ((uint32_t)0x40000000)            /*!<Bit 6 */
#define USB_OTG_HPTXSTS_PTXQTOP_7               ((uint32_t)0x80000000)            /*!<Bit 7 */

/********************  Bit definition for USB_OTG_HAINT register  ********************/
#define USB_OTG_HAINT_HAINT                   ((uint32_t)0x0000FFFF)            /*!< Channel interrupts */

/********************  Bit definition for USB_OTG_HAINTMSK register  ********************/
#define USB_OTG_HAINTMSK_HAINTM                  ((uint32_t)0x0000FFFF)            /*!< Channel interrupt mask */


/** Host Port 0 Control and Status Register */
/********************  Bit definition for USB_OTG_HPRT register  ********************/
#define USB_OTG_HPRT_PCSTS                   ((uint32_t)0x00000001)            /*!< Port connect status        */
#define USB_OTG_HPRT_PCDET                   ((uint32_t)0x00000002)            /*!< Port connect detected      */
#define USB_OTG_HPRT_PENA                    ((uint32_t)0x00000004)            /*!< Port enable                */
#define USB_OTG_HPRT_PENCHNG                 ((uint32_t)0x00000008)            /*!< Port enable/disable change */
#define USB_OTG_HPRT_POCA                    ((uint32_t)0x00000010)            /*!< Port overcurrent active    */
#define USB_OTG_HPRT_POCCHNG                 ((uint32_t)0x00000020)            /*!< Port overcurrent change    */
#define USB_OTG_HPRT_PRES                    ((uint32_t)0x00000040)            /*!< Port resume                */
#define USB_OTG_HPRT_PSUSP                   ((uint32_t)0x00000080)            /*!< Port suspend               */
#define USB_OTG_HPRT_PRST                    ((uint32_t)0x00000100)            /*!< Port reset                 */

#define USB_OTG_HPRT_PLSTS                   ((uint32_t)0x00000C00)            /*!< Port line status           */
#define USB_OTG_HPRT_PLSTS_0                 ((uint32_t)0x00000400)            /*!<Bit 0 */
#define USB_OTG_HPRT_PLSTS_1                 ((uint32_t)0x00000800)            /*!<Bit 1 */
#define USB_OTG_HPRT_PPWR                    ((uint32_t)0x00001000)            /*!< Port power                 */

#define USB_OTG_HPRT_PTCTL                   ((uint32_t)0x0001E000)            /*!< Port test control          */
#define USB_OTG_HPRT_PTCTL_0                 ((uint32_t)0x00002000)            /*!<Bit 0 */
#define USB_OTG_HPRT_PTCTL_1                 ((uint32_t)0x00004000)            /*!<Bit 1 */
#define USB_OTG_HPRT_PTCTL_2                 ((uint32_t)0x00008000)            /*!<Bit 2 */
#define USB_OTG_HPRT_PTCTL_3                 ((uint32_t)0x00010000)            /*!<Bit 3 */

#define USB_OTG_HPRT_PSPD                    ((uint32_t)0x00060000)            /*!< Port speed                 */
#define USB_OTG_HPRT_PSPD_0                  ((uint32_t)0x00020000)            /*!<Bit 0 */
#define USB_OTG_HPRT_PSPD_1                  ((uint32_t)0x00040000)            /*!<Bit 1 */


/**
 * Host Channel Specific Registers. <i>500h-5FCh</i>
 */
struct dwcotg_hc_regs_t {
	/** Host Channel 0 Characteristic Register. <i>Offset: 500h + (chan_num * 20h) + 00h</i> */
	volatile uint32_t hcchar;
	/** Host Channel 0 Split Control Register. <i>Offset: 500h + (chan_num * 20h) + 04h</i> */
	volatile uint32_t hcsplt;
	/** Host Channel 0 Interrupt Register. <i>Offset: 500h + (chan_num * 20h) + 08h</i> */
	volatile uint32_t hcint;
	/** Host Channel 0 Interrupt Mask Register. <i>Offset: 500h + (chan_num * 20h) + 0Ch</i> */
	volatile uint32_t hcintmsk;
	/** Host Channel 0 Transfer Size Register. <i>Offset: 500h + (chan_num * 20h) + 10h</i> */
	volatile uint32_t hctsiz;
	/** Host Channel 0 DMA Address Register. <i>Offset: 500h + (chan_num * 20h) + 14h</i> */
	volatile uint32_t hcdma;
	volatile uint32_t reserved;
	/** Host Channel 0 DMA Buffer Address Register. <i>Offset: 500h + (chan_num * 20h) + 1Ch</i> */
	volatile uint32_t hcdmab;
};

/********************  Bit definition for USB_OTG_HCCHAR register  ********************/
#define USB_OTG_HCCHAR_MPSIZ                   ((uint32_t)0x000007FF)            /*!< Maximum packet size */

#define USB_OTG_HCCHAR_EPNUM                   ((uint32_t)0x00007800)            /*!< Endpoint number */
#define USB_OTG_HCCHAR_EPNUM_0                 ((uint32_t)0x00000800)            /*!<Bit 0 */
#define USB_OTG_HCCHAR_EPNUM_1                 ((uint32_t)0x00001000)            /*!<Bit 1 */
#define USB_OTG_HCCHAR_EPNUM_2                 ((uint32_t)0x00002000)            /*!<Bit 2 */
#define USB_OTG_HCCHAR_EPNUM_3                 ((uint32_t)0x00004000)            /*!<Bit 3 */
#define USB_OTG_HCCHAR_EPDIR                   ((uint32_t)0x00008000)            /*!< Endpoint direction */
#define USB_OTG_HCCHAR_LSDEV                   ((uint32_t)0x00020000)            /*!< Low-speed device */

#define USB_OTG_HCCHAR_EPTYP                   ((uint32_t)0x000C0000)            /*!< Endpoint type */
#define USB_OTG_HCCHAR_EPTYP_0                 ((uint32_t)0x00040000)            /*!<Bit 0 */
#define USB_OTG_HCCHAR_EPTYP_1                 ((uint32_t)0x00080000)            /*!<Bit 1 */

#define USB_OTG_HCCHAR_MC                      ((uint32_t)0x00300000)            /*!< Multi Count (MC) / Error Count (EC) */
#define USB_OTG_HCCHAR_MC_0                    ((uint32_t)0x00100000)            /*!<Bit 0 */
#define USB_OTG_HCCHAR_MC_1                    ((uint32_t)0x00200000)            /*!<Bit 1 */

#define USB_OTG_HCCHAR_DAD                     ((uint32_t)0x1FC00000)            /*!< Device address */
#define USB_OTG_HCCHAR_DAD_0                   ((uint32_t)0x00400000)            /*!<Bit 0 */
#define USB_OTG_HCCHAR_DAD_1                   ((uint32_t)0x00800000)            /*!<Bit 1 */
#define USB_OTG_HCCHAR_DAD_2                   ((uint32_t)0x01000000)            /*!<Bit 2 */
#define USB_OTG_HCCHAR_DAD_3                   ((uint32_t)0x02000000)            /*!<Bit 3 */
#define USB_OTG_HCCHAR_DAD_4                   ((uint32_t)0x04000000)            /*!<Bit 4 */
#define USB_OTG_HCCHAR_DAD_5                   ((uint32_t)0x08000000)            /*!<Bit 5 */
#define USB_OTG_HCCHAR_DAD_6                   ((uint32_t)0x10000000)            /*!<Bit 6 */
#define USB_OTG_HCCHAR_ODDFRM                  ((uint32_t)0x20000000)            /*!< Odd frame */
#define USB_OTG_HCCHAR_CHDIS                   ((uint32_t)0x40000000)            /*!< Channel disable */
#define USB_OTG_HCCHAR_CHENA                   ((uint32_t)0x80000000)            /*!< Channel enable */

/********************  Bit definition for USB_OTG_HCSPLT register  ********************/

#define USB_OTG_HCSPLT_PRTADDR                 ((uint32_t)0x0000007F)            /*!< Port address */
#define USB_OTG_HCSPLT_PRTADDR_0               ((uint32_t)0x00000001)            /*!<Bit 0 */
#define USB_OTG_HCSPLT_PRTADDR_1               ((uint32_t)0x00000002)            /*!<Bit 1 */
#define USB_OTG_HCSPLT_PRTADDR_2               ((uint32_t)0x00000004)            /*!<Bit 2 */
#define USB_OTG_HCSPLT_PRTADDR_3               ((uint32_t)0x00000008)            /*!<Bit 3 */
#define USB_OTG_HCSPLT_PRTADDR_4               ((uint32_t)0x00000010)            /*!<Bit 4 */
#define USB_OTG_HCSPLT_PRTADDR_5               ((uint32_t)0x00000020)            /*!<Bit 5 */
#define USB_OTG_HCSPLT_PRTADDR_6               ((uint32_t)0x00000040)            /*!<Bit 6 */

#define USB_OTG_HCSPLT_HUBADDR                 ((uint32_t)0x00003F80)            /*!< Hub address */
#define USB_OTG_HCSPLT_HUBADDR_0               ((uint32_t)0x00000080)            /*!<Bit 0 */
#define USB_OTG_HCSPLT_HUBADDR_1               ((uint32_t)0x00000100)            /*!<Bit 1 */
#define USB_OTG_HCSPLT_HUBADDR_2               ((uint32_t)0x00000200)            /*!<Bit 2 */
#define USB_OTG_HCSPLT_HUBADDR_3               ((uint32_t)0x00000400)            /*!<Bit 3 */
#define USB_OTG_HCSPLT_HUBADDR_4               ((uint32_t)0x00000800)            /*!<Bit 4 */
#define USB_OTG_HCSPLT_HUBADDR_5               ((uint32_t)0x00001000)            /*!<Bit 5 */
#define USB_OTG_HCSPLT_HUBADDR_6               ((uint32_t)0x00002000)            /*!<Bit 6 */

#define USB_OTG_HCSPLT_XACTPOS                 ((uint32_t)0x0000C000)            /*!< XACTPOS */
#define USB_OTG_HCSPLT_XACTPOS_0               ((uint32_t)0x00004000)            /*!<Bit 0 */
#define USB_OTG_HCSPLT_XACTPOS_1               ((uint32_t)0x00008000)            /*!<Bit 1 */
#define USB_OTG_HCSPLT_COMPLSPLT               ((uint32_t)0x00010000)            /*!< Do complete split */
#define USB_OTG_HCSPLT_SPLITEN                 ((uint32_t)0x80000000)            /*!< Split enable */

/********************  Bit definition for USB_OTG_HCINT register  ********************/
#define USB_OTG_HCINT_XFRC                    ((uint32_t)0x00000001)            /*!< Transfer completed */
#define USB_OTG_HCINT_CHH                     ((uint32_t)0x00000002)            /*!< Channel halted */
#define USB_OTG_HCINT_AHBERR                  ((uint32_t)0x00000004)            /*!< AHB error */
#define USB_OTG_HCINT_STALL                   ((uint32_t)0x00000008)            /*!< STALL response received interrupt */
#define USB_OTG_HCINT_NAK                     ((uint32_t)0x00000010)            /*!< NAK response received interrupt */
#define USB_OTG_HCINT_ACK                     ((uint32_t)0x00000020)            /*!< ACK response received/transmitted interrupt */
#define USB_OTG_HCINT_NYET                    ((uint32_t)0x00000040)            /*!< Response received interrupt */
#define USB_OTG_HCINT_TXERR                   ((uint32_t)0x00000080)            /*!< Transaction error */
#define USB_OTG_HCINT_BBERR                   ((uint32_t)0x00000100)            /*!< Babble error */
#define USB_OTG_HCINT_FRMOR                   ((uint32_t)0x00000200)            /*!< Frame overrun */
#define USB_OTG_HCINT_DTERR                   ((uint32_t)0x00000400)            /*!< Data toggle error */

/********************  Bit definition for USB_OTG_HCINTMSK register  ********************/
#define USB_OTG_HCINTMSK_XFRCM                   ((uint32_t)0x00000001)            /*!< Transfer completed mask */
#define USB_OTG_HCINTMSK_CHHM                    ((uint32_t)0x00000002)            /*!< Channel halted mask */
#define USB_OTG_HCINTMSK_AHBERR                  ((uint32_t)0x00000004)            /*!< AHB error */
#define USB_OTG_HCINTMSK_STALLM                  ((uint32_t)0x00000008)            /*!< STALL response received interrupt mask */
#define USB_OTG_HCINTMSK_NAKM                    ((uint32_t)0x00000010)            /*!< NAK response received interrupt mask */
#define USB_OTG_HCINTMSK_ACKM                    ((uint32_t)0x00000020)            /*!< ACK response received/transmitted interrupt mask */
#define USB_OTG_HCINTMSK_NYET                    ((uint32_t)0x00000040)            /*!< response received interrupt mask */
#define USB_OTG_HCINTMSK_TXERRM                  ((uint32_t)0x00000080)            /*!< Transaction error mask */
#define USB_OTG_HCINTMSK_BBERRM                  ((uint32_t)0x00000100)            /*!< Babble error mask */
#define USB_OTG_HCINTMSK_FRMORM                  ((uint32_t)0x00000200)            /*!< Frame overrun mask */
#define USB_OTG_HCINTMSK_DTERRM                  ((uint32_t)0x00000400)            /*!< Data toggle error mask */

/********************  Bit definition for USB_OTG_HCTSIZ register  ********************/
#define USB_OTG_HCTSIZ_XFRSIZ                    ((uint32_t)0x0007FFFF)            /*!< Transfer size */
#define USB_OTG_HCTSIZ_PKTCNT                    ((uint32_t)0x1FF80000)            /*!< Packet count */
#define USB_OTG_HCTSIZ_DOPING                    ((uint32_t)0x80000000)            /*!< Do PING */
#define USB_OTG_HCTSIZ_DPID                      ((uint32_t)0x60000000)            /*!< Data PID */
#define USB_OTG_HCTSIZ_DPID_0                    ((uint32_t)0x20000000)            /*!<Bit 0 */
#define USB_OTG_HCTSIZ_DPID_1                    ((uint32_t)0x40000000)            /*!<Bit 1 */

/********************  Bit definition for USB_OTG_HCDMA register  ********************/
#define USB_OTG_HCDMA_DMAADDR                    ((uint32_t)0xFFFFFFFF)            /*!< DMA address */










/**
 * Device Logical IN Endpoint-Specific Registers. <i>Offsets
 * 900h-AFCh</i>
 *
 * There will be one set of endpoint registers per logical endpoint
 * implemented.
 *
 * <i>These registers are visible only in Device mode and must not be
 * accessed in Host mode, as the results are unknown.</i>
 */
struct dwcotg_dev_in_ep_regs_t {
	/** Device IN Endpoint Control Register. <i>Offset:900h +
	 * (ep_num * 20h) + 00h</i> */
	volatile uint32_t diepctl;
	/** Reserved. <i>Offset:900h + (ep_num * 20h) + 04h</i> */
	uint32_t reserved04;
	/** Device IN Endpoint Interrupt Register. <i>Offset:900h +
	 * (ep_num * 20h) + 08h</i> */
	volatile uint32_t diepint;
	/** Reserved. <i>Offset:900h + (ep_num * 20h) + 0Ch</i> */
	uint32_t reserved0C;
	/** Device IN Endpoint Transfer Size
	 * Register. <i>Offset:900h + (ep_num * 20h) + 10h</i> */
	volatile uint32_t dieptsiz;
	/** Device IN Endpoint DMA Address Register. <i>Offset:900h +
	 * (ep_num * 20h) + 14h</i> */
	volatile uint32_t diepdma;
	/** Device IN Endpoint Transmit FIFO Status Register. <i>Offset:900h +
	 * (ep_num * 20h) + 18h</i> */
	volatile uint32_t dtxfsts;
	/** Device IN Endpoint DMA Buffer Register. <i>Offset:900h +
	 * (ep_num * 20h) + 1Ch</i> */
	volatile uint32_t diepdmab;
};

/********************  Bit definition for USB_OTG_DIEPCTL register  ********************/
#define USB_OTG_DIEPCTL_MPSIZ                   ((uint32_t)0x000007FF)            /*!< Maximum packet size              */
#define USB_OTG_DIEPCTL_USBAEP                  ((uint32_t)0x00008000)            /*!< USB active endpoint              */
#define USB_OTG_DIEPCTL_EONUM_DPID              ((uint32_t)0x00010000)            /*!< Even/odd frame                   */
#define USB_OTG_DIEPCTL_NAKSTS                  ((uint32_t)0x00020000)            /*!< NAK status                       */

#define USB_OTG_DIEPCTL_EPTYP                   ((uint32_t)0x000C0000)            /*!< Endpoint type                    */
#define USB_OTG_DIEPCTL_EPTYP_0                 ((uint32_t)0x00040000)            /*!<Bit 0 */
#define USB_OTG_DIEPCTL_EPTYP_1                 ((uint32_t)0x00080000)            /*!<Bit 1 */
#define USB_OTG_DIEPCTL_STALL                   ((uint32_t)0x00200000)            /*!< STALL handshake                  */

#define USB_OTG_DIEPCTL_TXFNUM                  ((uint32_t)0x03C00000)            /*!< TxFIFO number                    */
#define USB_OTG_DIEPCTL_TXFNUM_0                ((uint32_t)0x00400000)            /*!<Bit 0 */
#define USB_OTG_DIEPCTL_TXFNUM_1                ((uint32_t)0x00800000)            /*!<Bit 1 */
#define USB_OTG_DIEPCTL_TXFNUM_2                ((uint32_t)0x01000000)            /*!<Bit 2 */
#define USB_OTG_DIEPCTL_TXFNUM_3                ((uint32_t)0x02000000)            /*!<Bit 3 */
#define USB_OTG_DIEPCTL_CNAK                    ((uint32_t)0x04000000)            /*!< Clear NAK                        */
#define USB_OTG_DIEPCTL_SNAK                    ((uint32_t)0x08000000)            /*!< Set NAK */
#define USB_OTG_DIEPCTL_SD0PID_SEVNFRM          ((uint32_t)0x10000000)            /*!< Set DATA0 PID                    */
#define USB_OTG_DIEPCTL_SODDFRM                 ((uint32_t)0x20000000)            /*!< Set odd frame                    */
#define USB_OTG_DIEPCTL_EPDIS                   ((uint32_t)0x40000000)            /*!< Endpoint disable                 */
#define USB_OTG_DIEPCTL_EPENA                   ((uint32_t)0x80000000)            /*!< Endpoint enable                  */

/********************  Bit definition for USB_OTG_DIEPINT register  ********************/
#define USB_OTG_DIEPINT_XFRC                    ((uint32_t)0x00000001)            /*!< Transfer completed interrupt */
#define USB_OTG_DIEPINT_EPDISD                  ((uint32_t)0x00000002)            /*!< Endpoint disabled interrupt */
#define USB_OTG_DIEPINT_TOC                     ((uint32_t)0x00000008)            /*!< Timeout condition */
#define USB_OTG_DIEPINT_ITTXFE                  ((uint32_t)0x00000010)            /*!< IN token received when TxFIFO is empty */
#define USB_OTG_DIEPINT_INEPNE                  ((uint32_t)0x00000040)            /*!< IN endpoint NAK effective */
#define USB_OTG_DIEPINT_TXFE                    ((uint32_t)0x00000080)            /*!< Transmit FIFO empty */
#define USB_OTG_DIEPINT_TXFIFOUDRN              ((uint32_t)0x00000100)            /*!< Transmit Fifo Underrun */
#define USB_OTG_DIEPINT_BNA                     ((uint32_t)0x00000200)            /*!< Buffer not available interrupt */
#define USB_OTG_DIEPINT_PKTDRPSTS               ((uint32_t)0x00000800)            /*!< Packet dropped status */
#define USB_OTG_DIEPINT_BERR                    ((uint32_t)0x00001000)            /*!< Babble error interrupt */
#define USB_OTG_DIEPINT_NAK                     ((uint32_t)0x00002000)            /*!< NAK interrupt */

/********************  Bit definition for USB_OTG_DIEPTSIZ register  ********************/
#define USB_OTG_DIEPTSIZ_XFRSIZ                  ((uint32_t)0x0007FFFF)            /*!< Transfer size */
#define USB_OTG_DIEPTSIZ_PKTCNT                  ((uint32_t)0x1FF80000)            /*!< Packet count */
#define USB_OTG_DIEPTSIZ_MULCNT                  ((uint32_t)0x60000000)            /*!< Packet count */

/********************  Bit definition for USB_OTG_DTXFSTS register  ********************/
#define USB_OTG_DTXFSTS_INEPTFSAV                ((uint32_t)0x0000FFFF)            /*!< IN endpoint TxFIFO space available */


/**
 * Device Logical OUT Endpoint-Specific Registers. <i>Offsets:
 * B00h-CFCh</i>
 *
 * There will be one set of endpoint registers per logical endpoint
 * implemented.
 *
 * <i>These registers are visible only in Device mode and must not be
 * accessed in Host mode, as the results are unknown.</i>
 */
struct dwcotg_dev_out_ep_regs_t {
	/** Device OUT Endpoint Control Register. <i>Offset:B00h +
	 * (ep_num * 20h) + 00h</i> */
	volatile uint32_t doepctl;
	/** Reserved. <i>Offset:B00h + (ep_num * 20h) + 04h</i> */
	uint32_t reserved04;
	/** Device OUT Endpoint Interrupt Register. <i>Offset:B00h +
	 * (ep_num * 20h) + 08h</i> */
	volatile uint32_t doepint;
	/** Reserved. <i>Offset:B00h + (ep_num * 20h) + 0Ch</i> */
	uint32_t reserved0C;
	/** Device OUT Endpoint Transfer Size Register. <i>Offset:
	 * B00h + (ep_num * 20h) + 10h</i> */
	volatile uint32_t doeptsiz;
	/** Device OUT Endpoint DMA Address Register. <i>Offset:B00h
	 * + (ep_num * 20h) + 14h</i> */
	volatile uint32_t doepdma;
	/** Reserved. <i>Offset:B00h + 	 * (ep_num * 20h) + 18h</i> */
	uint32_t unused;
	/** Device OUT Endpoint DMA Buffer Register. <i>Offset:B00h
	 * + (ep_num * 20h) + 1Ch</i> */
	uint32_t doepdmab;
};

/********************  Bit definition for USB_OTG_DOEPCTL register  ********************/
#define USB_OTG_DOEPCTL_MPSIZ                     ((uint32_t)0x000007FF)            /*!< Maximum packet size */          /*!<Bit 1 */
#define USB_OTG_DOEPCTL_USBAEP                    ((uint32_t)0x00008000)            /*!< USB active endpoint */
#define USB_OTG_DOEPCTL_NAKSTS                    ((uint32_t)0x00020000)            /*!< NAK status */
#define USB_OTG_DOEPCTL_SD0PID_SEVNFRM            ((uint32_t)0x10000000)            /*!< Set DATA0 PID */
#define USB_OTG_DOEPCTL_SODDFRM                   ((uint32_t)0x20000000)            /*!< Set odd frame */
#define USB_OTG_DOEPCTL_EPTYP                     ((uint32_t)0x000C0000)            /*!< Endpoint type */
#define USB_OTG_DOEPCTL_EPTYP_0                   ((uint32_t)0x00040000)            /*!<Bit 0 */
#define USB_OTG_DOEPCTL_EPTYP_1                   ((uint32_t)0x00080000)            /*!<Bit 1 */
#define USB_OTG_DOEPCTL_SNPM                      ((uint32_t)0x00100000)            /*!< Snoop mode */
#define USB_OTG_DOEPCTL_STALL                     ((uint32_t)0x00200000)            /*!< STALL handshake */
#define USB_OTG_DOEPCTL_CNAK                      ((uint32_t)0x04000000)            /*!< Clear NAK */
#define USB_OTG_DOEPCTL_SNAK                      ((uint32_t)0x08000000)            /*!< Set NAK */
#define USB_OTG_DOEPCTL_EPDIS                     ((uint32_t)0x40000000)            /*!< Endpoint disable */
#define USB_OTG_DOEPCTL_EPENA                     ((uint32_t)0x80000000)            /*!< Endpoint enable */

/********************  Bit definition for USB_OTG_DOEPINT register  ********************/
#define USB_OTG_DOEPINT_XFRC                    ((uint32_t)0x00000001)            /*!< Transfer completed interrupt */
#define USB_OTG_DOEPINT_EPDISD                  ((uint32_t)0x00000002)            /*!< Endpoint disabled interrupt */
#define USB_OTG_DOEPINT_STUP                    ((uint32_t)0x00000008)            /*!< SETUP phase done */
#define USB_OTG_DOEPINT_OTEPDIS                 ((uint32_t)0x00000010)            /*!< OUT token received when endpoint disabled */
#define USB_OTG_DOEPINT_STSPHSERCVD             ((uint32_t)0x00000020)            /*!< Status Phase Received For Control Write */
#define USB_OTG_DOEPINT_B2BSTUP                 ((uint32_t)0x00000040)            /*!< Back-to-back SETUP packets received */
#define USB_OTG_DOEPINT_NYET                    ((uint32_t)0x00004000)            /*!< NYET interrupt */

/********************  Bit definition for USB_OTG_DOEPTSIZ register  ********************/
#define USB_OTG_DOEPTSIZ_XFRSIZ                  ((uint32_t)0x0007FFFF)            /*!< Transfer size */
#define USB_OTG_DOEPTSIZ_PKTCNT                  ((uint32_t)0x1FF80000)            /*!< Packet count */

#define USB_OTG_DOEPTSIZ_STUPCNT                 ((uint32_t)0x60000000)            /*!< SETUP packet count */
#define USB_OTG_DOEPTSIZ_STUPCNT_0               ((uint32_t)0x20000000)            /*!<Bit 0 */
#define USB_OTG_DOEPTSIZ_STUPCNT_1               ((uint32_t)0x40000000)            /*!<Bit 1 */

/** OTG Power and Clock Gating Control Register */
/********************  Bit definition for PCGCCTL register  ********************/
#define USB_OTG_PCGCCTL_STOPCLK                 ((uint32_t)0x00000001)            /*!< SETUP packet count */
#define USB_OTG_PCGCCTL_GATECLK                 ((uint32_t)0x00000002)            /*!<Bit 0 */
#define USB_OTG_PCGCCTL_PHYSUSP                 ((uint32_t)0x00000010)            /*!<Bit 1 */

#endif // __DWCOTG_REGS_H__
