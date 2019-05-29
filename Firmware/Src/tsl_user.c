/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : tsl_user.c.c
  * Description        : User configuration file for TOUCHSENSING
  *                      middleWare.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include "tsl_user.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/*============================================================================*/
/* Channels                                                                   */
/*============================================================================*/

/* Source and Configuration (ROM) */
CONST TSL_ChannelSrc_T MyChannels_Src[TSLPRM_TOTAL_CHANNELS] =
{
    { CHANNEL_0_SRC, CHANNEL_0_IO_MSK, CHANNEL_0_GRP_MSK }
    { CHANNEL_-1_SRC, CHANNEL_-1_IO_MSK, CHANNEL_-1_GRP_MSK },
};

/* Destination (ROM) */
CONST TSL_ChannelDest_T MyChannels_Dest[TSLPRM_TOTAL_CHANNELS] =
{
    { CHANNEL_0_DEST }
    { CHANNEL_-1_DEST },
};

/* Data (RAM) */
TSL_ChannelData_T MyChannels_Data[TSLPRM_TOTAL_CHANNELS];

/*============================================================================*/
/* Banks                                                                      */
/*============================================================================*/

/* List (ROM) */
CONST TSL_Bank_T MyBanks[TSLPRM_TOTAL_BANKS] = {
};

/*============================================================================*/
/* Generic Objects                                                            */
/*============================================================================*/

/* List (ROM) */
CONST TSL_Object_T MyObjects[TSLPRM_TOTAL_OBJECTS] =
{
};

/* Group (RAM) */
TSL_ObjectGroup_T MyObjGroup =
{
  &MyObjects[0],        /* First object */
  TSLPRM_TOTAL_OBJECTS, /* Number of objects */
  0x00,                 /* State mask reset value */
  TSL_STATE_NOT_CHANGED /* Current state */
};

/*============================================================================*/
/* TSL Common Parameters placed in RAM or ROM                                 */
/* --> external declaration in tsl_conf.h                                     */
/*============================================================================*/

TSL_Params_T TSL_Params =
{
  TSLPRM_ACQ_MIN,
  TSLPRM_ACQ_MAX,
  TSLPRM_CALIB_SAMPLES,
  TSLPRM_DTO,
#if TSLPRM_TOTAL_TKEYS > 0  
  MyTKeys_StateMachine,   /* Default state machine for TKeys */
  &MyTKeys_Methods,       /* Default methods for TKeys */
#endif
#if TSLPRM_TOTAL_LNRTS > 0
  MyLinRots_StateMachine, /* Default state machine for LinRots */
  &MyLinRots_Methods      /* Default methods for LinRots */
#endif
};

/* Private functions prototype -----------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
/* USER CODE BEGIN Global variables */

/* USER CODE END Global variables */

__IO TSL_tTick_ms_T ECSLastTick; /* Hold the last time value for ECS */

/**
  * @brief  Initialize the STMTouch Driver
  * @param  None
  * @retval None
  */
void tsl_user_Init(void)
{
  TSL_obj_GroupInit(&MyObjGroup); /* Init Objects */
  
  TSL_Init(MyBanks); /* Init acquisition module */
  
  tsl_user_SetThresholds(); /* Init thresholds for each object individually (optional) */
}

/**
  * @brief  Execute STMTouch Driver main State machine
  * @param  None
  * @retval status Return TSL_STATUS_OK if the acquisition is done
  */
tsl_user_status_t tsl_user_Exec(void)
{
  static uint32_t idx_bank = 0;
  static uint32_t config_done = 0;
  tsl_user_status_t status = TSL_USER_STATUS_BUSY;

  /* Configure and start bank acquisition */
  if (!config_done)
  {
/* USER CODE BEGIN not config_done start*/

/* USER CODE END not config_done start*/
    TSL_acq_BankConfig(idx_bank);
    TSL_acq_BankStartAcq();
    config_done = 1;
/* USER CODE BEGIN not config_done */

/* USER CODE END not config_done */
  }

  /* Check end of acquisition (polling mode) and read result */
  if (TSL_acq_BankWaitEOC() == TSL_STATUS_OK)
  {
/* USER CODE BEGIN end of acquisition start*/

/* USER CODE END end of acquisition start*/
    STMSTUDIO_LOCK;
    TSL_acq_BankGetResult(idx_bank, 0, 0);
    STMSTUDIO_UNLOCK;
    idx_bank++; /* Next bank */
    config_done = 0;
/* USER CODE BEGIN end of acquisition */

/* USER CODE END end of acquisition */
  }

  /* Process objects, DxS and ECS
     Check if all banks have been acquired
  */
  if (idx_bank > TSLPRM_TOTAL_BANKS-1)
  {
/* USER CODE BEGIN before reset*/

/* USER CODE END before reset*/
    /* Reset flags for next banks acquisition */
    idx_bank = 0;
    config_done = 0;
    
    /* Process Objects */
    TSL_obj_GroupProcess(&MyObjGroup);
    
    /* DxS processing (if TSLPRM_USE_DXS option is set) */
    TSL_dxs_FirstObj(&MyObjGroup);
    
    /* ECS every TSLPRM_ECS_DELAY (in ms) */
    if (TSL_tim_CheckDelay_ms(TSLPRM_ECS_DELAY, &ECSLastTick) == TSL_STATUS_OK)
    {
      if (TSL_ecs_Process(&MyObjGroup) == TSL_STATUS_OK)
      {
        status = TSL_USER_STATUS_OK_ECS_ON;
      }
      else
      {
        status = TSL_USER_STATUS_OK_ECS_OFF;
      }
    }
    else
    {
      status = TSL_USER_STATUS_OK_NO_ECS;
    }
/* USER CODE BEGIN Process objects */

/* USER CODE END Process objects */
  }
  else
  {
    status = TSL_USER_STATUS_BUSY;
/* USER CODE BEGIN TSL_USER_STATUS_BUSY */

/* USER CODE END TSL_USER_STATUS_BUSY */
  }
  
  return status;
}

/**
  * @brief  Set thresholds for each object (optional).
  * @param  None
  * @retval None
  */
void tsl_user_SetThresholds(void)
{
/* USER CODE BEGIN Tsl_user_SetThresholds */
  /* Example: Decrease the Detect thresholds for the TKEY 0
  MyTKeys_Param[0].DetectInTh -= 10;
  MyTKeys_Param[0].DetectOutTh -= 10;
  */
/* USER CODE END Tsl_user_SetThresholds */
  }

/**
  * @brief  Executed when a sensor is in Error state
  * @param  None
  * @retval None
  */

/**
  * @brief  Executed when a sensor is in Off state
  * @param  None
  * @retval None
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
