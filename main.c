#include <stdint.h>
#include "stm32f10x.h"

static void delay(uint32_t x)
{
    volatile int i;

    for (i = 0; i < x; i++) ;
}

int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* GPIOD Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    /* Configure PD0 and PD2 in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    int i = 0;
    while (1)
    {
        unsigned reg;

        if (i++ % 2) {
            GPIOA->BSRR = 0x0000FFFF;
            GPIOB->BSRR = 0x0000FFFF;
            GPIOC->BSRR = 0x0000FFFF;
            GPIOD->BSRR = 0x0000FFFF;
        } else {
            GPIOA->BRR  = 0x0000FFFF;
            GPIOB->BRR  = 0x0000FFFF;
            GPIOC->BRR  = 0x0000FFFF;
            GPIOD->BRR  = 0x0000FFFF;

        }

        delay(500000);
    }

}
