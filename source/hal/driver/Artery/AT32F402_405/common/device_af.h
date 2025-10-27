// macro name foramt:   VSF_HW_AF_{PERIPHERAL_NAME}_{PERIPHERAL_PIN_NAME}_P{PORT_INDEX}_{PIN_INDEX}
// PERIPHERAL_NAME:     USART/SPI/I2C/PWM/......
// PERIPHERAL_PIN_NAME:
//  USART:              CK/TX/RX/CTS/RTS/DE
//  SPI:                MISO/MOSI/SCK/CS(NSS)
//  I2C:                SDA/SCL/SMBA

#if !defined(VSF_HW_USART_MASK) && defined(VSF_HW_USART_COUNT)
#   define VSF_HW_USART_MASK                ((1 << VSF_HW_USART_COUNT) - 1)
#endif

#ifdef VSF_HW_USART_MASK
#if VSF_HW_USART_MASK & (1 << 1)

// remap for USART1

#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 8))
#   define VSF_HW_AF_USART1_CK_P0_8         7
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 9))
#   define VSF_HW_AF_USART1_TX_P0_9         7
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 10))
#   define VSF_HW_AF_USART1_RX_P0_10        7
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 11))
#   define VSF_HW_AF_USART1_CTS_P0_11       7
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 12))
#   define VSF_HW_AF_USART1_RTS_P0_12       7
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 12))
#   define VSF_HW_AF_USART1_DE_P0_12        7
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 15))
#   define VSF_HW_AF_USART1_TX_P0_15        7
#endif

#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 3))
#   define VSF_HW_AF_USART1_RX_P1_3         7
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 3))
#   define VSF_HW_AF_USART1_RTS_P1_3        8
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 4))
#   define VSF_HW_AF_USART1_CTS_P1_4        7
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 5))
#   define VSF_HW_AF_USART1_CK_P1_5         7
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 6))
#   define VSF_HW_AF_USART1_TX_P1_6         7
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 7))
#   define VSF_HW_AF_USART1_RX_P1_7         7
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 8))
#   define VSF_HW_AF_USART1_TX_P1_8         7
#endif

#endif

#if VSF_HW_USART_MASK & (1 << 2)

// remap for USART2

#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 0))
#   define VSF_HW_AF_USART2_RX_P0_0         6
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 0))
#   define VSF_HW_AF_USART2_CTS_P0_0        7
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 1))
#   define VSF_HW_AF_USART2_RTS_P0_1        7
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 1))
#   define VSF_HW_AF_USART2_DE_P0_1         7
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 2))
#   define VSF_HW_AF_USART2_TX_P0_2         7
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 3))
#   define VSF_HW_AF_USART2_RX_P0_3         7
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 4))
#   define VSF_HW_AF_USART2_CK_P0_4         7
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 8))
#   define VSF_HW_AF_USART2_TX_P0_8         8
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 14))
#   define VSF_HW_AF_USART2_TX_P0_14        8
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 15))
#   define VSF_HW_AF_USART2_RX_P0_15        8
#endif

#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 0))
#   define VSF_HW_AF_USART2_TX_P1_0         7
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 1))
#   define VSF_HW_AF_USART2_CK_P1_1         7
#endif

#endif

#if VSF_HW_USART_MASK & (1 << 3)

// remap for USART3

#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 5))
#   define VSF_HW_AF_USART3_CK_P0_5         6
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 5))
#   define VSF_HW_AF_USART3_RX_P0_5         7
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 6))
#   define VSF_HW_AF_USART3_CTS_P0_6        7
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 6))
#   define VSF_HW_AF_USART3_RX_P0_6         8
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 7))
#   define VSF_HW_AF_USART3_TX_P0_7         7
#endif

#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 0))
#   define VSF_HW_AF_USART3_CK_P1_0         8
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 1))
#   define VSF_HW_AF_USART3_RTS_P1_1        8
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 1))
#   define VSF_HW_AF_USART3_DE_P1_1         8
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 10))
#   define VSF_HW_AF_USART3_TX_P1_10        7
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 11))
#   define VSF_HW_AF_USART3_RX_P1_11        7
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 12))
#   define VSF_HW_AF_USART3_CK_P1_12        8
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 13))
#   define VSF_HW_AF_USART3_CTS_P1_13       8
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 14))
#   define VSF_HW_AF_USART3_RTS_P1_14       7
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 14))
#   define VSF_HW_AF_USART3_DE_P1_14        7
#endif

#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 4))
#   define VSF_HW_AF_USART3_TX_P2_4         7
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 5))
#   define VSF_HW_AF_USART3_RX_P2_5         7
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 10))
#   define VSF_HW_AF_USART3_TX_P2_10        7
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 11))
#   define VSF_HW_AF_USART3_RX_P2_11        7
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 12))
#   define VSF_HW_AF_USART3_CK_P2_12        7
#endif

#if defined(VSF_HW_GPIO_PORT3_MASK) && (VSF_HW_GPIO_PORT3_MASK & (1 << 2))
#   define VSF_HW_AF_USART3_RTS_P3_2        7
#endif
#if defined(VSF_HW_GPIO_PORT3_MASK) && (VSF_HW_GPIO_PORT3_MASK & (1 << 2))
#   define VSF_HW_AF_USART3_DE_P3_2         7
#endif

#endif

#if VSF_HW_USART_MASK & (1 << 4)

// remap for USART4

#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 0))
#   define VSF_HW_AF_USART4_TX_P0_0         8
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 1))
#   define VSF_HW_AF_USART4_RX_P0_1         8
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 15))
#   define VSF_HW_AF_USART4_RTS_P0_15       10
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 15))
#   define VSF_HW_AF_USART4_DE_P0_15        10
#endif

#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 6))
#   define VSF_HW_AF_USART4_CK_P1_6         10
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 7))
#   define VSF_HW_AF_USART4_CTS_P1_7        8
#endif

#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 10))
#   define VSF_HW_AF_USART4_TX_P2_10        8
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 11))
#   define VSF_HW_AF_USART4_RX_P2_11        8
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 12))
#   define VSF_HW_AF_USART4_CK_P2_12        8
#endif

#endif

#if VSF_HW_USART_MASK & (1 << 5)

// remap for USART5

#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 3))
#   define VSF_HW_AF_USART5_TX_P1_3         10
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 4))
#   define VSF_HW_AF_USART5_RX_P1_4         10
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 5))
#   define VSF_HW_AF_USART5_RX_P1_5         8
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 5))
#   define VSF_HW_AF_USART5_CK_P1_5         10
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 5))
#   define VSF_HW_AF_USART5_RTS_P1_5        10
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 5))
#   define VSF_HW_AF_USART5_DE_P1_5         10
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 6))
#   define VSF_HW_AF_USART5_TX_P1_6         8
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 8))
#   define VSF_HW_AF_USART5_RX_P1_8         8
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 9))
#   define VSF_HW_AF_USART5_TX_P1_9         8
#endif

#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 10))
#   define VSF_HW_AF_USART5_TX_P2_12        10
#endif

#if defined(VSF_HW_GPIO_PORT3_MASK) && (VSF_HW_GPIO_PORT3_MASK & (1 << 8))
#   define VSF_HW_AF_USART5_RX_P3_2         8
#endif

#endif

#if VSF_HW_USART_MASK & (1 << 6)

// remap for USART6

#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 4))
#   define VSF_HW_AF_USART6_TX_P0_4         8
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 5))
#   define VSF_HW_AF_USART6_RX_P0_5         8
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 11))
#   define VSF_HW_AF_USART6_TX_P0_11        8
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 12))
#   define VSF_HW_AF_USART6_RX_P0_12        8
#endif

#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 0))
#   define VSF_HW_AF_USART6_TX_P2_0         8
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 1))
#   define VSF_HW_AF_USART6_RX_P2_1         8
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 6))
#   define VSF_HW_AF_USART6_TX_P2_6         8
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 7))
#   define VSF_HW_AF_USART6_RX_P2_7         8
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 8))
#   define VSF_HW_AF_USART6_CK_P2_8         8
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 8))
#   define VSF_HW_AF_USART6_RTS_P2_8        8
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 8))
#   define VSF_HW_AF_USART6_DE_P2_8         8
#endif

#endif

#if VSF_HW_USART_MASK & (1 << 7)

// remap for USART7

#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 8))
#   define VSF_HW_AF_USART7_TX_P0_8         9
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 15))
#   define VSF_HW_AF_USART7_TX_P0_15        9
#endif

#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 3))
#   define VSF_HW_AF_USART7_RX_P1_3         9
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 4))
#   define VSF_HW_AF_USART7_TX_P1_4         9
#endif

#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 0))
#   define VSF_HW_AF_USART7_TX_P2_0         9
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 1))
#   define VSF_HW_AF_USART7_RX_P2_1         9
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 6))
#   define VSF_HW_AF_USART7_TX_P2_6         9
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 7))
#   define VSF_HW_AF_USART7_RX_P2_7         9
#endif

#if defined(VSF_HW_GPIO_PORT5_MASK) && (VSF_HW_GPIO_PORT5_MASK & (1 << 6))
#   define VSF_HW_AF_USART7_RX_P5_6         9
#endif
#if defined(VSF_HW_GPIO_PORT5_MASK) && (VSF_HW_GPIO_PORT5_MASK & (1 << 7))
#   define VSF_HW_AF_USART7_TX_P5_7         9
#endif

#endif

#if VSF_HW_USART_MASK & (1 << 8)

// remap for USART8

#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 2))
#   define VSF_HW_AF_USART8_TX_P2_2         8
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 3))
#   define VSF_HW_AF_USART8_RX_P2_3         8
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 8))
#   define VSF_HW_AF_USART8_TX_P2_8         7
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 9))
#   define VSF_HW_AF_USART8_RX_P2_9         7
#endif

#endif

#endif      // VSF_HW_USART_MASK

#if !defined(VSF_HW_SPI_MASK) && defined(VSF_HW_SPI_COUNT)
#   define VSF_HW_SPI_MASK                  ((1 << VSF_HW_SPI_COUNT) - 1)
#endif

#ifdef VSF_HW_SPI_MASK
#if VSF_HW_SPI_MASK & (1 << 1)

// remap for SPI1

#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 4))
#   define VSF_HW_AF_SPI1_CS_P0_4           5
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 5))
#   define VSF_HW_AF_SPI1_SCK_P0_5          5
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 6))
#   define VSF_HW_AF_SPI1_MISO_P0_6         5
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 7))
#   define VSF_HW_AF_SPI1_MOSI_P0_7         5
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 15))
#   define VSF_HW_AF_SPI1_CS_P0_15          5
#endif

#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 0))
#   define VSF_HW_AF_SPI1_MISO_P1_0         5
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 1))
#   define VSF_HW_AF_SPI1_MOSI_P1_1         5
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 3))
#   define VSF_HW_AF_SPI1_SCK_P1_3          5
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 4))
#   define VSF_HW_AF_SPI1_MISO_P1_4         5
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 5))
#   define VSF_HW_AF_SPI1_MOSI_P1_5         5
#endif

#endif

#if VSF_HW_SPI_MASK & (1 << 2)

// remap for SPI2

#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 9))
#   define VSF_HW_AF_SPI2_SCK_P0_9          5
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 10))
#   define VSF_HW_AF_SPI2_MOSI_P0_10        5
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 11))
#   define VSF_HW_AF_SPI2_CS_P0_11          5
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 12))
#   define VSF_HW_AF_SPI2_MISO_P0_12        5
#endif

#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 1))
#   define VSF_HW_AF_SPI2_SCK_P1_1          6
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 9))
#   define VSF_HW_AF_SPI2_CS_P1_9           5
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 10))
#   define VSF_HW_AF_SPI2_SCK_P1_10         5
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 12))
#   define VSF_HW_AF_SPI2_CS_P1_12          5
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 13))
#   define VSF_HW_AF_SPI2_SCK_P1_13         5
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 14))
#   define VSF_HW_AF_SPI2_MISO_P1_14        5
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 15))
#   define VSF_HW_AF_SPI2_MOSI_P1_15        5
#endif

#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 1))
#   define VSF_HW_AF_SPI2_MOSI_P2_1         6
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 2))
#   define VSF_HW_AF_SPI2_MISO_P2_2         5
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 3))
#   define VSF_HW_AF_SPI2_MOSI_P2_3         5
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 7))
#   define VSF_HW_AF_SPI2_SCK_P2_7          5
#endif

#if defined(VSF_HW_GPIO_PORT5_MASK) && (VSF_HW_GPIO_PORT5_MASK & (1 << 1))
#   define VSF_HW_AF_SPI2_CS_P5_1           5
#endif

#endif

#if VSF_HW_SPI_MASK & (1 << 3)

// remap for SPI3

#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 4))
#   define VSF_HW_AF_SPI3_CS_P0_4           6
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 13))
#   define VSF_HW_AF_SPI3_MISO_P0_13        6
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 14))
#   define VSF_HW_AF_SPI3_MOSI_P0_14        6
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 15))
#   define VSF_HW_AF_SPI3_CS_P0_15          6
#endif

#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 0))
#   define VSF_HW_AF_SPI3_MOSI_P1_0         6
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 2))
#   define VSF_HW_AF_SPI3_MOSI_P1_2         6
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 3))
#   define VSF_HW_AF_SPI3_SCK_P1_3          6
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 4))
#   define VSF_HW_AF_SPI3_MISO_P1_4         6
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 5))
#   define VSF_HW_AF_SPI3_MOSI_P1_5         6
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 12))
#   define VSF_HW_AF_SPI3_SCK_P1_12         6
#endif

#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 1))
#   define VSF_HW_AF_SPI3_MOSI_P2_1         5
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 10))
#   define VSF_HW_AF_SPI3_SCK_P2_10         6
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 11))
#   define VSF_HW_AF_SPI3_MISO_P2_11        6
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 12))
#   define VSF_HW_AF_SPI3_MOSI_P2_12        6
#endif

#endif

#endif

#if !defined(VSF_HW_I2C_MASK) && defined(VSF_HW_I2C_COUNT)
#   define VSF_HW_I2C_MASK                  ((1 << VSF_HW_I2C_COUNT) - 1)
#endif

#ifdef VSF_HW_I2C_MASK
#if VSF_HW_I2C_MASK & (1 << 1)

// remap for I2C1

#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 1))
#   define VSF_HW_AF_I2C1_SMBA_P0_1         5
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 4))
#   define VSF_HW_AF_I2C1_SCL_P0_4          4
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 9))
#   define VSF_HW_AF_I2C1_SCL_P0_9          8
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 10))
#   define VSF_HW_AF_I2C1_SDA_P0_10         8
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 11))
#   define VSF_HW_AF_I2C1_SMBA_P0_11        6
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 13))
#   define VSF_HW_AF_I2C1_SDA_P0_13         4
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 14))
#   define VSF_HW_AF_I2C1_SMBA_P0_14        4
#endif

#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 6))
#   define VSF_HW_AF_I2C1_SCL_P1_6          4
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 7))
#   define VSF_HW_AF_I2C1_SDA_P1_7          4
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 8))
#   define VSF_HW_AF_I2C1_SCL_P1_8          4
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 9))
#   define VSF_HW_AF_I2C1_SDA_P1_9          4
#endif

#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 0))
#   define VSF_HW_AF_I2C1_SCL_P2_0          7
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 1))
#   define VSF_HW_AF_I2C1_SDA_P2_1          7
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 5))
#   define VSF_HW_AF_I2C1_SMBA_P2_5         4
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 6))
#   define VSF_HW_AF_I2C1_SCL_P2_6          4
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 7))
#   define VSF_HW_AF_I2C1_SDA_P2_7          4
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 9))
#   define VSF_HW_AF_I2C1_SDA_P2_9          8
#endif

#if defined(VSF_HW_GPIO_PORT5_MASK) && (VSF_HW_GPIO_PORT5_MASK & (1 << 0))
#   define VSF_HW_AF_I2C1_SDA_P5_0          4
#endif
#if defined(VSF_HW_GPIO_PORT5_MASK) && (VSF_HW_GPIO_PORT5_MASK & (1 << 1))
#   define VSF_HW_AF_I2C1_SCL_P5_1          4
#endif
#if defined(VSF_HW_GPIO_PORT5_MASK) && (VSF_HW_GPIO_PORT5_MASK & (1 << 4))
#   define VSF_HW_AF_I2C1_SDA_P5_4          4
#endif
#if defined(VSF_HW_GPIO_PORT5_MASK) && (VSF_HW_GPIO_PORT5_MASK & (1 << 5))
#   define VSF_HW_AF_I2C1_SCL_P5_5          4
#endif

#endif

#if VSF_HW_I2C_MASK & (1 << 2)

// remap for I2C2

#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 0))
#   define VSF_HW_AF_I2C2_SCL_P0_0          4
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 1))
#   define VSF_HW_AF_I2C2_SDA_P0_1          4
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 11))
#   define VSF_HW_AF_I2C2_SCL_P0_11         4
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 12))
#   define VSF_HW_AF_I2C2_SDA_P0_12         4
#endif

#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 3))
#   define VSF_HW_AF_I2C2_SDA_P1_3          4
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 9))
#   define VSF_HW_AF_I2C2_SDA_P1_9          7
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 10))
#   define VSF_HW_AF_I2C2_SCL_P1_10         4
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 11))
#   define VSF_HW_AF_I2C2_SDA_P1_11         4
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 12))
#   define VSF_HW_AF_I2C2_SMBA_P1_12        4
#endif

#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 12))
#   define VSF_HW_AF_I2C2_SDA_P2_12         4
#endif

#if defined(VSF_HW_GPIO_PORT5_MASK) && (VSF_HW_GPIO_PORT5_MASK & (1 << 6))
#   define VSF_HW_AF_I2C2_SCL_P5_6          4
#endif
#if defined(VSF_HW_GPIO_PORT5_MASK) && (VSF_HW_GPIO_PORT5_MASK & (1 << 7))
#   define VSF_HW_AF_I2C2_SDA_P5_7          4
#endif

#endif

#if VSF_HW_I2C_MASK & (1 << 3)

// remap for I2C3

#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 7))
#   define VSF_HW_AF_I2C3_SCL_P0_7          4
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 8))
#   define VSF_HW_AF_I2C3_SCL_P0_8          4
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 9))
#   define VSF_HW_AF_I2C3_SMBA_P0_9         4
#endif

#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 2))
#   define VSF_HW_AF_I2C3_SMBA_P1_2         4
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 4))
#   define VSF_HW_AF_I2C3_SDA_P1_4          4
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 5))
#   define VSF_HW_AF_I2C3_SMBA_P1_5         4
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 13))
#   define VSF_HW_AF_I2C3_SMBA_P1_13        4
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 13))
#   define VSF_HW_AF_I2C3_SCL_P1_13         7
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 14))
#   define VSF_HW_AF_I2C3_SDA_P1_14         4
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 15))
#   define VSF_HW_AF_I2C3_SCL_P1_15         4
#endif

#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 0))
#   define VSF_HW_AF_I2C3_SCL_P2_0          4
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 1))
#   define VSF_HW_AF_I2C3_SDA_P2_1          4
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 9))
#   define VSF_HW_AF_I2C3_SDA_P2_9          4
#endif

#endif

#endif

#if !defined(VSF_HW_USB_OTG_MASK) && defined(VSF_HW_USB_OTG_COUNT)
#   define VSF_HW_USB_OTG_MASK              ((1 << VSF_HW_USB_OTG_COUNT) - 1)
#endif

#ifdef VSF_HW_USB_OTG_MASK
#if VSF_HW_USB_OTG_MASK & (1 << 0)
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 8))
#   define VSF_HW_AF_OTG0_SOF_P0_8          10
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 9))
#   define VSF_HW_AF_OTG0_VBUS_P0_9         10
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 10))
#   define VSF_HW_AF_OTG0_ID_P0_10          10
#endif
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 13))
#   define VSF_HW_AF_OTG0_OE_P0_13          10
#endif
#endif
#if VSF_HW_USB_OTG_MASK & (1 << 1)
#if defined(VSF_HW_GPIO_PORT0_MASK) && (VSF_HW_GPIO_PORT0_MASK & (1 << 4))
#   define VSF_HW_AF_OTG1_SOF_P0_4          10
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 12))
#   define VSF_HW_AF_OTG1_ID_P1_12          10
#endif
#if defined(VSF_HW_GPIO_PORT1_MASK) && (VSF_HW_GPIO_PORT1_MASK & (1 << 13))
#   define VSF_HW_AF_OTG1_VBUS_P1_13        10
#endif
#if defined(VSF_HW_GPIO_PORT2_MASK) && (VSF_HW_GPIO_PORT2_MASK & (1 << 9))
#   define VSF_HW_AF_OTG1_OE_P2_9           10
#endif
#endif
#endif