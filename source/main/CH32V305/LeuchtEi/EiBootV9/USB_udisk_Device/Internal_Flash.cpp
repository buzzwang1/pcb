/********************************** (C) COPYRIGHT *******************************
 * File Name          : Internal_Flash.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/08
 * Description        : Internal Flash program
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v30x.h"
#include "Internal_Flash.h"
#include "ch32v30x_flash.h"

void IFlash_Prog_4096(uint32_t address,uint32_t *pbuf)
{
    address &= 0x00FFFFFF;
    address |= 0x08000000;
    __disable_irq();
    FLASH_Unlock_Fast();

    for (uint32_t t = 0; t < 4096 / INTERNAL_FLASH_PAGE_SIZE; t++)
    {
      //256 Byte
      FLASH_ErasePage_Fast(address);
      FLASH_ProgramPage_Fast(address, pbuf);
      
      address += INTERNAL_FLASH_PAGE_SIZE;
      pbuf    += INTERNAL_FLASH_PAGE_SIZE / 4;
    }
    FLASH_Lock_Fast();
    __enable_irq();
}
