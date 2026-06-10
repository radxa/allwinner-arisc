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

#define USE_DEFAULT_DRAM_PARA 1
#if USE_DEFAULT_DRAM_PARA
static const uint32_t dram_para_default[] = {
	/* Param By: Cubie A7A */
	/* [0]  dram_clk      */ 0x708,
	/* [1]  dram_type     */ 0x9,
	/* [2]  dram_zq       */ 0xE0E0E0E,
	/* [3]  dram_odt      */ 0xF0F0F0F,
	/* [4]  dram_para1    */ 0xEC030E0F,
	/* [5]                */ 0x0,
	/* [6]  dram_mr0      */ 0xA10A,
	/* [7]  dram_mr1      */ 0x20001001,
	/* [8]  dram_mr2      */ 0x0,
	/* [9]  dram_mr3      */ 0x80,
	/* [10] dram_tpr0     */ 0x88,
	/* [11] dram_tpr1     */ 0x6,
	/* [12] dram_tpr2     */ 0x0,
	/* [13] dram_tpr3     */ 0x0,
	/* [14] dram_tpr4     */ 0x0,
	/* [15] dram_tpr5     */ 0x1B,
	/* [16] dram_tpr6     */ 0x44,
	/* [17] dram_tpr7     */ 0x0,
	/* [18] dram_tpr8     */ 0x1D,
	/* [19] dram_tpr9     */ 0x0,
	/* [20] dram_tpr10    */ 0x86,
	/* [21] dram_tpr11    */ 0x0,
	/* [22] dram_tpr12    */ 0x4040,
	/* [23] dram_tpr13    */ 0x80000000,
	/* [24]               */ 0x11080503,
	/* [25]               */ 0x3800,
	/* [26]               */ 0x3514,
	/* [27]               */ 0x925F0000,
	/* [28]               */ 0x0,
	/* [29]               */ 0x0,
	/* [30]               */ 0x65,
	/* [31]               */ 0x0,
	/* [32] dram_training */ 0x35343435,
	/* [33]               */ 0x32322E30,
	/* [34]               */ 0x31342E35,
	/* [35]               */ 0x34373332,
	/* [36]               */ 0x2E382F2D,
	/* [37]               */ 0x37333732,
	/* [38]               */ 0x36343431,
	/* [39]               */ 0x30353334,
	/* [40]               */ 0x2C342A32,
	/* [41]               */ 0x2E212323,
	/* [42]               */ 0x21222C27,
	/* [43]               */ 0x2A252225,
	/* [44]               */ 0x242D231C,
	/* [45]               */ 0x23252524,
	/* [46]               */ 0x23292428,
	/* [47]               */ 0x1C2D2129,
	/* [48]               */ 0x22252726,
	/* [49]               */ 0x23222A22,
	/* [50]               */ 0x1E1C1F22,
	/* [51]               */ 0x1E1F201E,
};
#endif

uint32_t *dram_dts_parse(void)
{
	static uint32_t *dram_para;
	int32_t dram_para_node;
	void *fdt;
	u32 i;
	s32 fdt_err;

	fdt = (void *)(dtb_base);

	/* Debug: print DTB header info */
	LOG("DTB: base=0x%x, magic=0x%x, version=0x%x\n",
	    fdt, fdt_magic(fdt), fdt_version(fdt));
	LOG("DTB: totalsize=0x%x, off_struct=0x%x, off_strings=0x%x\n",
	    fdt_totalsize(fdt), fdt_off_dt_struct(fdt), fdt_off_dt_strings(fdt));
	LOG("DTB: size_struct=0x%x, size_strings=0x%x\n",
	    fdt_size_dt_struct(fdt), fdt_size_dt_strings(fdt));

	/* Check header validity */
	fdt_err = fdt_check_header(fdt);
	if (fdt_err != 0) {
		WRN("DTB header invalid: err=%d (%s), use default dram para\n", fdt_err, fdt_strerror(fdt_err));
	}

	/* parse dram para */
	dram_para_node = fdt_path_offset(fdt, "/dram");
	if (dram_para_node < 0) {
		WRN("fdt_path_offset(/dram) failed: %d (%s), use default dram para\n",
		    dram_para_node, fdt_strerror(dram_para_node));
		WRN("DTB header broken: off_struct=0x%x, size_struct=0x%x\n",
		    fdt_off_dt_struct(fdt), fdt_size_dt_struct(fdt));
	}
#if !USE_DEFAULT_DRAM_PARA
	char dram_para_prop[20];
	for (i = 0; i < (sizeof(dts_dram_para) / sizeof(u32)); i++) {
		if (i < 10) {
			sprintf(dram_para_prop, "dram_para0%d", i);
		} else {
			sprintf(dram_para_prop, "dram_para%d", i);
		}
		fdt_getprop_u32(fdt, dram_para_node, dram_para_prop, &dts_dram_para[i]);
		LOG("dram para[%d] 0x%x\n", i, dts_dram_para[i]);
	}
#else
	for (i = 0; i < (sizeof(dram_para_default) / sizeof(u32)); i++) {
		dts_dram_para[i] = dram_para_default[i];
		LOG("dram para[%d] 0x%x (default)\n", i, dts_dram_para[i]);
	}
#endif
	dram_para = &dts_dram_para[0];

	return dram_para;
}
