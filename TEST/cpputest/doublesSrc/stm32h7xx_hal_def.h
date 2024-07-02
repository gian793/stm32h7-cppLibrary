#ifndef STM32H7xx_HAL_DEFS_H
#define STM32H7xx_HAL_DEFS_H


#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/**
  * @brief  HAL Status structures definition
  */
typedef enum
{
  HAL_OK       = 0x00,
  HAL_ERROR    = 0x01,
  HAL_BUSY     = 0x02,
  HAL_TIMEOUT  = 0x03
} HAL_StatusTypeDef;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STM32H7xx_HAL_DEFS_H */
