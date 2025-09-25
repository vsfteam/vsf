
#if !defined(VSF_HW_USART_MASK) && defined(VSF_HW_USART_COUNT)
#   define VSF_HW_USART_MASK                ((1 << VSF_HW_USART_COUNT) - 1)
#endif

#ifdef VSF_HW_USART_MASK
#if VSF_HW_USART_MASK & (1 << 1)

// remap for USART1

#   define VSF_HW_AF_USART1_CK_P0_8         6
#   define VSF_HW_AF_USART1_TX_P0_9         7
#   define VSF_HW_AF_USART1_RX_P0_10        5
#   define VSF_HW_AF_USART1_CTS_P0_11       6
#   define VSF_HW_AF_USART1_NSS_P0_11       6
#   define VSF_HW_AF_USART1_RTS_P0_12       6

#   define VSF_HW_AF_USART1_TX_P1_6         7
#   define VSF_HW_AF_USART1_RX_P1_7         8
#   define VSF_HW_AF_USART1_TX_P1_14        9
#   define VSF_HW_AF_USART1_RX_P1_15        9

#   define VSF_HW_AF_USART1_TX_P5_0         8
#   define VSF_HW_AF_USART1_RX_P5_1         9
#   define VSF_HW_AF_USART1_CK_P5_2         10
#   define VSF_HW_AF_USART1_CTS_P5_3        10
#   define VSF_HW_AF_USART1_NSS_P5_3        10
#   define VSF_HW_AF_USART1_DE_P5_4         10
#   define VSF_HW_AF_USART1_RTS_P5_4        10

#endif

#if VSF_HW_USART_MASK & (1 << 2)

// remap for USART2

#   define VSF_HW_AF_USART2_CTS_P0_0        10
#   define VSF_HW_AF_USART2_NSS_P0_0        10
#   define VSF_HW_AF_USART2_DE_P0_1         8
#   define VSF_HW_AF_USART2_RTS_P0_1        8
#   define VSF_HW_AF_USART2_TX_P0_2         7
#   define VSF_HW_AF_USART2_RX_P0_3         11
#   define VSF_HW_AF_USART2_CK_P0_4         11

#   define VSF_HW_AF_USART2_CTS_P4_3        7
#   define VSF_HW_AF_USART2_NSS_P4_3        7
#   define VSF_HW_AF_USART2_DE_P4_4         7
#   define VSF_HW_AF_USART2_RTS_P4_4        7
#   define VSF_HW_AF_USART2_TX_P4_5         6
#   define VSF_HW_AF_USART2_RX_P4_6         7
#   define VSF_HW_AF_USART2_CK_P4_7         7

#   define VSF_HW_AF_USART2_TX_P5_0         8
#   define VSF_HW_AF_USART2_RX_P5_1         9
#   define VSF_HW_AF_USART2_CK_P5_2         10
#   define VSF_HW_AF_USART2_CTS_P5_3        10
#   define VSF_HW_AF_USART2_NSS_P5_3        10
#   define VSF_HW_AF_USART2_DE_P5_4         10
#   define VSF_HW_AF_USART2_RTS_P5_4        10

#endif

#if VSF_HW_USART_MASK & (1 << 3)

// remap for USART3

#   define VSF_HW_AF_USART3_TX_P1_10        9
#   define VSF_HW_AF_USART3_RX_P1_11        6
#   define VSF_HW_AF_USART3_CK_P1_12        8
#   define VSF_HW_AF_USART3_CTS_P1_13       8
#   define VSF_HW_AF_USART3_NSS_P1_13       8
#   define VSF_HW_AF_USART3_DE_P1_14        10
#   define VSF_HW_AF_USART3_RTS_P1_14       10

#   define VSF_HW_AF_USART3_TX_P2_10        7
#   define VSF_HW_AF_USART3_RX_P2_11        7
#   define VSF_HW_AF_USART3_CK_P2_12        10

#   define VSF_HW_AF_USART3_TX_P3_8         6
#   define VSF_HW_AF_USART3_RX_P3_9         5
#   define VSF_HW_AF_USART3_CK_P3_10        7
#   define VSF_HW_AF_USART3_CTS_P3_11       5
#   define VSF_HW_AF_USART3_NSS_P3_11       5
#   define VSF_HW_AF_USART3_DE_P3_12        6
#   define VSF_HW_AF_USART3_RTS_P3_12       6

#endif

#if VSF_HW_USART_MASK & (1 << 4)

// remap for USART4

#   define VSF_HW_AF_USART4_TX_P2_6         10
#   define VSF_HW_AF_USART4_RX_P2_7         10
#   define VSF_HW_AF_USART4_CK_P2_8         7

#   define VSF_HW_AF_USART4_CK_P6_7         6
#   define VSF_HW_AF_USART4_DE_P6_8         7
#   define VSF_HW_AF_USART4_RTS_P6_8        7
#   define VSF_HW_AF_USART4_RX_P6_9         7
#   define VSF_HW_AF_USART4_DE_P6_12        11
#   define VSF_HW_AF_USART4_RTS_P6_12       11
#   define VSF_HW_AF_USART4_CTS_P6_13       9
#   define VSF_HW_AF_USART4_NSS_P6_13       9
#   define VSF_HW_AF_USART4_TX_P6_14        10
#   define VSF_HW_AF_USART4_CTS_P6_15       5
#   define VSF_HW_AF_USART4_NSS_P6_15       5

#endif

#if VSF_HW_USART_MASK & (1 << 5)

// remap for USART5

#   define VSF_HW_AF_USART5_RX_P4_2         6
#   define VSF_HW_AF_USART5_TX_P4_3         4
#   define VSF_HW_AF_USART5_CK_P4_15        5

#   define VSF_HW_AF_USART5_RX_P6_11        9
#   define VSF_HW_AF_USART5_TX_P6_12        12
#   define VSF_HW_AF_USART5_CTS_P6_13       10
#   define VSF_HW_AF_USART5_NSS_P6_13       10
#   define VSF_HW_AF_USART5_DE_P6_14        11
#   define VSF_HW_AF_USART5_RTS_P6_14       11
#   define VSF_HW_AF_USART5_CK_P6_15        6

#endif

#if VSF_HW_USART_MASK & (1 << 6)

// remap for USART6

#   define VSF_HW_AF_USART6_RX_P4_4         6
#   define VSF_HW_AF_USART6_TX_P4_5         6
#   define VSF_HW_AF_USART6_CK_P4_14        7

#   define VSF_HW_AF_USART6_CTS_P6_4        8
#   define VSF_HW_AF_USART6_NSS_P6_4        8
#   define VSF_HW_AF_USART6_DE_P6_5         9
#   define VSF_HW_AF_USART6_RTS_P6_5        9

#   define VSF_HW_AF_USART6_RX_P7_14        7
#   define VSF_HW_AF_USART6_TX_P7_15        8

#endif

#if VSF_HW_USART_MASK & (1 << 7)

// remap for USART7

#   define VSF_HW_AF_USART7_CK_P5_13        7
#   define VSF_HW_AF_USART7_RX_P5_14        6
#   define VSF_HW_AF_USART7_TX_P5_15        7

#   define VSF_HW_AF_USART7_CTS_P6_2        8
#   define VSF_HW_AF_USART7_NSS_P6_2        8
#   define VSF_HW_AF_USART7_DE_P6_3         6
#   define VSF_HW_AF_USART7_RTS_P6_3        6

#   define VSF_HW_AF_USART6_CK_P7_3         7
#   define VSF_HW_AF_USART6_RX_P7_4         6
#   define VSF_HW_AF_USART6_TX_P7_5         6

#endif

#if VSF_HW_USART_MASK & (1 << 8)

// remap for USART8

#   define VSF_HW_AF_USART8_CK_P8_1         9
#   define VSF_HW_AF_USART8_RX_P8_2         8
#   define VSF_HW_AF_USART8_TX_P8_3         8
#   define VSF_HW_AF_USART8_RX_P8_4         8
#   define VSF_HW_AF_USART8_RX_P8_5         9
#   define VSF_HW_AF_USART8_CK_P8_6         8

#   define VSF_HW_AF_USART8_CTS_P9_8        5
#   define VSF_HW_AF_USART8_RTS_P9_9        5

#endif

#if VSF_HW_USART_MASK & (1 << 9)

// remap for USART9

#   define VSF_HW_AF_USART9_TX_P0_0         11
#   define VSF_HW_AF_USART9_RX_P0_1         9
#   define VSF_HW_AF_USART9_RX_P0_11        7
#   define VSF_HW_AF_USART9_TX_P0_12        7
#   define VSF_HW_AF_USART9_DE_P0_15        11
#   define VSF_HW_AF_USART9_RTS_P0_15       11

#   define VSF_HW_AF_USART9_CTS_P1_0        8
#   define VSF_HW_AF_USART9_RX_P1_8         9
#   define VSF_HW_AF_USART9_TX_P1_9         8
#   define VSF_HW_AF_USART9_DE_P1_14        11
#   define VSF_HW_AF_USART9_RTS_P1_14       11
#   define VSF_HW_AF_USART9_CTS_P1_15       10

#   define VSF_HW_AF_USART9_TX_P2_10        8
#   define VSF_HW_AF_USART9_RX_P2_11        8

#   define VSF_HW_AF_USART9_RX_P5_0         6
#   define VSF_HW_AF_USART9_TX_P5_1         5

#   define VSF_HW_AF_USART9_TX_P7_13        6
#   define VSF_HW_AF_USART9_RX_P7_14        8

#   define VSF_HW_AF_USART9_RX_P8_9         7

#endif

#if VSF_HW_USART_MASK & (1 << 10)

// remap for USART10

#   define VSF_HW_AF_USART10_RX_P1_5        10
#   define VSF_HW_AF_USART10_TX_P1_6        8
#   define VSF_HW_AF_USART10_RX_P1_12       9
#   define VSF_HW_AF_USART10_TX_P1_13       9

#   define VSF_HW_AF_USART10_DE_P2_8        8
#   define VSF_HW_AF_USART10_RTS_P2_8       8
#   define VSF_HW_AF_USART10_CTS_P2_9       11
#   define VSF_HW_AF_USART10_TX_P2_12       11

#   define VSF_HW_AF_USART10_RX_P3_2        10

#   define VSF_HW_AF_USART10_DE_P7_8        8
#   define VSF_HW_AF_USART10_RTS_P7_8       8
#   define VSF_HW_AF_USART10_CTS_P7_9       7
#   define VSF_HW_AF_USART10_RX_P7_11       7
#   define VSF_HW_AF_USART10_TX_P7_12       9

#endif

#if VSF_HW_USART_MASK & (1 << 11)

// remap for USART11

#   define VSF_HW_AF_USART11_RX_P0_8        7
#   define VSF_HW_AF_USART11_tX_P0_15       12

#   define VSF_HW_AF_USART11_RX_P1_3        10
#   define VSF_HW_AF_USART11_TX_P1_4        12

#   define VSF_HW_AF_USART11_RX_P4_7        7
#   define VSF_HW_AF_USART11_TX_P4_8        7
#   define VSF_HW_AF_USART11_DE_P4_9        7
#   define VSF_HW_AF_USART11_RTS_P4_9       7
#   define VSF_HW_AF_USART11_CTS_P4_10      6

#   define VSF_HW_AF_USART11_RX_P5_6        10
#   define VSF_HW_AF_USART11_TX_P5_7        9
#   define VSF_HW_AF_USART11_DE_P5_8        10
#   define VSF_HW_AF_USART11_RTS_P5_8       10
#   define VSF_HW_AF_USART11_CTS_P5_9       11

#endif

#if VSF_HW_USART_MASK & (1 << 12)

// remap for USART12

#   define VSF_HW_AF_USART12_TX_P3_12       7
#   define VSF_HW_AF_USART12_RX_P3_13       5
#   define VSF_HW_AF_USART12_CTS_P3_14      7
#   define VSF_HW_AF_USART12_DE_P3_15       7
#   define VSF_HW_AF_USART12_RTS_P3_15      7

#   define VSF_HW_AF_USART12_RX_P4_0        10
#   define VSF_HW_AF_USART12_TX_P4_1        8

#   define VSF_HW_AF_USART12_TX_P9_8        6
#   define VSF_HW_AF_USART12_RX_P9_9        6
#   define VSF_HW_AF_USART12_CTS_P9_10      6
#   define VSF_HW_AF_USART12_RTS_P9_11      6

#endif

#if VSF_HW_USART_MASK & (1 << 13)

// remap for USART13

#   define VSF_HW_AF_USART13_CTS_P3_0       7
#   define VSF_HW_AF_USART13_CTS_P3_12      8
#   define VSF_HW_AF_USART13_DE_P3_13       6
#   define VSF_HW_AF_USART13_RTS_P3_13      6
#   define VSF_HW_AF_USART13_RX_P3_14       8
#   define VSF_HW_AF_USART13_TX_P3_15       8

#   define VSF_HW_AF_USART13_RX_P4_12       7
#   define VSF_HW_AF_USART13_TX_P4_13       6
#   define VSF_HW_AF_USART13_CTS_P4_14      8
#   define VSF_HW_AF_USART13_DE_P4_15       6
#   define VSF_HW_AF_USART13_RTS_P4_15      6

#   define VSF_HW_AF_USART13_RX_P6_0        9
#   define VSF_HW_AF_USART13_TX_P6_1        10

#   define VSF_HW_AF_USART13_RTS_P9_3       6
#   define VSF_HW_AF_USART13_CTS_P9_4       6

#endif

#if VSF_HW_USART_MASK & (1 << 14)

// remap for USART14

#   define VSF_HW_AF_USART14_RX_P7_6        9
#   define VSF_HW_AF_USART14_TX_P7_7        7
#   define VSF_HW_AF_USART14_DE_P7_8        9
#   define VSF_HW_AF_USART14_RTS_P7_8       9
#   define VSF_HW_AF_USART14_CTS_P7_9       8

#   define VSF_HW_AF_USART14_RTS_P9_8       7
#   define VSF_HW_AF_USART14_CTS_P9_9       7
#   define VSF_HW_AF_USART14_RX_P9_10       7
#   define VSF_HW_AF_USART14_TX_P9_11       7

#endif

#if VSF_HW_USART_MASK & (1 << 14)

// remap for USART14

#   define VSF_HW_AF_USART15_RX_P7_2        9
#   define VSF_HW_AF_USART15_TX_P7_3        8
#   define VSF_HW_AF_USART15_DE_P7_4        7
#   define VSF_HW_AF_USART15_RTS_P7_4       7
#   define VSF_HW_AF_USART15_CTS_P7_5       7

#   define VSF_HW_AF_USART15_RX_P9_0        6
#   define VSF_HW_AF_USART15_TX_P9_1        7
#   define VSF_HW_AF_USART15_RTS_P9_2       6
#   define VSF_HW_AF_USART15_CTS_P9_3       7

#endif
#endif      // VSF_HW_USART_MASK
