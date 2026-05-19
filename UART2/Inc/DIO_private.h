/*
 * DIO_private.h
 *
 *  Created on: Dec 15, 2024
 *      Author: user
 */

#ifndef DIO_PRIVATE_H
#define DIO_PRIVATE_H

#define PortA_address 	0x40010800// page 51
#define PortB_address 	0x40010C00
#define PortC_address   0x40011000
#define PortD_address 	0x40011400
#define PortE_address 	0x40011800
#define PortF_address 	0x40011C00
#define PortG_address 	0x40012000

		/* PORT A registers */
#define GPIOA_CRL 	*((u32*)(PortA_address + 0x00))//p194
#define GPIOA_CRH 	*((u32*)(PortA_address + 0x04))
#define GPIOA_IDR 	*((u32*)(PortA_address + 0x08))
#define GPIOA_ODR 	*((u32*)(PortA_address + 0x0C))
#define GPIOA_BSRR 	*((u32*)(PortA_address + 0x10))
#define GPIOA_BRR 	*((u32*)(PortA_address + 0x14))
#define GPIOA_LCKR 	*((u32*)(PortA_address + 0x18))

		/* PORT B registers */
#define GPIOB_CRL 	*((u32*)(PortB_address + 0x00))
#define GPIOB_CRH 	*((u32*)(PortB_address + 0x04))
#define GPIOB_IDR 	*((u32*)(PortB_address + 0x08))
#define GPIOB_ODR 	*((u32*)(PortB_address + 0x0C))
#define GPIOB_BSRR 	*((u32*)(PortB_address + 0x10))
#define GPIOB_BRR 	*((u32*)(PortB_address + 0x14))
#define GPIOB_LCKR 	*((u32*)(PortB_address + 0x18))

		/* PORT C registers */
#define GPIOC_CRL 	*((u32*)(PortC_address + 0x00))
#define GPIOC_CRH 	*((u32*)(PortC_address + 0x04))
#define GPIOC_IDR 	*((u32*)(PortC_address + 0x08))
#define GPIOC_ODR 	*((u32*)(PortC_address + 0x0C))
#define GPIOC_BSRR 	*((u32*)(PortC_address + 0x10))
#define GPIOC_BRR 	*((u32*)(PortC_address + 0x14))
#define GPIOC_LCKR 	*((u32*)(PortC_address + 0x18))

		/* PORT D registers */
#define GPIOD_CRL 	*((u32*)(PortD_address + 0x00))
#define GPIOD_CRH 	*((u32*)(PortD_address + 0x04))
#define GPIOD_IDR 	*((u32*)(PortD_address + 0x08))
#define GPIOD_ODR 	*((u32*)(PortD_address + 0x0C))
#define GPIOD_BSRR 	*((u32*)(PortD_address + 0x10))
#define GPIOD_BRR 	*((u32*)(PortD_address + 0x14))
#define GPIOD_LCKR 	*((u32*)(PortD_address + 0x18))

		/* PORT E registers */
#define GPIOE_CRL 	*((u32*)(PortE_address + 0x00))
#define GPIOE_CRH 	*((u32*)(PortE_address + 0x04))
#define GPIOE_IDR 	*((u32*)(PortE_address + 0x08))
#define GPIOE_ODR 	*((u32*)(PortE_address + 0x0C))
#define GPIOE_BSRR 	*((u32*)(PortE_address + 0x10))
#define GPIOE_BRR 	*((u32*)(PortE_address + 0x14))
#define GPIOE_LCKR 	*((u32*)(PortE_address + 0x18))

		/* PORT F registers */
#define GPIOF_CRL 	*((u32*)(PortF_address + 0x00))
#define GPIOF_CRH 	*((u32*)(PortF_address + 0x04))
#define GPIOF_IDR 	*((u32*)(PortF_address + 0x08))
#define GPIOF_ODR 	*((u32*)(PortF_address + 0x0C))
#define GPIOF_BSRR 	*((u32*)(PortF_address + 0x10))
#define GPIOF_BRR 	*((u32*)(PortF_address + 0x14))
#define GPIOF_LCKR 	*((u32*)(PortF_address + 0x18))

		/* PORT G registers */
#define GPIOG_CRL 	*((u32*)(PortG_address + 0x00))
#define GPIOG_CRH 	*((u32*)(PortG_address + 0x04))
#define GPIOG_IDR 	*((u32*)(PortG_address + 0x08))
#define GPIOG_ODR 	*((u32*)(PortG_address + 0x0C))
#define GPIOG_BSRR 	*((u32*)(PortG_address + 0x10))
#define GPIOG_BRR 	*((u32*)(PortG_address + 0x14))
#define GPIOG_LCKR 	*((u32*)(PortG_address + 0x18))






#endif /* DIO_PRIVATE_H_ */
