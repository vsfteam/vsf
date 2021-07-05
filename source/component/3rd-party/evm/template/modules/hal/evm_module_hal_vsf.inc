#define EVM_MODULE_HAL_SUPPORT_GPIO         1

typedef struct evm_hal_gpio_dev_t {
    implement(_evm_hal_gpio_dev_t)

    // hal specific member below
} evm_hal_gpio_dev_t;

int evm_hal_gpio_open(evm_hal_gpio_dev_t *gpio_dev)
{
    return 0;
}

void evm_hal_gpio_close(evm_hal_gpio_dev_t *gpio_dev)
{

}

void evm_hal_gpio_direction(evm_hal_gpio_dev_t *gpio_dev)
{

}

void evm_hal_gpio_write(evm_hal_gpio_dev_t *gpio_dev, int value)
{

}

int evm_hal_gpio_read(evm_hal_gpio_dev_t *gpio_dev)
{
    return 0;
}
