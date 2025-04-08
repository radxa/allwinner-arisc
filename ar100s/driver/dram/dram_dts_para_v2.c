/*
 *	Allwinner Technology, All Right Reserved. 2019-2024 Copyright (c)
 *
 *  File: 	plat_init.c
 *
 *	Description:  This file implements DFS functions for AW1890 DRAM controller
 *
 *  History:
 *  		2023/11/6		WJW		V0.10		Initial version;
*/
#include <libfdt.h>
#include "include.h"

extern u32 dtb_base;
uint32_t dts_dram_para[96];

uint32_t *dram_dts_parse(void)
{
	static uint32_t *dram_para;
	int32_t dram_para_node;
	char dram_para_prop[20];
	void *fdt;
	u32 i;

	fdt = (void *)(dtb_base);

	/* parse dram para */
	dram_para_node = fdt_path_offset(fdt, "/dram");
	if (dram_para_node < 0) {
		WRN("no dram-para: %x fdt:%x\n", dram_para_node, fdt);
		return NULL;
	}

	for (i = 0; i < (sizeof(dts_dram_para) / sizeof(u32)); i++) {
		if (i < 10) {
			sprintf(dram_para_prop, "dram_para0%d", i);
		} else {
			sprintf(dram_para_prop, "dram_para%d", i);
		}
		fdt_getprop_u32(fdt, dram_para_node, dram_para_prop, &dts_dram_para[i]);
		printk("dram para[%d] 0x%x\n", i, dts_dram_para[i]);
	}

	dram_para = &dts_dram_para[0];

	return dram_para;
}
