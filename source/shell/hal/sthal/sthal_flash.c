/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal.h"
#include "sthal.h"

#if defined(HAL_FLASH_MODULE_ENABLED) && VSF_HAL_USE_FLASH == ENABLED

/*============================ MACROS ========================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

FLASH_ProcessTypeDef pFlash;

/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout);

/*============================ IMPLEMENTATION ================================*/

HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address,
                                    uint64_t Data)
{
    if (TypeProgram == 0) {
        return HAL_ERROR;
    }

    if (Address == 0) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t Address,
                                       uint64_t Data)
{
    return HAL_OK;
}

void HAL_FLASH_IRQHandler(void) {}

VSF_CAL_WEAK(HAL_FLASH_EndOfOperationCallback)
void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
    VSF_UNUSED_PARAM(ReturnValue);
}

VSF_CAL_WEAK(HAL_FLASH_OperationErrorCallback)
void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue)
{
    VSF_UNUSED_PARAM(ReturnValue);
}

HAL_StatusTypeDef HAL_FLASH_Unlock(void)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_FLASH_Lock(void)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_FLASH_OB_Unlock(void)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_FLASH_OB_Lock(void)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_FLASH_OB_Launch(void)
{
    return HAL_OK;
}

uint32_t HAL_FLASH_GetError(void)
{
    return pFlash.ErrorCode;
}

HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout)
{
    return HAL_OK;
}

#endif
