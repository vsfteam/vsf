
#define VSF_HW_INTERRUPTS_NUM           160

// VSF_HW_INTERRUPT0 .. VSF_HW_INTERRUPT31 are for GPIO/EXTI
#define VSF_HW_gpio_IRQN                0
#define VSF_HW_exti_IRQN                0

#if !defined(VSF_HW_USART_MASK) && defined(VSF_HW_USART_COUNT)
#   define VSF_HW_USART_MASK            ((1 << VSF_HW_USART_COUNT) - 1)
#endif

// VSF_HW_INTERRUPT32 .. VSF_HW_INTERRUPT63 are for USART
#define VSF_HW_usart_IRQN               32
#ifdef VSF_HW_USART_MASK
#if VSF_HW_USART_MASK & (1 << 0)
#   define VSF_HW_INTERRUPT32           USART0_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 1)
#   define VSF_HW_INTERRUPT33           USART1_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 2)
#   define VSF_HW_INTERRUPT34           USART2_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 3)
#   define VSF_HW_INTERRUPT35           USART3_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 4)
#   define VSF_HW_INTERRUPT36           USART4_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 5)
#   define VSF_HW_INTERRUPT37           USART5_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 6)
#   define VSF_HW_INTERRUPT38           USART6_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 7)
#   define VSF_HW_INTERRUPT39           USART7_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 8)
#   define VSF_HW_INTERRUPT40           USART8_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 9)
#   define VSF_HW_INTERRUPT41           USART9_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 10)
#   define VSF_HW_INTERRUPT42           USART10_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 11)
#   define VSF_HW_INTERRUPT43           USART11_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 12)
#   define VSF_HW_INTERRUPT44           USART12_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 13)
#   define VSF_HW_INTERRUPT45           USART13_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 14)
#   define VSF_HW_INTERRUPT46           USART14_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 15)
#   define VSF_HW_INTERRUPT47           USART15_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 16)
#   define VSF_HW_INTERRUPT48           USART16_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 17)
#   define VSF_HW_INTERRUPT49           USART17_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 18)
#   define VSF_HW_INTERRUPT50           USART18_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 19)
#   define VSF_HW_INTERRUPT51           USART19_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 20)
#   define VSF_HW_INTERRUPT52           USART20_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 21)
#   define VSF_HW_INTERRUPT53           USART21_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 22)
#   define VSF_HW_INTERRUPT54           USART22_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 23)
#   define VSF_HW_INTERRUPT55           USART23_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 24)
#   define VSF_HW_INTERRUPT56           USART24_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 25)
#   define VSF_HW_INTERRUPT57           USART25_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 26)
#   define VSF_HW_INTERRUPT58           USART26_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 27)
#   define VSF_HW_INTERRUPT59           USART27_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 28)
#   define VSF_HW_INTERRUPT60           USART28_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 29)
#   define VSF_HW_INTERRUPT61           USART29_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 30)
#   define VSF_HW_INTERRUPT62           USART30_IRQHandler
#endif
#if VSF_HW_USART_MASK & (1 << 31)
#   define VSF_HW_INTERRUPT63           USART31_IRQHandler
#endif
#endif

#if !defined(VSF_HW_SPI_MASK) && defined(VSF_HW_SPI_COUNT)
#   define VSF_HW_SPI_MASK              ((1 << VSF_HW_SPI_COUNT) - 1)
#endif

// VSF_HW_INTERRUPT64 .. VSF_HW_INTERRUPT95 are for SPI
#define VSF_HW_spi_IRQN                 64
#ifdef VSF_HW_SPI_MASK
#if VSF_HW_SPI_MASK & (1 << 0)
#   define VSF_HW_INTERRUPT64           SPI0_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 1)
#   define VSF_HW_INTERRUPT65           SPI1_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 2)
#   define VSF_HW_INTERRUPT66           SPI2_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 3)
#   define VSF_HW_INTERRUPT67           SPI3_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 4)
#   define VSF_HW_INTERRUPT68           SPI4_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 5)
#   define VSF_HW_INTERRUPT69           SPI5_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 6)
#   define VSF_HW_INTERRUPT70           SPI6_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 7)
#   define VSF_HW_INTERRUPT71           SPI7_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 8)
#   define VSF_HW_INTERRUPT72           SPI8_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 9)
#   define VSF_HW_INTERRUPT73           SPI9_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 10)
#   define VSF_HW_INTERRUPT74           SPI10_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 11)
#   define VSF_HW_INTERRUPT75           SPI11_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 12)
#   define VSF_HW_INTERRUPT76           SPI12_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 13)
#   define VSF_HW_INTERRUPT77           SPI13_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 14)
#   define VSF_HW_INTERRUPT78           SPI14_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 15)
#   define VSF_HW_INTERRUPT79           SPI15_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 16)
#   define VSF_HW_INTERRUPT80           SPI16_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 17)
#   define VSF_HW_INTERRUPT81           SPI17_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 18)
#   define VSF_HW_INTERRUPT82           SPI18_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 19)
#   define VSF_HW_INTERRUPT83           SPI19_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 20)
#   define VSF_HW_INTERRUPT84           SPI20_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 21)
#   define VSF_HW_INTERRUPT85           SPI21_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 22)
#   define VSF_HW_INTERRUPT86           SPI22_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 23)
#   define VSF_HW_INTERRUPT87           SPI23_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 24)
#   define VSF_HW_INTERRUPT88           SPI24_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 25)
#   define VSF_HW_INTERRUPT89           SPI25_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 26)
#   define VSF_HW_INTERRUPT90           SPI26_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 27)
#   define VSF_HW_INTERRUPT91           SPI27_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 28)
#   define VSF_HW_INTERRUPT92           SPI28_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 29)
#   define VSF_HW_INTERRUPT93           SPI29_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 30)
#   define VSF_HW_INTERRUPT94           SPI30_IRQHandler
#endif
#if VSF_HW_SPI_MASK & (1 << 31)
#   define VSF_HW_INTERRUPT95           SPI31_IRQHandler
#endif
#endif

#if !defined(VSF_HW_I2C_MASK) && defined(VSF_HW_I2C_COUNT)
#   define VSF_HW_I2C_MASK              ((1 << VSF_HW_I2C_COUNT) - 1)
#endif

// VSF_HW_INTERRUPT96 .. VSF_HW_INTERRUPT127 are for I2C
#define VSF_HW_i2c_IRQN                 96
#ifdef VSF_HW_I2C_MASK
#if VSF_HW_I2C_MASK & (1 << 0)
#   define VSF_HW_INTERRUPT96           I2C0_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 1)
#   define VSF_HW_INTERRUPT97           I2C1_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 2)
#   define VSF_HW_INTERRUPT98           I2C2_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 3)
#   define VSF_HW_INTERRUPT99           I2C3_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 4)
#   define VSF_HW_INTERRUPT100          I2C4_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 5)
#   define VSF_HW_INTERRUPT101          I2C5_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 6)
#   define VSF_HW_INTERRUPT102          I2C6_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 7)
#   define VSF_HW_INTERRUPT102          I2C7_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 8)
#   define VSF_HW_INTERRUPT104          I2C8_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 9)
#   define VSF_HW_INTERRUPT105          I2C9_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 10)
#   define VSF_HW_INTERRUPT106          I2C10_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 11)
#   define VSF_HW_INTERRUPT107          I2C11_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 12)
#   define VSF_HW_INTERRUPT108          I2C12_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 13)
#   define VSF_HW_INTERRUPT109          I2C13_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 14)
#   define VSF_HW_INTERRUPT110          I2C14_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 15)
#   define VSF_HW_INTERRUPT111          I2C15_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 16)
#   define VSF_HW_INTERRUPT112          I2C16_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 17)
#   define VSF_HW_INTERRUPT113          I2C17_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 18)
#   define VSF_HW_INTERRUPT114          I2C18_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 19)
#   define VSF_HW_INTERRUPT115          I2C19_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 20)
#   define VSF_HW_INTERRUPT116          I2C20_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 21)
#   define VSF_HW_INTERRUPT117          I2C21_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 22)
#   define VSF_HW_INTERRUPT118          I2C22_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 23)
#   define VSF_HW_INTERRUPT119          I2C23_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 24)
#   define VSF_HW_INTERRUPT120          I2C24_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 25)
#   define VSF_HW_INTERRUPT121          I2C25_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 26)
#   define VSF_HW_INTERRUPT122          I2C26_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 27)
#   define VSF_HW_INTERRUPT123          I2C27_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 28)
#   define VSF_HW_INTERRUPT124          I2C28_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 29)
#   define VSF_HW_INTERRUPT125          I2C29_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 30)
#   define VSF_HW_INTERRUPT126          I2C30_IRQHandler
#endif
#if VSF_HW_I2C_MASK & (1 << 31)
#   define VSF_HW_INTERRUPT127          I2C31_IRQHandler
#endif
#endif

// VSF_HW_INTERRUPT128 .. VSF_HW_INTERRUPT159 are for SDIO
#define VSF_HW_sdio_IRQN                128