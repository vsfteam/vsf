/**
	*****************************************************************************
	* @file     cmem7_usb.h
	*
	* @brief    CMEM7 USB header file
	*
	*
	* @version  V1.0
	* @date     3. September 2013
	*
	* @note               
	*           
	*****************************************************************************
	* @attention
	*
	* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
	* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
	* TIME. AS A RESULT, CAPITAL-MICRO SHALL NOT BE HELD LIABLE FOR ANY DIRECT, 
	* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
	* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
	* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
	*
	* <h2><center>&copy; COPYRIGHT 2013 Capital-micro </center></h2>
	*****************************************************************************
	*/

#ifndef __CMEM7_USB_H
#define __CMEM7_USB_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "cmem7.h"
#include "cmem7_conf.h"
#include "string.h"

/**
 *
 */
#define SET_HCDMA_DESC_ADDR(a)		(((uint32_t)(a)) >> 9)
#define MIN(a, b)					(((a) <= (b)) ? (a) : (b))
#define MAX(a, b)					(((a) >= (b)) ? (a) : (b))
#define BIT(b)						(0x1u << (b))

/** @defgroup USB_HOST_PID
  * @{
  */
#define USB_HOST_PID_DATA0			0x0				/*!< Indicates the Data PID is DATA0                                       */
#define USB_HOST_PID_DATA2			0x1				/*!< Indicates the Data PID is DATA2                                       */
#define USB_HOST_PID_DATA1			0x2				/*!< Indicates the Data PID is DATA1                                       */
#define USB_HOST_PID_MDATA			0x3				/*!< Indicates the Data PID is MDATA (non-control)                         */
#define USB_HOST_PID_SETUP			0x3				/*!< Indicates the Data PID is SETUP (control)                             */
/**
  * @}
  */

/** @defgroup USB_EP_TYPE
  * @{
  */
typedef enum {
	USB_EP_TYPE_CONTROL =			0x0,			/*!< Control                                                               */
	USB_EP_TYPE_ISO =				0x1,			/*!< Isochronous                                                           */
	USB_EP_TYPE_BULK =				0x2,			/*!< Bulk                                                                  */
	USB_EP_TYPE_INT =				0x3,			/*!< Interrupt                                                             */
} USB_EP_TYPE;
/**
  * @}
  */

/** @defgroup USB_ENUM_SPEED
  * @{
  */
typedef enum {
	USB_ENUM_SPEED_HS =				0x0,			/*!< Enumerated Speed is High Speed                                        */
	USB_ENUM_SPEED_FS =				0x1,			/*!< Enumerated Speed is Full Speed                                        */
	USB_ENUM_SPEED_LS =				0x2,			/*!< Enumerated Speed is Low  Speed                                        */
	USB_ENUM_SPEED_FS_48M =			0x3,			/*!< Enumerated Speed is Full Speed (PHY clock is running at 48MHz)        */
} USB_ENUM_SPEED;
/**
  * @}
  */

/** @defgroup USB_INT_GP
  * @{
  */
typedef enum {
	USB_INT_GP_HOST_DISC,							/*!< Device disconnection interrupt (Only for HOST Mode)                   */
	USB_INT_GP_DEV_RESET,							/*!< USB Port Reset Interrupt (Only for DEVICE Mode)                       */
	USB_INT_GP_DEV_ENUMDONE,						/*!< Enumeration Done Interrupt (Only for DEVICE Mode)                     */
	USB_INT_GP_DEV_SUSP,							/*!< USB Suspend Interrupt (Only for DEVICE Mode)                          */
	USB_INT_GP_DEV_EARLY,							/*!< USB Idle Interrupt (Only for DEVICE Mode)                             */
	USB_INT_GP_SOF,									/*!< SOF Interrupt                                                         */
	USB_INT_GP_MIS,									/*!< USB access overstep the boundary Interrupt                            */
	USB_INT_GP_IDCHG,								/*!< OTG Connector ID Status Change Interrupt                              */
	USB_INT_GP_SESSREQ,								/*!< Session Request / Create Interrupt                                    */
	USB_INT_GP_RXFIFO,								/*!< RxFIFO Non-Empty                                                      */
} USB_INT_GP;
/**
  * @}
  */

/** @defgroup USB_INT_OTG
  * @{
  */
typedef enum {
	USB_INT_OTG_SESEND,								/*!< Session End Interrupt                                                 */
	USB_INT_OTG_STANDAUP,							/*!< B Device timeout to connect Interrupt                                 */
	USB_INT_OTG_HNDETECT,							/*!< Host Negotiation Detected Interrupt                                   */
	USB_INT_OTG_HNSUCCHG,							/*!< Host Negotiation Success Status Change Interrupt                      */
	USB_INT_OTG_KEEPAPP,							/*!< Debounce Done Interrupt (Only for HOST Mode)                          */
} USB_INT_OTG;
/**
  * @}
  */

/** @defgroup USB_OTG_CTL
  * @{
  */
typedef enum {
	USB_OTG_DEV_HNSUCC = 8,							/*!< Host Negotiation Success (Only for DEVICE Mode, Read Only)            */
	USB_OTG_DEV_HNPREQ = 9,							/*!< HNP Request (Only for DEVICE Mode)                                    */
	USB_OTG_HST_HNPENABLE = 10,						/*!< Host Set HNP Enable (Only for HOST Mode)                              */
	USB_OTG_DEV_HNPENABLE = 11,						/*!< Device HNP Enabled (Only for DEVICE Mode)                             */
} USB_OTG_CTL;

/**
  * @}
  */

typedef union {
    __IO uint32_t  HPRT;                            /*!< Host Port Control and Status Register                                 */
    
    struct {
      __I  uint32_t  PCS        :  1;               /*!< If a device is attached to the port                                   */
      __IO uint32_t  PCD        :  1;               /*!< A device connection is detected                                       */
      __IO uint32_t  PE         :  1;               /*!< Port Enable                                                           */
      __IO uint32_t  PEDC       :  1;               /*!< Set if when the status of the Port Enable (bit 2) of this register
                                                         changes                                                               */
      __I  uint32_t  POA        :  1;               /*!< Indicates the overcurrent condition of the port                       */
      __IO uint32_t  POC        :  1;               /*!< Set if when the status of the Port Overcurrent Active bit (bit
                                                         4) in this register changes                                           */
      __IO uint32_t  PR         :  1;               /*!< Application and Core all can perform resume by setting, then
                                                         clear it whatever resume is success or not                            */
      __IO uint32_t  PS         :  1;               /*!< Sets this bit to put this port in Suspend mode                        */
      __IO uint32_t  PRESET     :  1;               /*!< sets this bit, a reset sequence is started on this port               */
           uint32_t             :  1;
      __IO uint32_t  PLSDP      :  1;               /*!< Logic level of D+                                                     */
      __IO uint32_t  PLSDN      :  1;               /*!< Logic level of D-                                                     */
      __IO uint32_t  PP         :  1;               /*!< this field to control power to this port. 1, power on                 */
      __IO uint32_t  PTC        :  4;               /*!< The application writes a nonzero value to this field to put
                                                         the port into a Test mode                                             */
      __I  uint32_t  SPEED      :  2;               /*!< Indicates the speed of the device attached to this port               */
    } HPRT_b;                                       /*!< BitSize                                                               */
} USB_REG_HPRT;

typedef struct {
  union {
    uint32_t VALUE;                                 /*!< provide the status of the buffer                                            */
    struct {
      __IO uint32_t  SIZE       : 17;               /*!< Total bytes to transfer for OUT; the expected transfer size for IN    */
      __IO uint32_t  AQTD       :  6;               /*!< IN Only, Alternated Queue Transfer Descriptor Valid                   */
      __IO uint32_t  AQTD_VLD   :  1;               /*!< IN Only, Alternated Queue Transfer Descriptor Valid                   */
      __IO uint32_t  SUP        :  1;               /*!< OUT Only, it indicates that the buffer data pointed by this descriptor
                                                       is a setup packet of 8 bytes                                            */
      __IO uint32_t  IOC        :  1;               /*!< It indicates that that the core must generate a XferCompl interrupt   */
      __IO uint32_t  EOL        :  1;               /*!< It indicates that this is the last descriptor in the list             */
           uint32_t             :  1;
      __IO uint32_t  STS        :  2;               /*!< The status of the rx/tx data:
                                                       00=Success; 01=PKTERR; 10=Reserved; 11=Reserved                         */
           uint32_t             :  1;
      __IO uint32_t  A          :  1;               /*!< Active: 0=descriptor is not ready; 1=descriptor is ready              */
    } HOST_NISO_b;                                  /*!< BitSize                                                               */
    struct {
      __IO uint32_t  SIZE       : 16;               /*!< Total bytes to transfer for OUT; the expected transfer size for IN    */
           uint32_t             :  7;
      __IO uint32_t  MTRF       :  1;               /*!< IN Only, Alternated Queue Transfer Descriptor Valid                   */
      __IO uint32_t  SR         :  1;               /*!< IN Only, Alternated Queue Transfer Descriptor Valid                   */
      __IO uint32_t  IOC        :  1;               /*!< It indicates that that the core must generate a XferCompl interrupt   */
      __IO uint32_t  SP         :  1;               /*!< It indicates that this is the last descriptor in the list             */
      __IO uint32_t  L          :  1;               /*!< It indicates that this is the last descriptor in the list             */
      __IO uint32_t  STS        :  2;               /*!< The status of the rx/tx data:
                                                       00=Success; 01=PKTERR; 10=Reserved; 11=Reserved                         */
      __IO uint32_t  BS         :  2;               /*!< Active: 0=descriptor is not ready; 1=descriptor is ready              */
    } DEV_NISO_OUT_b;                               /*!< BitSize                                                               */
    struct {
      __IO uint32_t  SIZE       : 16;               /*!< Total bytes to transfer for OUT; the expected transfer size for IN    */
           uint32_t             :  7;
      __IO uint32_t  PID        :  2;               /*!< IN Only, Alternated Queue Transfer Descriptor Valid                   */
      __IO uint32_t  IOC        :  1;               /*!< It indicates that that the core must generate a XferCompl interrupt   */
      __IO uint32_t  SP         :  1;               /*!< It indicates that this is the last descriptor in the list             */
      __IO uint32_t  L          :  1;               /*!< It indicates that this is the last descriptor in the list             */
      __IO uint32_t  STS        :  2;               /*!< The status of the rx/tx data:
                                                       00=Success; 01=PKTERR; 10=Reserved; 11=Reserved                         */
      __IO uint32_t  BS         :  2;               /*!< Active: 0=descriptor is not ready; 1=descriptor is ready              */
    } DEV_NISO_IN_b;                                /*!< BitSize                                                               */
    struct {
      __IO uint32_t  SIZE       : 11;               /*!< Total bytes to transfer for OUT; the expected transfer size for IN    */
           uint32_t             :  1;
      __IO uint32_t  FRAMENUM   : 11;               /*!< [7:0][2:0] is for frame and uframe number                             */
      __IO uint32_t  PID        :  2;               /*!< IN Only, Alternated Queue Transfer Descriptor Valid                   */
      __IO uint32_t  IOC        :  1;               /*!< It indicates that that the core must generate a XferCompl interrupt   */
      __IO uint32_t  SP         :  1;               /*!< It indicates that this is the last descriptor in the list             */
      __IO uint32_t  L          :  1;               /*!< It indicates that this is the last descriptor in the list             */
      __IO uint32_t  STS        :  2;               /*!< The status of the rx/tx data:
                                                       00=Success; 01=PKTERR; 10=Reserved; 11=Reserved                         */
      __IO uint32_t  BS         :  2;               /*!< Active: 0=descriptor is not ready; 1=descriptor is ready              */
    } DEV_ISO_OUT_b;                                /*!< BitSize                                                               */
    struct {
      __IO uint32_t  SIZE       : 12;               /*!< Total bytes to transfer for OUT; the expected transfer size for IN    */
      __IO uint32_t  FRAMENUM   : 11;               /*!< [7:0][2:0] is for frame and uframe number                             */
      __IO uint32_t  PID        :  2;               /*!< IN Only, Alternated Queue Transfer Descriptor Valid                   */
      __IO uint32_t  IOC        :  1;               /*!< It indicates that that the core must generate a XferCompl interrupt   */
      __IO uint32_t  SP         :  1;               /*!< It indicates that this is the last descriptor in the list             */
      __IO uint32_t  L          :  1;               /*!< It indicates that this is the last descriptor in the list             */
      __IO uint32_t  STS        :  2;               /*!< The status of the rx/tx data:
                                                       00=Success; 01=PKTERR; 10=Reserved; 11=Reserved                         */
      __IO uint32_t  BS         :  2;               /*!< Active: 0=descriptor is not ready; 1=descriptor is ready              */
    } DEV_ISO_IN_b;                                	/*!< BitSize                                                               */
  } QUADLET;
  uint32_t  BUIFFER;                                /*!< points to a data buffer                                               */
} OTG_DESCRIPTOR;

#define PTXFSIZ(n)				(*((volatile unsigned int *)(0x41300100 + (n) * 0x4)))	// 0 means HOST, otherwise means Device DIEPTXFn
#define OTG_NUM_IN_EPS			4	// Number of Device Mode IN Endpoints Including Control Endpoints (INEps in GHWCFG4)
#define OTG_NUM_PERIO_EPS		0	// Number of Device Mode Periodic IN Endpoints (NumDevPerioEps in GHWCFG4)
#define OTG_DFIFO_DEPTH			2080	// Number of Device Mode Periodic IN Endpoints (NumDevPerioEps in GHWCFG4)

/**
 * @brief Get OTG Connector ID Status (Is B-Device)
 * @retval BOOL TRUE: High; FALSE: Low
   */
BOOL USB_ogtIsBdevID(void);

/**
 * @brief Set USB Global Interrupt Enable
   * @param[in] enable TRUE: Enable; FALSE: Disable
 * @retval void
   */
void USB_EnableInt(BOOL enable);

/**
 * @brief Flush TX/RX FIFO
   * @param[in] num Flush FIFO£¬0: non-periodic TX FIFO (HOST Mode) or FIFO 0 (DEVICE Mode); 1: Periodic TX FIFO (HOST Mode) or FIFO 1 (DEVICE Mode); 2-15: FIFO n in DEVICE Mode; 16: Flush All TX FIFO; otherwise: Flush RX FIFO
 * @retval void
   */
void USB_FlushFIFO(uint32_t num);

/**
 * @brief Initialize USB
   * @param[in] type USB Mode, Bit0: Force HOST Mode; Bit1: Force DEVICE Mode; Bit4: Enable SRP; Bit5: Enable HNP
 * @retval void
   */
void USB_coreInit(uint32_t type);

/**
 * @brief Get USB Mode (Is Host Mode?)
 * @retval BOOL TRUE: HOST Mode; FALSE: DEVICE Mode
   */
BOOL USB_roleIsHost(void);

/**
 * @brief Control and get VBus Status (Only for HOST Mode)
   * @param[in] opt Bit1: Set VBus using Bit0; Bit0: Turn VBus On or Off
 * @retval BOOL TRUE: VBus is on; FALSE£ºVBus is off
 * @note It cannot control VBus actually due to HW problem
   */
BOOL USB_hostVBus(uint32_t opt);

/**
 * @brief Initialize USB HOST Module (Only for HOST Mode)
 * @retval void
   */
void USB_hostInit(void);

/**
 * @brief Set USB Port Reset Status (Only for HOST Mode)
   * @param[in] rst TRUE: Port is reseting; FALSE: Port stop reseting
 * @retval void
   */
void USB_HostResetPort(BOOL rst);

/**
 * @brief Frame Number of the next SOF (Only for HOST Mode)
 * @retval uint16_t Frame Number of the next SOF will be send
   */
uint16_t USB_HostGetCurFrame(void);

/**
 * @brief Suspend USP Port (Only for HOST Mode)
 * @retval void
   */
void USB_HostSuspendPort(void);

/**
 * @brief Get the device enumerated speed (Only for HOST Mode)
 * @retval USB_ENUM_SPEED A value of @ref USB_ENUM_SPEED defined
   */
USB_ENUM_SPEED USB_hostGetEnumSpd(void);

/**
 * @brief Get USB Port Connection Status (Only for HOST Mode)
 * @retval BOOL TRUE: A device is attached; FALSE: No device is attached
   */
BOOL USB_hostPrtConn(void);

/**
 * @brief Channel 0 Transaction (Only for HOST Mode)
   * @param[in] devaddr USB Device Address
   * @param[in] desc A pointer to DMA Descriptor (512-byte alignment)
   * @param[in] ctd In terms of number of DMA descriptors (0 means start from 1st DMA descriptor)
   * @param[in] ntd Number of Transfer Descriptors (from 0 to 63 which means from 1 to 64 descriptors)
   * @param[in] ping TRUE: Do PING protocol (Only for OUT Transfer); Must be 0 for IN Transfer
   * @param[in] pid PID: A value of @ref USB_HOST_PID defined: USB_HOST_PID_DATA0, USB_HOST_PID_DATA2, USB_HOST_PID_DATA1, USB_HOST_PID_MDATA or USB_HOST_PID_SETUP
   * @param[in] mps Maximum Packet Size (in bytes)
   * @param[in] epnum Endpoint Address
   * @param[in] in Is IN Transfer, TRUE: IN Transfer; FALSE: OUT Transfer
   * @param[in] eptype Endpoint Type, A value of @ref USB_EP_TYPE defined
   * @param[in] speed Device Speed, A value of @ref USB_ENUM_SPEED defined
   */
void USB_hostCH0(uint32_t devaddr, OTG_DESCRIPTOR *desc, uint32_t ctd, uint32_t ntd, BOOL ping, uint32_t pid, uint32_t mps, uint32_t epnum, BOOL in, USB_EP_TYPE eptype, USB_ENUM_SPEED speed);

/**
 * @brief Channel 1 Transaction (Only for HOST Mode)
 * @see USB_hostCH0
 */
void USB_hostCH1(uint32_t devaddr, OTG_DESCRIPTOR *desc, uint32_t ctd, uint32_t ntd, BOOL ping, uint32_t pid, uint32_t mps, uint32_t epnum, BOOL in, USB_EP_TYPE eptype, USB_ENUM_SPEED speed);

/**
 * @brief Channel 2 Transaction (Only for HOST Mode)
 * @see USB_hostCH0
 */
void USB_hostCH2(uint32_t devaddr, OTG_DESCRIPTOR *desc, uint32_t ctd, uint32_t ntd, BOOL ping, uint32_t pid, uint32_t mps, uint32_t epnum, BOOL in, USB_EP_TYPE eptype, USB_ENUM_SPEED speed);

/**
 * @brief Channel n Transaction (Only for HOST Mode)
   * @param[in] ch Channel number will be used
   * @param[in] devaddr USB Device Address
   * @param[in] desc A pointer to DMA Descriptor (512-byte alignment)
   * @param[in] ctd In terms of number of DMA descriptors (0 means start from 1st DMA descriptor)
   * @param[in] ntd Number of Transfer Descriptors (from 0 to 63 which means from 1 to 64 descriptors)
   * @param[in] ping TRUE: Do PING protocol (Only for OUT Transfer); Must be 0 for IN Transfer
   * @param[in] pid PID: A value of @ref USB_HOST_PID defined: USB_HOST_PID_DATA0, USB_HOST_PID_DATA2, USB_HOST_PID_DATA1, USB_HOST_PID_MDATA or USB_HOST_PID_SETUP
   * @param[in] mps Maximum Packet Size (in bytes)
   * @param[in] epnum Endpoint Address
   * @param[in] in Is IN Transfer, TRUE: IN Transfer; FALSE: OUT Transfer
   * @param[in] eptype Endpoint Type, A value of @ref USB_EP_TYPE defined
   * @param[in] speed Device Speed, A value of @ref USB_ENUM_SPEED defined
 * @retval int returns 0 if success, otherwise returns a negative value
   */
int USB_hostCHn(uint32_t ch, uint32_t devaddr, OTG_DESCRIPTOR *desc, uint32_t ctd, uint32_t ntd, BOOL ping, uint32_t pid, uint32_t mps, uint32_t epnum, BOOL in, USB_EP_TYPE eptype, USB_ENUM_SPEED speed);

/**
 * @brief Halt Channel n (Only for HOST Mode)
   * @param[in] ch Channel number will be halted
 * @retval int returns 0 if success, otherwise returns a negative value
   */
int USB_hostCHnHalt(uint32_t ch);

/**
 * @brief Disable USB Port (Only for HOST Mode)
   * @param[in] dis TRUE: Disable USB Port; FALSE: Do NOT Disable USB Port
 * @retval BOOL If USB Port is disabled, TRUE: USB Port Disabled; FALSE: USB Port Enabled
 * @note It CANNOT Enable USB Port; Port will be enabled automatically after the port was reset successfully.
   */
BOOL USB_hostPortDisable(BOOL dis);

/**
 * @brief Enable Connection Interrupt (Only for HOST Mode)
   * @param[in] en TRUE: Enable Interrupt; FALSE: Mask the Interrupt
 * @retval void
   */
void USB_hostINT_enConn(BOOL en);

/**
 * @brief Connection Interrupt Asserted (Only for HOST Mode)
 * @retval BOOL TRUE: Interrupt Asserted; FALSE: Interrupt is NOT asserted
 * @note PCD or PEDC asserted, please call USB_hostINT_clrPCD or USB_hostINT_clrPEDC function to clear it.
   */
BOOL USB_hostINT_isConn(void);

/**
 * @brief Port Connection Detected (PCD) Interrupt Asserted (Only for HOST Mode)
 * @retval BOOL TRUE: Interrupt Asserted; FALSE: Interrupt is NOT asserted
   */
BOOL USB_hostINT_isPCD(void);

/**
 * @brief Clear Port Connection Detected (PCD) Interrupt Flag (Only for HOST Mode)
 * @retval void
   */
void USB_hostINT_clrPCD(void);

/**
 * @brief Port Enable/Disable Change£¨PEDC£©Interrupt Asserted (Only for HOST Mode)
 * @retval BOOL TRUE: Interrupt Asserted; FALSE: Interrupt is NOT asserted
   */
BOOL USB_hostINT_isPEDC(void);

/**
 * @brief Clear Port Enable/Disable Change£¨PEDC£©Interrupt Flag (Only for HOST Mode)
 * @retval void
   */
void USB_hostINT_clrPEDC(void);

/**
 * @brief Enable Transaction Done Interrupt (Only for HOST Mode)
   * @param[in] ch Channel to enable
 * @para in en TRUE: Enable Interrupt; FALSE: Mask the Interrupt
 * @retval int returns 0 if success, otherwise returns a negative value
   */
int USB_hostINT_enDone(uint32_t ch, BOOL en);

/**
 * @brief Transaction Done Interrupt Asserted (Only for HOST Mode)
   * @param[in] ch Channel to check
 * @retval uint32_t result, Bit0: Done; Bit1: Buffer Not Available Error; Bit2: Channel Idle; Bit3: Transaction Error
   */
uint32_t USB_hostINT_isDone(uint32_t ch);

/**
 * @brief Transaction Done Interrupt Asserted (Only for HOST Mode)
   * @param[in] ch Channel to check
 * @retval uint32_t result, 0: No interrupt is pendding
   */
uint32_t USB_hostINT_isPend(uint32_t ch);

/**
 * @brief Initialize USB DEVICE Module (Only for DEVICE Mode)
 * @retval void
   */
void USB_devInit(void);

/**
 * @brief Get the device enumerated speed (Only for DEVICE Mode)
 * @retval USB_ENUM_SPEED A value of @ref USB_ENUM_SPEED defined
   */
USB_ENUM_SPEED USB_devGetEnumSpd(void);

/**
 * @brief Set Device Address (Only for DEVICE Mode)
   * @param[in] Device Address
 * @retval void
   */
void USB_devSetAddress(uint32_t addr);


/**
 * @brief Endpoint 0 OUT Transation (Only for DEVICE Mode)
   * @param[in] size Transation length (in bytes)
   * @param[in] pktcnt Packet Count
   * @param[in] stpcnt The number of back-to-back SETUP data packets the endpoint can receive. (0 - 3)
   * @param[in] desc A pointer to DMA descriptors
   * @param[in] snoop Snoop Mode, TRUE: Enable Snoop Mode, which means it does not check if the OUT packets are correct before transferring them to application memory; FLASE: Disable Snoop Mode
 * @retval void
   */
void USB_devEP0out(uint32_t size, uint32_t pktcnt, uint32_t stpcnt, void *desc, BOOL snoop);

/**
 * @brief Endpoint 0 IN Transation (Only for DEVICE Mode)
   * @param[in] size Transation length (in bytes)
   * @param[in] pktcnt Packet Count
   * @param[in] desc A pointer to DMA descriptors
   * @param[in] mps Maximum Packet Size (in bytes), only 8,, 32, 64 is valid
 * @retval BOOL TRUE: Parameter mps is valid; FLASE: Parameter mps is invalid
   */
BOOL USB_devEP0in(uint32_t size, uint32_t pktcnt, void *desc, uint32_t mps/*8,16,32,64-byte*/);

/**
 * @brief Active Endpoint 1 (Only for DEVICE Mode)
   * @param[in] in Endpoint direction, TRUE: IN; FALSE: OUT
   * @param[in] mps Maximum Packet Size (in bytes), 0 means inactive the endpoint
   * @param[in] type Endpoint type, A value of @ref USB_EP_TYPE defined
 * @retval void
 * @note All Endpoint but EP0 will be inactived after USB Port reseted
   */
void USB_devActEP1(const BOOL in, const uint32_t mps, USB_EP_TYPE type);

/**
 * @brief Active Endpoint 2 (Only for DEVICE Mode)
 * @see USB_devActEP1
 */
void USB_devActEP2(const BOOL in, const uint32_t mps, USB_EP_TYPE type);

/**
 * @brief Endpoint 1 IN Transation (Only for DEVICE Mode)
   * @param[in] size Maximum Packet Size (in bytes)
   * @param[in] pktcnt Packet Count
   * @param[in] pid PID (only for interrupt/bulk), 0x1 means DATA0; 0x2 means DATA1
   * @param[in] desc A pointer to DMA descriptors
 * @retval void
 * @see
 */
void USB_devEP1in(uint32_t size, uint32_t pktcnt, uint32_t pid, void *desc);

/**
 * @brief Endpoint 1 OUT Transation (Only for DEVICE Mode)
   * @param[in] size Data length (in bytes)
   * @param[in] pktcnt Packet Count
   * @param[in] PID (only for interrupt/bulk), 0x1 means DATA0; 0x2 means DATA1
   * @param[in] stpcnt The number of back-to-back SETUP data packets the endpoint can receive. (0 - 3)
   * @param[in] desc A pointer to DMA descriptors
   * @param[in] snoop Snoop Mode, TRUE: Enable Snoop Mode, which means it does not check if the OUT packets are correct before transferring them to application memory; FLASE: Disable Snoop Mode
 * @retval void
   */
void USB_devEP1out(uint32_t size, uint32_t pktcnt, uint32_t pid, uint32_t stpcnt, void *desc, BOOL snoop);

/**
 * @brief Endpoint 2 IN Transation (Only for DEVICE Mode)
 * @see USB_devEP1in
 */
void USB_devEP2in(uint32_t size, uint32_t pktcnt, uint32_t pid, void *desc);

/**
 * @brief Endpoint 2 OUT Transation (Only for DEVICE Mode)
 * @see USB_devEP1out
 */
void USB_devEP2out(uint32_t size, uint32_t pktcnt, uint32_t pid, uint32_t stpcnt, void *desc, BOOL snoop);

/**
 * @brief Get EP if it is available (Only for DEVICE Mode)
   * @param[in] ep Endpoint
   * @param[in] in Endpoint Direction, TRUE: IN; FALSE: OUT
 * @retval uint32_t result, 0: Inactive, 1: Busy, 3: Idle
   */
uint32_t USB_devEPnAvail(uint32_t ep, BOOL in);

/**
 * @brief Set NAK handshake (Only for DEVICE Mode)
   * @param[in] ep Endpoint
   * @param[in] in Endpoint Direction, TRUE: IN; FALSE: OUT
   * @param[in] en TRUE: Enable NAK handshake; FALSE: Disable NAK handshake
 * @retval void
   */
void USB_devNAKhandshake(uint32_t ep, BOOL in, BOOL en);

/**
 * @brief Set STALL handshake (Only for DEVICE Mode)
   * @param[in] ep Endpoint
   * @param[in] in Endpoint Direction, TRUE: IN; FALSE: OUT
   * @param[in] en TRUE: Enable STALL handshake; FALSE: Disable STALL handshake
 * @retval BOOL Return the old status before en is set.
 * @note Not for ISO Endpoint; For Endpoint 0 it clears itself when a SETUP token is received.
   */
BOOL USB_devSTALLhandshake(uint32_t ep, BOOL in, BOOL en);

/**
 * @brief Enable Transaction Done Interrupt (Only for DEVICE Mode)
   * @param[in] ep Endpoint
   * @param[in] in Endpoint Direction, TRUE: IN; FALSE: OUT
 * @para in en TRUE: Enable Interrupt; FALSE: Mask the Interrupt
 * @retval void
   */
void USB_devINT_enDone(uint32_t ep, BOOL in, BOOL en);

/**
 * @brief Transaction Done Interrupt Asserted (Only for DEVICE Mode)
   * @param[in] ep Endpoint
   * @param[in] in Endpoint Direction, TRUE: IN; FALSE: OUT
 * @retval uint32_t Result, Bit0: Done; Bit1£ºBuffer Not Available Error; Bit2: SETUP Phase Done
   */
uint32_t USB_devINT_isDone(uint32_t ep, BOOL in);

/**
 * @brief get global IEP_INT status
 * @retval uint32_t USB->GINTSTS_b.IEP_INT
   */
uint32_t USB_GetIEP(void);

/**
 * @brief get global OEP_INT status
 * @retval uint32_t USB->GINTSTS_b.OEP_INT
   */
uint32_t USB_GetOEP(void);

/**
 * @brief get channel which int occurs
 * @retval uint32_t USB->DAINT
   */
uint32_t USB_GetIntChannel(void);

/**
 * @brief Enable OTG Interrupt (Only for DEVICE Mode)
 * @para in en TRUE: Enable Interrupt; FALSE: Mask the Interrupt
 * @retval void
   */
void USB_INT_enOTG(BOOL en);

/**
 * @brief OTG Interrupt Asserted
 * @retval BOOL TRUE: Interrupt Asserted; FALSE: Interrupt is NOT asserted
   */
BOOL USB_INT_isOTG(void);

/**
 * @brief OTG Function Interrupt Asserted
   * @param[in] otg OTG Function Interrupt to check, A value of @ref USB_INT_OTG defined
 * @retval BOOL TRUE: Interrupt Asserted; FALSE: Interrupt is NOT asserted
   */
BOOL USB_INT_isOTGon(USB_INT_OTG otg);

/**
 * @brief Clear OTG Function Interrupt Flag
   * @param[in] otg OTG Function Interrupt to clear, A value of @ref USB_INT_OTG defined
 * @retval void
   */
void USB_INT_clrOTGon(USB_INT_OTG otg);

/**
 * @brief Enable @ref USB_INT_GP Interrupt
 * @para in name Interrupt to operate, A value of @ref USB_INT_GP defined
 * @para in en TRUE: Enable Interrupt; FALSE: Mask the Interrupt
 * @retval void
 */
void USB_INT_enGP(USB_INT_GP name, BOOL en);

/**
 * @brief @ref USB_INT_GP Interrupt Asserted
 * @para in name Interrupt to check, A value of @ref USB_INT_GP defined
 * @retval BOOL TRUE: Interrupt Asserted; FALSE: Interrupt is NOT asserted
   */
BOOL USB_INT_isGP(USB_INT_GP name);

/**
 * @brief Clear @ref USB_INT_GP Interrupt Flag
 * @para in name Interrupt to clear, A value of @ref USB_INT_GP defined
 * @retval void
   */
void USB_INT_clrGP(USB_INT_GP name);

/**
 * @brief Enable OTG Function
   * @param[in] ctl OTG Function to operate, A value of @ref USB_OTG_CTL defined
   * @param[in] val TRUE: Enable the function; FALSE: Disable the function
 * @retval BOOL The old value before it configure
   */
BOOL USB_otgControl(USB_OTG_CTL ctl, BOOL val);

/*
 *
 */
#define USB_ENDPOINT_XFER_CONTROL       0
#define USB_ENDPOINT_XFER_ISOC          1
#define USB_ENDPOINT_XFER_BULK          2
#define USB_ENDPOINT_XFER_INT           3
#define USB_ENDPOINT_XFERTYPE_MASK      3

typedef void							USB_OTG_CORE_DEVICE;
/********************************************************************************
                              Data structure type
********************************************************************************/
typedef enum {
  USB_OTG_OK,
  USB_OTG_FAIL
} USB_OTG_Status;

typedef USB_OTG_Status USB_OTG_STS;

enum USB_OTG_SPEED {
	USB_SPEED_UNKNOWN = 0,
	USB_SPEED_LOW,
	USB_SPEED_FULL,
	USB_SPEED_HIGH
};

typedef struct usb_ep_descriptor
{
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bEndpointAddress;
  uint8_t  bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t  bInterval;
} EP_DESCRIPTOR , *PEP_DESCRIPTOR;

/**
  * @brief  Initialize core registers address.
  * @param  pdev : device instance
  * @param  BaseAddress : Base Address for OTG core
  * @retval status
  */
USB_OTG_Status USB_OTG_SetAddress(USB_OTG_CORE_DEVICE *pdev, uint32_t BaseAddress);
/**
* @brief  USB_OTG_CoreInit
*         Initializes the USB_OTG controller registers and prepares the core
*         device mode or host mode operation.
* @param  pdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_Status USB_OTG_CoreInit(USB_OTG_CORE_DEVICE *pdev);
/**
* @brief  USB_OTG_CoreInitDev : Initializes the USB_OTG controller registers 
*         for device mode
* @param  pdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_Status USB_OTG_CoreInitDev(USB_OTG_CORE_DEVICE *pdev);
/**
  * @brief  Initializes the USB_OTG controller for host mode
  * @param  pdev : device instance
  * @retval status
  */
USB_OTG_Status USB_OTG_CoreInitHost(USB_OTG_CORE_DEVICE *pdev);
/**
* @brief  USB_OTG_DisableGlobalInt
*         Enables the controller's Global Int in the AHB Config reg
* @param  pdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_Status USB_OTG_DisableGlobalInt(USB_OTG_CORE_DEVICE *pdev);
/**
* @brief  USB_OTG_EnableGlobalInt
*         Enables the controller's Global Int in the AHB Config reg
* @param  pdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_Status USB_OTG_EnableGlobalInt(USB_OTG_CORE_DEVICE *pdev);
/**
  * @brief  Configure an EP
  * @param  pdev : Device instance
  * @param  epdesc : Endpoint Descriptor  
  * @retval status
  */
uint32_t USB_OTG_USBD_EP_Open(USB_OTG_CORE_DEVICE *pdev, EP_DESCRIPTOR *epdesc);
/**
  * @brief  called when an EP is disabled
  * @param  pdev : device instance
  * @param  ep_addr : endpoint address
  * @retval status
  */
uint32_t USB_OTG_USBD_EP_Close(USB_OTG_CORE_DEVICE *pdev, uint8_t ep_addr);
/**
  * @brief  Stall an endpoint.
  * @param  pdev : device instance
  * @param  epnum : endpoint index
  * @retval status
  */
uint32_t USB_OTG_USBD_EP_Stall(USB_OTG_CORE_DEVICE *pdev, uint8_t epnum);
/**
  * @brief  Clear stall condition on endpoints.
  * @param  pdev : device instance
  * @param  epnum : endpoint index
  * @retval status
  */
uint32_t USB_OTG_USBD_EP_ClrStall(USB_OTG_CORE_DEVICE *pdev, uint8_t epnum);
/**
  * @brief  This Function flushes the buffer.
  * @param  pdev : device instance
  * @param  epnum : endpoint index
  * @retval status
  */
uint32_t USB_OTG_USBD_EP_Flush(USB_OTG_CORE_DEVICE *pdev, uint8_t epnum);
/**
  * @brief  Initiate an srp session
  * @param  None
  * @retval None
  */
void USB_OTG_InitiateSRP(void);
/**
  * @brief  Initialize the HOST portion of the driver.
  * @param  pdev : device instance
  * @retval Status
  */
uint32_t HOST_Init(USB_OTG_CORE_DEVICE *pdev);
/**
  * @brief  Submit the USB request block (URB) to the right Host channel
   * @param ch Channel number will be used
   * @param devaddr USB Device Address
   * @param desc A pointer to DMA Descriptor (512-byte alignment)
   * @param ctd In terms of number of DMA descriptors (0 means start from 1st DMA descriptor)
   * @param ntd Number of Transfer Descriptors (from 0 to 63 which means from 1 to 64 descriptors)
   * @param ping TRUE: Do PING protocol (Only for OUT Transfer); Must be 0 for IN Transfer
   * @param pid PID: A value of @ref USB_HOST_PID defined: USB_HOST_PID_DATA0, USB_HOST_PID_DATA2, USB_HOST_PID_DATA1, USB_HOST_PID_MDATA or USB_HOST_PID_SETUP
   * @param mps Maximum Packet Size (in bytes)
   * @param epnum Endpoint Address
   * @param eptype Endpoint Type, A value of @ref USB_EP_TYPE defined
   * @param speed Device Speed, A value of @ref USB_ENUM_SPEED defined
  * @retval Status
  */
uint32_t HOST_StartXfer(USB_OTG_CORE_DEVICE *pdev, uint32_t ch, uint32_t devaddr, OTG_DESCRIPTOR *desc, uint32_t ctd, uint32_t ntd, BOOL ping, uint32_t pid, uint32_t mps, uint32_t epnum, USB_EP_TYPE eptype, USB_ENUM_SPEED speed);
/**
  * @brief  Initialize the USB device on of the driver.
  * @param  pdev : device instance
  * @retval None
  */
void USB_OTG_USBD_Init (USB_OTG_CORE_DEVICE *pdev);
/**
  * @brief  Read data from Fifo
  * @param  pdev : device instance
  * @param  ep_addr : endpoint address
  * @param size Data length (in bytes)
  * @param pktcnt Packet Count
  * @param PID (only for interrupt/bulk), 0x1 means DATA0; 0x2 means DATA1
  * @param stpcnt The number of back-to-back SETUP data packets the endpoint can receive. (0 - 3)
  * @param desc A pointer to DMA descriptors
  * @param snoop Snoop Mode, TRUE: Enable Snoop Mode, which means it does not check if the OUT packets are correct before transferring them to application memory; FLASE: Disable Snoop Mode
  * @retval status
  */
uint32_t USB_OTG_USBD_EP_Read( USB_OTG_CORE_DEVICE *pdev, uint8_t ep_addr, uint32_t size, uint32_t pktcnt, uint32_t pid, uint32_t stpcnt, void *desc, BOOL snoop);
/**
  * @brief  Read data from Fifo
  * @param  pdev : device instance
  * @param  ep_addr : endpoint address
  * @param size Maximum Packet Size (in bytes)
  * @param pktcnt Packet Count
  * @param pid PID (only for interrupt/bulk), 0x1 means DATA0; 0x2 means DATA1
  * @param desc A pointer to DMA descriptors
  * @retval status
  */
uint32_t USB_OTG_USBD_EP_Write(USB_OTG_CORE_DEVICE *pdev, uint8_t ep_addr, uint32_t size, uint32_t pktcnt, uint32_t pid, void *desc, uint32_t mps/*8,16,32,64-byte*/);
/**
  * @brief  USB_OTG_ReadHPRT0 : Reads HPRT0 to modify later
  * @param  pdev : Selected device
  * @retval HPRT0 value
  */
uint32_t USB_OTG_ReadHPRT0(USB_OTG_CORE_DEVICE *pdev);
/**
  * @brief  USB_OTG_HC_Halt : Halt channel
  * @param  pdev : Selected device
  * @param  hc_num : channel number
  * @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_HC_Halt(USB_OTG_CORE_DEVICE *pdev, uint8_t hc_num);
/**
  * @brief  USB_OTG_GetDeviceSpeed
  *         Get the device speed from the device status register
  * @param  None
  * @retval status
*/
enum USB_OTG_SPEED USB_OTG_GetDeviceSpeed (USB_OTG_CORE_DEVICE *pdev);
/**
  * @brief  Flush a Tx FIFO
  * @param  pdev : device instance
  * @param  num : Tx FIFO number
  * @retval status
  */
USB_OTG_STS USB_OTG_FlushTxFifo (USB_OTG_CORE_DEVICE *pdev, uint32_t num);
/**
  * @brief  Flush the Rx FIFO
  * @param  pdev : device instance
  * @retval status
  */
USB_OTG_STS USB_OTG_FlushRxFifo(USB_OTG_CORE_DEVICE *pdev);
/**
  * @brief  USB_OTG_IsEvenFrame 
  *         This function returns the frame number for sof packet
  * @param  pdev : Selected device
  * @retval Frame number
  */
uint8_t USB_OTG_IsEvenFrame (USB_OTG_CORE_DEVICE *pdev);

#ifdef __cplusplus
}
#endif

#endif /* __CMEM7_USB_H */
