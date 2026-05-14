/*
*********************************************************************************************************
*                                                AR100 SYSTEM
*                                     AR100 Software System Develop Kits
*                                               standby module
*
*                                    (c) Copyright 2012-2016, superm China
*                                             All Rights Reserved
*
* File    : drivers/smc/smc.c
* By      : wuyan
* Version : v1.0
* Date    : 2024-10-28
* Descript: support smc driver.
* Update  : date                 auther       ver     notes
*           2024-10-28 17:04:28  wuyan        1.0     Create this file.
*********************************************************************************************************
*/
#include <include.h>
#include <platform_regs.h>

/* platform smc registers save */
static uint32_t smc_registers[(SUNXI_SMC_REG_MAX / 4) + 1] = {0};
static bool smc_enable;

static bool sunxi_smc_get_enable(void)
{
	/* Check if SMC is enabled (not bypassed) */
	return !(readl(SMC_ACTION_REG) & SMC_FUNCTION_BYPASS);
}

static void sunxi_smc_cfg_save(void)
{
	uint32_t i;

	smc_enable = sunxi_smc_get_enable();
	if (!smc_enable)
		return;

	/* Save all SMC registers */
	for (i = 0; i < ARRAY_SIZE(smc_registers); i++) {
		smc_registers[i] = readl(SUNXI_SMC_PBASE + (i * 4));
	}
}

static void sunxi_smc_cfg_restore(void)
{
	uint32_t i;

	if (!smc_enable)
		return;

	/* Restore all SMC registers */
	for (i = 0; i < ARRAY_SIZE(smc_registers); i++) {
		writel(smc_registers[i], SUNXI_SMC_PBASE + (i * 4));
	}
}

void __attribute__((weak)) smc_standby_init(void)
{
	sunxi_smc_cfg_save();
}

void __attribute__((weak)) smc_standby_exit(void)
{
	sunxi_smc_cfg_restore();
}
