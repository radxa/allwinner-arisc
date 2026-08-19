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

#ifndef CFG_HOT_REBOOT
uint32_t dts_dram_para[160];
#else
uint32_t dram_head_magic __attribute__((section(".dram_head_magic"), used));
uint32_t dts_dram_para[160] __attribute__((section(".dts_dram_para"), used));
uint32_t dram_tail_magic __attribute__((section(".dram_tail_magic"), used));
#endif

#define DRAM_PARA_WORDS (sizeof(dram_para_t) / sizeof(uint32_t))

uint32_t *dram_dts_parse(void)
{
	static uint32_t *dram_para;
	int32_t dram_para_node;
	char dram_para_prop[20];
	void *fdt;
	u32 i;
	s32 ret;

	fdt = (void *)(dtb_base);

	/* Debug: print DTB header info */
	LOG("DTB: base=0x%x, magic=0x%x, version=0x%x\n",
	    fdt, fdt_magic(fdt), fdt_version(fdt));
	LOG("DTB: totalsize=0x%x, off_struct=0x%x, off_strings=0x%x\n",
	    fdt_totalsize(fdt), fdt_off_dt_struct(fdt), fdt_off_dt_strings(fdt));
	LOG("DTB: size_struct=0x%x, size_strings=0x%x\n",
	    fdt_size_dt_struct(fdt), fdt_size_dt_strings(fdt));

	/* Check header validity */
	ret = fdt_check_header(fdt);
	if (ret != 0) {
		ERR("DTB header invalid: err=%d (%s)\n", ret, fdt_strerror(ret));
		return NULL;
	}

	/* parse dram para */
	dram_para_node = fdt_path_offset(fdt, "/dram");
	if (dram_para_node < 0) {
		ERR("fdt_path_offset(/dram) failed: %d (%s)\n",
		    dram_para_node, fdt_strerror(dram_para_node));
		return NULL;
	}

	memset(dts_dram_para, 0, sizeof(dts_dram_para));
	for (i = 0; i < DRAM_PARA_WORDS; i++) {
		if (i < 10) {
			sprintf(dram_para_prop, "dram_para0%d", i);
		} else {
			sprintf(dram_para_prop, "dram_para%d", i);
		}
		ret = fdt_getprop_u32(fdt, dram_para_node, dram_para_prop,
				      &dts_dram_para[i]);
		if (ret < 0) {
			ERR("failed to read %s: %d (%s)\n", dram_para_prop,
			    ret, fdt_strerror(ret));
			return NULL;
		}
	}

	LOG("dram para: clk=0x%x type=0x%x para1=0x%x para2=0x%x tpr13=0x%x\n",
	    dts_dram_para[0], dts_dram_para[1], dts_dram_para[6],
	    dts_dram_para[7], dts_dram_para[30]);
	dram_para = &dts_dram_para[0];

	return dram_para;
}
