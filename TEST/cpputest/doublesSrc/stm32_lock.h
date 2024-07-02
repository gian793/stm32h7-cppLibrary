#ifndef __STM32_LOCK_H__
#define __STM32_LOCK_H__

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

typedef struct
{
  uint8_t flag; /**< Backup of PRIMASK.PM at nesting level 0 */
  uint8_t counter; /**< Nesting level */
} LockingData_t;


static inline void stm32_lock_init(LockingData_t *lock)
{
  lock->flag = 0;
  lock->counter = 0;
}

static inline void stm32_lock_acquire(LockingData_t *lock)
{
  uint8_t flag = 1; //(uint8_t)(__get_PRIMASK() & 0x1); /* PRIMASK.PM */

  if (lock->counter == 0)
  {
    lock->flag = flag;
  }
  else if (lock->counter == UINT8_MAX)
  {
	  //STM32_LOCK_BLOCK();
    while(1);
  }
  
  lock->counter++;
}

/**
  * @brief Release STM32 lock
  * @param lock The lock to release
  */
static inline void stm32_lock_release(LockingData_t *lock)
{
  if (lock->counter == 0)
  {
    //STM32_LOCK_BLOCK();
    while(1);
  }
  lock->counter--;
  if (lock->counter == 0 && lock->flag == 0)
  {
    //__enable_irq();
  }
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STM32_LOCK_H__ */
