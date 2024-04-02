/*
*********************************************************************************************************
*                                                AR100 SYSTEM
*                                     AR100 Software System Develop Kits
*                                              interrupt  module
*
*                                    (c) Copyright 2012-2016, Sunny China
*                                             All Rights Reserved
*
* File    : intc.h
* By      : Sunny
* Version : v1.0
* Date    : 2012-4-27
* Descript: interrupt controller public header.
* Update  : date                auther      ver     notes
*           2012-4-27 10:52:56  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __IRQNUM_CONFIG_H__
#define __IRQNUM_CONFIG_H__

/*
 * ------------------------------------------------------------------------------
 * r_intc interrupt source
 * ------------------------------------------------------------------------------
 */
#define INTC_R_NMI_IRQ          0
#define INTC_R_TIMER0_IRQ       CLIC_SRC_RV_TIMER0
#define INTC_R_TIMER1_IRQ       CLIC_SRC_RV_TIMER1
#define INTC_R_ALM0_IRQ         24
#define INTC_R_GPIOL_NS_IRQ     25
#define INTC_R_GPIOL_S_IRQ      26
#define INTC_R_GPIOM_NS_IRQ     27
#define INTC_R_GPIOM_S_IRQ      28
#define INTC_R_USB_IRQ          41
#define INTC_R_IRRX_IRQ         34

#define CLIC_SRC_SPI(_n)			(16 + (_n))

#define CLIC_SRC_RV_TIMER0			CLIC_SRC_SPI(20-16)
#define CLIC_SRC_RV_TIMER1			CLIC_SRC_SPI(21-16)
#define CLIC_SRC_RV_TIMER2			CLIC_SRC_SPI(22-16)
#define CLIC_SRC_RV_TIMER3			CLIC_SRC_SPI(23-16)

/*
 * ------------------------------------------------------------------------------
 * gic interrupt source
 * ------------------------------------------------------------------------------
 */
#define GIC_USB0_EHCI_IRQ  101
#define GIC_USB0_OHCI_IRQ  102
#define GIC_USB1_EHCI_IRQ  103
#define GIC_USB1_OHCI_IRQ  104
#define GIC_R_EXTERNAL_NMI_IRQ  256
#define GIC_R_ALARM0_IRQ        264
#define GIC_R_GPIOL_S_IRQ  265
#define GIC_R_GPIOL_NS_IRQ 266
#define GIC_R_GPIOM_S_IRQ  267
#define GIC_R_GPIOM_NS_IRQ 268
#define GIC_R_IR_IRQ       274

/*
 *------------------------------------------------------------------------------
 * the max interrupt source number
 *------------------------------------------------------------------------------
 */
#define IRQ_SOUCE_MAX           44

#endif	/*__IRQNUM_CONFIG_H__*/
