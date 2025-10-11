
#if !defined(VSF_HW_USART_MASK) && defined(VSF_HW_USART_COUNT)
#   define VSF_HW_USART_MASK                ((1 << VSF_HW_USART_COUNT) - 1)
#endif

#ifdef VSF_HW_USART_MASK
#if VSF_HW_USART_MASK & (1 << 0)
#   define VSF_HW_AF_USART0_TX_P0_0         1
#   define VSF_HW_AF_USART0_RX_P0_1         1
#   define VSF_HW_AF_USART0_CTS_P0_2        1
#   define VSF_HW_AF_USART0_RTS_P0_3        1
#   define VSF_HW_AF_USART0_DE_P0_4         1
#   define VSF_HW_AF_USART0_NSS_P0_5        1
#   define VSF_HW_AF_USART0_CK_P0_6         1
#endif
#if VSF_HW_USART_MASK & (1 << 1)
#   define VSF_HW_AF_USART1_TX_P1_0         1
#   define VSF_HW_AF_USART1_RX_P1_1         1
#   define VSF_HW_AF_USART1_CTS_P1_2        1
#   define VSF_HW_AF_USART1_RTS_P1_3        1
#   define VSF_HW_AF_USART1_DE_P1_4         1
#   define VSF_HW_AF_USART1_NSS_P1_5        1
#   define VSF_HW_AF_USART1_CK_P1_6         1
#endif
#if VSF_HW_USART_MASK & (1 << 2)
#   define VSF_HW_AF_USART2_TX_P2_0         1
#   define VSF_HW_AF_USART2_RX_P2_1         1
#   define VSF_HW_AF_USART2_CTS_P2_2        1
#   define VSF_HW_AF_USART2_RTS_P2_3        1
#   define VSF_HW_AF_USART2_DE_P2_4         1
#   define VSF_HW_AF_USART2_NSS_P2_5        1
#   define VSF_HW_AF_USART2_CK_P2_6         1
#endif
#if VSF_HW_USART_MASK & (1 << 3)
#   define VSF_HW_AF_USART3_TX_P3_0         1
#   define VSF_HW_AF_USART3_RX_P3_1         1
#   define VSF_HW_AF_USART3_CTS_P3_2        1
#   define VSF_HW_AF_USART3_RTS_P3_3        1
#   define VSF_HW_AF_USART3_DE_P3_4         1
#   define VSF_HW_AF_USART3_NSS_P3_5        1
#   define VSF_HW_AF_USART3_CK_P3_6         1
#endif
#if VSF_HW_USART_MASK & (1 << 4)
#   define VSF_HW_AF_USART4_TX_P4_0         1
#   define VSF_HW_AF_USART4_RX_P4_1         1
#   define VSF_HW_AF_USART4_CTS_P4_2        1
#   define VSF_HW_AF_USART4_RTS_P4_3        1
#   define VSF_HW_AF_USART4_DE_P4_4         1
#   define VSF_HW_AF_USART4_NSS_P4_5        1
#   define VSF_HW_AF_USART4_CK_P4_6         1
#endif
#if VSF_HW_USART_MASK & (1 << 5)
#   define VSF_HW_AF_USART5_TX_P5_0         1
#   define VSF_HW_AF_USART5_RX_P5_1         1
#   define VSF_HW_AF_USART5_CTS_P5_2        1
#   define VSF_HW_AF_USART5_RTS_P5_3        1
#   define VSF_HW_AF_USART5_DE_P5_4         1
#   define VSF_HW_AF_USART5_NSS_P5_5        1
#   define VSF_HW_AF_USART5_CK_P5_6         1
#endif
#if VSF_HW_USART_MASK & (1 << 6)
#   define VSF_HW_AF_USART6_TX_P6_0         1
#   define VSF_HW_AF_USART6_RX_P6_1         1
#   define VSF_HW_AF_USART6_CTS_P6_2        1
#   define VSF_HW_AF_USART6_RTS_P6_3        1
#   define VSF_HW_AF_USART6_DE_P6_4         1
#   define VSF_HW_AF_USART6_NSS_P6_5        1
#   define VSF_HW_AF_USART6_CK_P6_6         1
#endif
#if VSF_HW_USART_MASK & (1 << 7)
#   define VSF_HW_AF_USART7_TX_P7_0         1
#   define VSF_HW_AF_USART7_RX_P7_1         1
#   define VSF_HW_AF_USART7_CTS_P7_2        1
#   define VSF_HW_AF_USART7_RTS_P7_3        1
#   define VSF_HW_AF_USART7_DE_P7_4         1
#   define VSF_HW_AF_USART7_NSS_P7_5        1
#   define VSF_HW_AF_USART7_CK_P7_6         1
#endif
#if VSF_HW_USART_MASK & (1 << 8)
#   define VSF_HW_AF_USART8_TX_P8_0         1
#   define VSF_HW_AF_USART8_RX_P8_1         1
#   define VSF_HW_AF_USART8_CTS_P8_2        1
#   define VSF_HW_AF_USART8_RTS_P8_3        1
#   define VSF_HW_AF_USART8_DE_P8_4         1
#   define VSF_HW_AF_USART8_NSS_P8_5        1
#   define VSF_HW_AF_USART8_CK_P8_6         1
#endif
#if VSF_HW_USART_MASK & (1 << 9)
#   define VSF_HW_AF_USART9_TX_P0_0         1
#   define VSF_HW_AF_USART9_RX_P0_1         1
#   define VSF_HW_AF_USART9_CTS_P0_2        1
#   define VSF_HW_AF_USART9_RTS_P0_3        1
#   define VSF_HW_AF_USART9_DE_P0_4         1
#   define VSF_HW_AF_USART9_NSS_P0_5        1
#   define VSF_HW_AF_USART9_CK_P0_6         1
#endif
#if VSF_HW_USART_MASK & (1 << 10)
#   define VSF_HW_AF_USART10_TX_P10_0       1
#   define VSF_HW_AF_USART10_RX_P10_1       1
#   define VSF_HW_AF_USART10_CTS_P10_2      1
#   define VSF_HW_AF_USART10_RTS_P10_3      1
#   define VSF_HW_AF_USART10_DE_P10_4       1
#   define VSF_HW_AF_USART10_NSS_P10_5      1
#   define VSF_HW_AF_USART10_CK_P10_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 11)
#   define VSF_HW_AF_USART11_TX_P11_0       1
#   define VSF_HW_AF_USART11_RX_P11_1       1
#   define VSF_HW_AF_USART11_CTS_P11_2      1
#   define VSF_HW_AF_USART11_RTS_P11_3      1
#   define VSF_HW_AF_USART11_DE_P11_4       1
#   define VSF_HW_AF_USART11_NSS_P11_5      1
#   define VSF_HW_AF_USART11_CK_P11_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 12)
#   define VSF_HW_AF_USART12_TX_P12_0       1
#   define VSF_HW_AF_USART12_RX_P12_1       1
#   define VSF_HW_AF_USART12_CTS_P12_2      1
#   define VSF_HW_AF_USART12_RTS_P12_3      1
#   define VSF_HW_AF_USART12_DE_P12_4       1
#   define VSF_HW_AF_USART12_NSS_P12_5      1
#   define VSF_HW_AF_USART12_CK_P12_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 13)
#   define VSF_HW_AF_USART13_TX_P13_0       1
#   define VSF_HW_AF_USART13_RX_P13_1       1
#   define VSF_HW_AF_USART13_CTS_P13_2      1
#   define VSF_HW_AF_USART13_RTS_P13_3      1
#   define VSF_HW_AF_USART13_DE_P13_4       1
#   define VSF_HW_AF_USART13_NSS_P13_5      1
#   define VSF_HW_AF_USART13_CK_P13_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 14)
#   define VSF_HW_AF_USART14_TX_P14_0       1
#   define VSF_HW_AF_USART14_RX_P14_1       1
#   define VSF_HW_AF_USART14_CTS_P14_2      1
#   define VSF_HW_AF_USART14_RTS_P14_3      1
#   define VSF_HW_AF_USART14_DE_P14_4       1
#   define VSF_HW_AF_USART14_NSS_P14_5      1
#   define VSF_HW_AF_USART14_CK_P14_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 15)
#   define VSF_HW_AF_USART15_TX_P15_0       1
#   define VSF_HW_AF_USART15_RX_P15_1       1
#   define VSF_HW_AF_USART15_CTS_P15_2      1
#   define VSF_HW_AF_USART15_RTS_P15_3      1
#   define VSF_HW_AF_USART15_DE_P15_4       1
#   define VSF_HW_AF_USART15_NSS_P15_5      1
#   define VSF_HW_AF_USART15_CK_P15_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 16)
#   define VSF_HW_AF_USART16_TX_P16_0       1
#   define VSF_HW_AF_USART16_RX_P16_1       1
#   define VSF_HW_AF_USART16_CTS_P16_2      1
#   define VSF_HW_AF_USART16_RTS_P16_3      1
#   define VSF_HW_AF_USART16_DE_P16_4       1
#   define VSF_HW_AF_USART16_NSS_P16_5      1
#   define VSF_HW_AF_USART16_CK_P16_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 17)
#   define VSF_HW_AF_USART17_TX_P17_0       1
#   define VSF_HW_AF_USART17_RX_P17_1       1
#   define VSF_HW_AF_USART17_CTS_P17_2      1
#   define VSF_HW_AF_USART17_RTS_P17_3      1
#   define VSF_HW_AF_USART17_DE_P17_4       1
#   define VSF_HW_AF_USART17_NSS_P17_5      1
#   define VSF_HW_AF_USART17_CK_P17_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 18)
#   define VSF_HW_AF_USART18_TX_P18_0       1
#   define VSF_HW_AF_USART18_RX_P18_1       1
#   define VSF_HW_AF_USART18_CTS_P18_2      1
#   define VSF_HW_AF_USART18_RTS_P18_3      1
#   define VSF_HW_AF_USART18_DE_P18_4       1
#   define VSF_HW_AF_USART18_NSS_P18_5      1
#   define VSF_HW_AF_USART18_CK_P18_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 19)
#   define VSF_HW_AF_USART19_TX_P19_0       1
#   define VSF_HW_AF_USART19_RX_P19_1       1
#   define VSF_HW_AF_USART19_CTS_P19_2      1
#   define VSF_HW_AF_USART19_RTS_P19_3      1
#   define VSF_HW_AF_USART19_DE_P19_4       1
#   define VSF_HW_AF_USART19_NSS_P19_5      1
#   define VSF_HW_AF_USART19_CK_P19_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 20)
#   define VSF_HW_AF_USART20_TX_P20_0       1
#   define VSF_HW_AF_USART20_RX_P20_1       1
#   define VSF_HW_AF_USART20_CTS_P20_2      1
#   define VSF_HW_AF_USART20_RTS_P20_3      1
#   define VSF_HW_AF_USART20_DE_P20_4       1
#   define VSF_HW_AF_USART20_NSS_P20_5      1
#   define VSF_HW_AF_USART20_CK_P20_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 21)
#   define VSF_HW_AF_USART21_TX_P21_0       1
#   define VSF_HW_AF_USART21_RX_P21_1       1
#   define VSF_HW_AF_USART21_CTS_P21_2      1
#   define VSF_HW_AF_USART21_RTS_P21_3      1
#   define VSF_HW_AF_USART21_DE_P21_4       1
#   define VSF_HW_AF_USART21_NSS_P21_5      1
#   define VSF_HW_AF_USART21_CK_P21_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 22)
#   define VSF_HW_AF_USART22_TX_P22_0       1
#   define VSF_HW_AF_USART22_RX_P22_1       1
#   define VSF_HW_AF_USART22_CTS_P22_2      1
#   define VSF_HW_AF_USART22_RTS_P22_3      1
#   define VSF_HW_AF_USART22_DE_P22_4       1
#   define VSF_HW_AF_USART22_NSS_P22_5      1
#   define VSF_HW_AF_USART22_CK_P22_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 23)
#   define VSF_HW_AF_USART23_TX_P23_0       1
#   define VSF_HW_AF_USART23_RX_P23_1       1
#   define VSF_HW_AF_USART23_CTS_P23_2      1
#   define VSF_HW_AF_USART23_RTS_P23_3      1
#   define VSF_HW_AF_USART23_DE_P23_4       1
#   define VSF_HW_AF_USART23_NSS_P23_5      1
#   define VSF_HW_AF_USART23_CK_P23_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 24)
#   define VSF_HW_AF_USART24_TX_P24_0       1
#   define VSF_HW_AF_USART24_RX_P24_1       1
#   define VSF_HW_AF_USART24_CTS_P24_2      1
#   define VSF_HW_AF_USART24_RTS_P24_3      1
#   define VSF_HW_AF_USART24_DE_P24_4       1
#   define VSF_HW_AF_USART24_NSS_P24_5      1
#   define VSF_HW_AF_USART24_CK_P24_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 25)
#   define VSF_HW_AF_USART25_TX_P25_0       1
#   define VSF_HW_AF_USART25_RX_P25_1       1
#   define VSF_HW_AF_USART25_CTS_P25_2      1
#   define VSF_HW_AF_USART25_RTS_P25_3      1
#   define VSF_HW_AF_USART25_DE_P25_4       1
#   define VSF_HW_AF_USART25_NSS_P25_5      1
#   define VSF_HW_AF_USART25_CK_P25_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 26)
#   define VSF_HW_AF_USART26_TX_P26_0       1
#   define VSF_HW_AF_USART26_RX_P26_1       1
#   define VSF_HW_AF_USART26_CTS_P26_2      1
#   define VSF_HW_AF_USART26_RTS_P26_3      1
#   define VSF_HW_AF_USART26_DE_P26_4       1
#   define VSF_HW_AF_USART26_NSS_P26_5      1
#   define VSF_HW_AF_USART26_CK_P26_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 27)
#   define VSF_HW_AF_USART27_TX_P27_0       1
#   define VSF_HW_AF_USART27_RX_P27_1       1
#   define VSF_HW_AF_USART27_CTS_P27_2      1
#   define VSF_HW_AF_USART27_RTS_P27_3      1
#   define VSF_HW_AF_USART27_DE_P27_4       1
#   define VSF_HW_AF_USART27_NSS_P27_5      1
#   define VSF_HW_AF_USART27_CK_P27_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 28)
#   define VSF_HW_AF_USART28_TX_P28_0       1
#   define VSF_HW_AF_USART28_RX_P28_1       1
#   define VSF_HW_AF_USART28_CTS_P28_2      1
#   define VSF_HW_AF_USART28_RTS_P28_3      1
#   define VSF_HW_AF_USART28_DE_P28_4       1
#   define VSF_HW_AF_USART28_NSS_P28_5      1
#   define VSF_HW_AF_USART28_CK_P28_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 29)
#   define VSF_HW_AF_USART29_TX_P29_0       1
#   define VSF_HW_AF_USART29_RX_P29_1       1
#   define VSF_HW_AF_USART29_CTS_P29_2      1
#   define VSF_HW_AF_USART29_RTS_P29_3      1
#   define VSF_HW_AF_USART29_DE_P29_4       1
#   define VSF_HW_AF_USART29_NSS_P29_5      1
#   define VSF_HW_AF_USART29_CK_P29_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 30)
#   define VSF_HW_AF_USART30_TX_P30_0       1
#   define VSF_HW_AF_USART30_RX_P30_1       1
#   define VSF_HW_AF_USART30_CTS_P30_2      1
#   define VSF_HW_AF_USART30_RTS_P30_3      1
#   define VSF_HW_AF_USART30_DE_P30_4       1
#   define VSF_HW_AF_USART30_NSS_P30_5      1
#   define VSF_HW_AF_USART30_CK_P30_6       1
#endif
#if VSF_HW_USART_MASK & (1 << 31)
#   define VSF_HW_AF_USART31_TX_P31_0       1
#   define VSF_HW_AF_USART31_RX_P31_1       1
#   define VSF_HW_AF_USART31_CTS_P31_2      1
#   define VSF_HW_AF_USART31_RTS_P31_3      1
#   define VSF_HW_AF_USART31_DE_P31_4       1
#   define VSF_HW_AF_USART31_NSS_P31_5      1
#   define VSF_HW_AF_USART31_CK_P31_6       1
#endif
#endif      // VSF_HW_USART_MASK

#if !defined(VSF_HW_SPI_MASK) && defined(VSF_HW_SPI_COUNT)
#   define VSF_HW_SPI_MASK                  ((1 << VSF_HW_SPI_COUNT) - 1)
#endif

#ifdef VSF_HW_SPI_MASK
#if VSF_HW_SPI_MASK & (1 << 0)
#   define VSF_HW_AF_SPI0_MISO_P0_0         2
#   define VSF_HW_AF_SPI0_MOSI_P0_1         2
#   define VSF_HW_AF_SPI0_SCK_P0_2          2
#   define VSF_HW_AF_SPI0_NSS_P0_3          2
#endif
#if VSF_HW_SPI_MASK & (1 << 1)
#   define VSF_HW_AF_SPI1_MISO_P1_0         2
#   define VSF_HW_AF_SPI1_MOSI_P1_1         2
#   define VSF_HW_AF_SPI1_SCK_P1_2          2
#   define VSF_HW_AF_SPI1_NSS_P1_3          2
#endif
#if VSF_HW_SPI_MASK & (1 << 2)
#   define VSF_HW_AF_SPI2_MISO_P2_0         2
#   define VSF_HW_AF_SPI2_MOSI_P2_1         2
#   define VSF_HW_AF_SPI2_SCK_P2_2          2
#   define VSF_HW_AF_SPI2_NSS_P2_3          2
#endif
#if VSF_HW_SPI_MASK & (1 << 3)
#   define VSF_HW_AF_SPI3_MISO_P3_0         2
#   define VSF_HW_AF_SPI3_MOSI_P3_1         2
#   define VSF_HW_AF_SPI3_SCK_P3_2          2
#   define VSF_HW_AF_SPI3_NSS_P3_3          2
#endif
#if VSF_HW_SPI_MASK & (1 << 4)
#   define VSF_HW_AF_SPI4_MISO_P4_0         2
#   define VSF_HW_AF_SPI4_MOSI_P4_1         2
#   define VSF_HW_AF_SPI4_SCK_P4_2          2
#   define VSF_HW_AF_SPI4_NSS_P4_3          2
#endif
#if VSF_HW_SPI_MASK & (1 << 5)
#   define VSF_HW_AF_SPI5_MISO_P5_0         2
#   define VSF_HW_AF_SPI5_MOSI_P5_1         2
#   define VSF_HW_AF_SPI5_SCK_P5_2          2
#   define VSF_HW_AF_SPI5_NSS_P5_3          2
#endif
#if VSF_HW_SPI_MASK & (1 << 6)
#   define VSF_HW_AF_SPI6_MISO_P6_0         2
#   define VSF_HW_AF_SPI6_MOSI_P6_1         2
#   define VSF_HW_AF_SPI6_SCK_P6_2          2
#   define VSF_HW_AF_SPI6_NSS_P6_3          2
#endif
#if VSF_HW_SPI_MASK & (1 << 7)
#   define VSF_HW_AF_SPI7_MISO_P7_0         2
#   define VSF_HW_AF_SPI7_MOSI_P7_1         2
#   define VSF_HW_AF_SPI7_SCK_P7_2          2
#   define VSF_HW_AF_SPI7_NSS_P7_3          2
#endif
#if VSF_HW_SPI_MASK & (1 << 8)
#   define VSF_HW_AF_SPI8_MISO_P8_0         2
#   define VSF_HW_AF_SPI8_MOSI_P8_1         2
#   define VSF_HW_AF_SPI8_SCK_P8_2          2
#   define VSF_HW_AF_SPI8_NSS_P8_3          2
#endif
#if VSF_HW_SPI_MASK & (1 << 9)
#   define VSF_HW_AF_SPI9_MISO_P9_0         2
#   define VSF_HW_AF_SPI9_MOSI_P9_1         2
#   define VSF_HW_AF_SPI9_SCK_P9_2          2
#   define VSF_HW_AF_SPI9_NSS_P9_3          2
#endif
#if VSF_HW_SPI_MASK & (1 << 10)
#   define VSF_HW_AF_SPI10_MISO_P10_0       2
#   define VSF_HW_AF_SPI10_MOSI_P10_1       2
#   define VSF_HW_AF_SPI10_SCK_P10_2        2
#   define VSF_HW_AF_SPI10_NSS_P10_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 11)
#   define VSF_HW_AF_SPI11_MISO_P11_0       2
#   define VSF_HW_AF_SPI11_MOSI_P11_1       2
#   define VSF_HW_AF_SPI11_SCK_P11_2        2
#   define VSF_HW_AF_SPI11_NSS_P11_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 12)
#   define VSF_HW_AF_SPI12_MISO_P12_0       2
#   define VSF_HW_AF_SPI12_MOSI_P12_1       2
#   define VSF_HW_AF_SPI12_SCK_P12_2        2
#   define VSF_HW_AF_SPI12_NSS_P12_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 13)
#   define VSF_HW_AF_SPI13_MISO_P13_0       2
#   define VSF_HW_AF_SPI13_MOSI_P13_1       2
#   define VSF_HW_AF_SPI13_SCK_P13_2        2
#   define VSF_HW_AF_SPI13_NSS_P13_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 14)
#   define VSF_HW_AF_SPI14_MISO_P14_0       2
#   define VSF_HW_AF_SPI14_MOSI_P14_1       2
#   define VSF_HW_AF_SPI14_SCK_P14_2        2
#   define VSF_HW_AF_SPI14_NSS_P14_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 15)
#   define VSF_HW_AF_SPI15_MISO_P15_0       2
#   define VSF_HW_AF_SPI15_MOSI_P15_1       2
#   define VSF_HW_AF_SPI15_SCK_P15_2        2
#   define VSF_HW_AF_SPI15_NSS_P15_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 16)
#   define VSF_HW_AF_SPI16_MISO_P16_0       2
#   define VSF_HW_AF_SPI16_MOSI_P16_1       2
#   define VSF_HW_AF_SPI16_SCK_P16_2        2
#   define VSF_HW_AF_SPI16_NSS_P16_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 17)
#   define VSF_HW_AF_SPI17_MISO_P17_0       2
#   define VSF_HW_AF_SPI17_MOSI_P17_1       2
#   define VSF_HW_AF_SPI17_SCK_P17_2        2
#   define VSF_HW_AF_SPI17_NSS_P17_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 18)
#   define VSF_HW_AF_SPI18_MISO_P18_0       2
#   define VSF_HW_AF_SPI18_MOSI_P18_1       2
#   define VSF_HW_AF_SPI18_SCK_P18_2        2
#   define VSF_HW_AF_SPI18_NSS_P18_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 19)
#   define VSF_HW_AF_SPI19_MISO_P19_0       2
#   define VSF_HW_AF_SPI19_MOSI_P19_1       2
#   define VSF_HW_AF_SPI19_SCK_P19_2        2
#   define VSF_HW_AF_SPI19_NSS_P19_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 20)
#   define VSF_HW_AF_SPI20_MISO_P20_0       2
#   define VSF_HW_AF_SPI20_MOSI_P20_1       2
#   define VSF_HW_AF_SPI20_SCK_P20_2        2
#   define VSF_HW_AF_SPI20_NSS_P20_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 21)
#   define VSF_HW_AF_SPI21_MISO_P21_0       2
#   define VSF_HW_AF_SPI21_MOSI_P21_1       2
#   define VSF_HW_AF_SPI21_SCK_P21_2        2
#   define VSF_HW_AF_SPI21_NSS_P21_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 22)
#   define VSF_HW_AF_SPI22_MISO_P22_0       2
#   define VSF_HW_AF_SPI22_MOSI_P22_1       2
#   define VSF_HW_AF_SPI22_SCK_P22_2        2
#   define VSF_HW_AF_SPI22_NSS_P22_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 23)
#   define VSF_HW_AF_SPI23_MISO_P23_0       2
#   define VSF_HW_AF_SPI23_MOSI_P23_1       2
#   define VSF_HW_AF_SPI23_SCK_P23_2        2
#   define VSF_HW_AF_SPI23_NSS_P23_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 24)
#   define VSF_HW_AF_SPI24_MISO_P24_0       2
#   define VSF_HW_AF_SPI24_MOSI_P24_1       2
#   define VSF_HW_AF_SPI24_SCK_P24_2        2
#   define VSF_HW_AF_SPI24_NSS_P24_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 25)
#   define VSF_HW_AF_SPI25_MISO_P25_0       2
#   define VSF_HW_AF_SPI25_MOSI_P25_1       2
#   define VSF_HW_AF_SPI25_SCK_P25_2        2
#   define VSF_HW_AF_SPI25_NSS_P25_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 26)
#   define VSF_HW_AF_SPI26_MISO_P26_0       2
#   define VSF_HW_AF_SPI26_MOSI_P26_1       2
#   define VSF_HW_AF_SPI26_SCK_P26_2        2
#   define VSF_HW_AF_SPI26_NSS_P26_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 27)
#   define VSF_HW_AF_SPI27_MISO_P27_0       2
#   define VSF_HW_AF_SPI27_MOSI_P27_1       2
#   define VSF_HW_AF_SPI27_SCK_P27_2        2
#   define VSF_HW_AF_SPI27_NSS_P27_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 28)
#   define VSF_HW_AF_SPI28_MISO_P28_0       2
#   define VSF_HW_AF_SPI28_MOSI_P28_1       2
#   define VSF_HW_AF_SPI28_SCK_P28_2        2
#   define VSF_HW_AF_SPI28_NSS_P28_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 29)
#   define VSF_HW_AF_SPI29_MISO_P29_0       2
#   define VSF_HW_AF_SPI29_MOSI_P29_1       2
#   define VSF_HW_AF_SPI29_SCK_P29_2        2
#   define VSF_HW_AF_SPI29_NSS_P29_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 30)
#   define VSF_HW_AF_SPI30_MISO_P30_0       2
#   define VSF_HW_AF_SPI30_MOSI_P30_1       2
#   define VSF_HW_AF_SPI30_SCK_P30_2        2
#   define VSF_HW_AF_SPI30_NSS_P30_3        2
#endif
#if VSF_HW_SPI_MASK & (1 << 31)
#   define VSF_HW_AF_SPI31_MISO_P31_0       2
#   define VSF_HW_AF_SPI31_MOSI_P31_1       2
#   define VSF_HW_AF_SPI31_SCK_P31_2        2
#   define VSF_HW_AF_SPI31_NSS_P31_3        2
#endif
#endif

#if !defined(VSF_HW_I2C_MASK) && defined(VSF_HW_I2C_COUNT)
#   define VSF_HW_I2C_MASK                  ((1 << VSF_HW_I2C_COUNT) - 1)
#endif

#ifdef VSF_HW_I2C_MASK
#if VSF_HW_I2C_MASK & (1 << 0)
#   define VSF_HW_AF_I2C0_SCL_P0_0          3
#   define VSF_HW_AF_I2C0_SDA_P0_1          3
#   define VSF_HW_AF_I2C0_SMBA_P0_2         3
#endif
#if VSF_HW_I2C_MASK & (1 << 11)
#   define VSF_HW_AF_I2C1_SCL_P1_0          3
#   define VSF_HW_AF_I2C1_SDA_P1_1          3
#   define VSF_HW_AF_I2C1_SMBA_P1_2         3
#endif
#if VSF_HW_I2C_MASK & (1 << 2)
#   define VSF_HW_AF_I2C2_SCL_P2_0          3
#   define VSF_HW_AF_I2C2_SDA_P2_1          3
#   define VSF_HW_AF_I2C2_SMBA_P2_2         3
#endif
#if VSF_HW_I2C_MASK & (1 << 3)
#   define VSF_HW_AF_I2C3_SCL_P3_0          3
#   define VSF_HW_AF_I2C3_SDA_P3_1          3
#   define VSF_HW_AF_I2C3_SMBA_P3_2         3
#endif
#if VSF_HW_I2C_MASK & (1 << 4)
#   define VSF_HW_AF_I2C4_SCL_P4_0          3
#   define VSF_HW_AF_I2C4_SDA_P4_1          3
#   define VSF_HW_AF_I2C4_SMBA_P4_2         3
#endif
#if VSF_HW_I2C_MASK & (1 << 5)
#   define VSF_HW_AF_I2C5_SCL_P5_0          3
#   define VSF_HW_AF_I2C5_SDA_P5_1          3
#   define VSF_HW_AF_I2C5_SMBA_P5_2         3
#endif
#if VSF_HW_I2C_MASK & (1 << 6)
#   define VSF_HW_AF_I2C6_SCL_P6_0          3
#   define VSF_HW_AF_I2C6_SDA_P6_1          3
#   define VSF_HW_AF_I2C6_SMBA_P6_2         3
#endif
#if VSF_HW_I2C_MASK & (1 << 7)
#   define VSF_HW_AF_I2C7_SCL_P7_0          3
#   define VSF_HW_AF_I2C7_SDA_P7_1          3
#   define VSF_HW_AF_I2C7_SMBA_P7_2         3
#endif
#if VSF_HW_I2C_MASK & (1 << 8)
#   define VSF_HW_AF_I2C8_SCL_P8_0          3
#   define VSF_HW_AF_I2C8_SDA_P8_1          3
#   define VSF_HW_AF_I2C8_SMBA_P8_2         3
#endif
#if VSF_HW_I2C_MASK & (1 << 9)
#   define VSF_HW_AF_I2C9_SCL_P9_0          3
#   define VSF_HW_AF_I2C9_SDA_P9_1          3
#   define VSF_HW_AF_I2C9_SMBA_P9_2         3
#endif
#if VSF_HW_I2C_MASK & (1 << 10)
#   define VSF_HW_AF_I2C10_SCL_P10_0        3
#   define VSF_HW_AF_I2C10_SDA_P10_1        3
#   define VSF_HW_AF_I2C10_SMBA_P10_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 11)
#   define VSF_HW_AF_I2C11_SCL_P11_0        3
#   define VSF_HW_AF_I2C11_SDA_P11_1        3
#   define VSF_HW_AF_I2C11_SMBA_P11_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 12)
#   define VSF_HW_AF_I2C12_SCL_P12_0        3
#   define VSF_HW_AF_I2C12_SDA_P12_1        3
#   define VSF_HW_AF_I2C12_SMBA_P12_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 13)
#   define VSF_HW_AF_I2C13_SCL_P13_0        3
#   define VSF_HW_AF_I2C13_SDA_P13_1        3
#   define VSF_HW_AF_I2C13_SMBA_P13_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 14)
#   define VSF_HW_AF_I2C14_SCL_P14_0        3
#   define VSF_HW_AF_I2C14_SDA_P14_1        3
#   define VSF_HW_AF_I2C14_SMBA_P14_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 15)
#   define VSF_HW_AF_I2C15_SCL_P15_0        3
#   define VSF_HW_AF_I2C15_SDA_P15_1        3
#   define VSF_HW_AF_I2C15_SMBA_P15_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 16)
#   define VSF_HW_AF_I2C16_SCL_P16_0        3
#   define VSF_HW_AF_I2C16_SDA_P16_1        3
#   define VSF_HW_AF_I2C16_SMBA_P16_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 17)
#   define VSF_HW_AF_I2C17_SCL_P17_0        3
#   define VSF_HW_AF_I2C17_SDA_P17_1        3
#   define VSF_HW_AF_I2C17_SMBA_P17_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 18)
#   define VSF_HW_AF_I2C18_SCL_P18_0        3
#   define VSF_HW_AF_I2C18_SDA_P18_1        3
#   define VSF_HW_AF_I2C18_SMBA_P18_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 19)
#   define VSF_HW_AF_I2C19_SCL_P19_0        3
#   define VSF_HW_AF_I2C19_SDA_P19_1        3
#   define VSF_HW_AF_I2C19_SMBA_P19_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 20)
#   define VSF_HW_AF_I2C20_SCL_P20_0        3
#   define VSF_HW_AF_I2C20_SDA_P20_1        3
#   define VSF_HW_AF_I2C20_SMBA_P20_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 21)
#   define VSF_HW_AF_I2C21_SCL_P21_0        3
#   define VSF_HW_AF_I2C21_SDA_P21_1        3
#   define VSF_HW_AF_I2C21_SMBA_P21_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 22)
#   define VSF_HW_AF_I2C22_SCL_P22_0        3
#   define VSF_HW_AF_I2C22_SDA_P22_1        3
#   define VSF_HW_AF_I2C22_SMBA_P22_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 23)
#   define VSF_HW_AF_I2C23_SCL_P23_0        3
#   define VSF_HW_AF_I2C23_SDA_P23_1        3
#   define VSF_HW_AF_I2C23_SMBA_P23_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 24)
#   define VSF_HW_AF_I2C24_SCL_P24_0        3
#   define VSF_HW_AF_I2C24_SDA_P24_1        3
#   define VSF_HW_AF_I2C24_SMBA_P24_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 25)
#   define VSF_HW_AF_I2C25_SCL_P25_0        3
#   define VSF_HW_AF_I2C25_SDA_P25_1        3
#   define VSF_HW_AF_I2C25_SMBA_P25_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 26)
#   define VSF_HW_AF_I2C26_SCL_P26_0        3
#   define VSF_HW_AF_I2C26_SDA_P26_1        3
#   define VSF_HW_AF_I2C26_SMBA_P26_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 27)
#   define VSF_HW_AF_I2C27_SCL_P27_0        3
#   define VSF_HW_AF_I2C27_SDA_P27_1        3
#   define VSF_HW_AF_I2C27_SMBA_P27_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 28)
#   define VSF_HW_AF_I2C28_SCL_P28_0        3
#   define VSF_HW_AF_I2C28_SDA_P28_1        3
#   define VSF_HW_AF_I2C28_SMBA_P28_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 29)
#   define VSF_HW_AF_I2C29_SCL_P29_0        3
#   define VSF_HW_AF_I2C29_SDA_P29_1        3
#   define VSF_HW_AF_I2C29_SMBA_P29_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 30)
#   define VSF_HW_AF_I2C30_SCL_P30_0        3
#   define VSF_HW_AF_I2C30_SDA_P30_1        3
#   define VSF_HW_AF_I2C30_SMBA_P30_2       3
#endif
#if VSF_HW_I2C_MASK & (1 << 31)
#   define VSF_HW_AF_I2C31_SCL_P31_0        3
#   define VSF_HW_AF_I2C31_SDA_P31_1        3
#   define VSF_HW_AF_I2C31_SMBA_P31_2       3
#endif
#endif
