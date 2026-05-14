/*
*********************************************************************************************************
*                                                AR100 SYSTEM
*                                     AR100 Software System Develop Kits
*                                              interrupt  module
*
*                                    (c) Copyright 2012-2016, Sunny China
*                                             All Rights Reserved
*
* File    : hdmitx.c
* By      : Sunny
* Version : v1.0
* Date    : 2012-5-3
* Descript: hdmi assist tx cec.
* Update  : date                auther      ver     notes
*           2025-11-5 15:44:00  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "cec/cec.h"

struct hdmi_tx_s {
	bool init_done;
	bool fakeoff;
	bool cec_enable;
	bool cec_return;
	u32 cec_index;
	u32 cec_irq;
	CECMsgQueue cec_queue;
};

extern uint32_t volatile wakeup_source;
struct hdmi_tx_s hdmi;

#define INTC_R_HDMI_ASSIST_TX_IRQ  42
#if defined(CFG_SUN55IW7P1)
#define SUPPORT_RTC_FLAG	(1)
#define RTC_BASE 		(0x07200000)
#define RTC_GP_DATA(x)	(RTC_BASE + 0x100 + (x * 4))
#define RTC_GP_INDEX	(9)
#else
#define SUPPORT_RTC_FLAG	(0)
#endif


void _init_queue(CECMsgQueue *q)
{
	q->pHead = q->pPush = q->pPop = q->queue;
	memset(q->queue, 0, sizeof(q->queue));
}

bool _queue_empty(CECMsgQueue *q)
{
	return q->pPush == q->pPop;
}

bool _queue_full(CECMsgQueue *q)
{
	return (q->pPush + 1 == q->pPop) ||
			(q->pPush + 1 == q->queue + QUEUE_SIZE && q->pPop == q->queue);
}

bool _enqueue(CECMsgQueue *q, TCECMessage msg)
{
	if (_queue_full(q)) {
		WRN("Queue is full!\n");
		return FALSE;
	}

	*(q->pPush) = msg;
	q->pPush++;

	if (q->pPush == q->queue + QUEUE_SIZE) {
		q->pPush = q->queue;
	}

	return TRUE;
}

bool _dequeue(CECMsgQueue *q, TCECMessage *msg)
{
	if (_queue_empty(q)) {
		//WRN("Queue is empty!\n");
		return FALSE;
	}
	*msg = *(q->pPop);
	q->pPop++;

	if (q->pPop == q->queue + QUEUE_SIZE) {
		q->pPop = q->queue;
	}
	return TRUE;
}

int _hdmitx_cec_msg_print(u8 *buff, u8 len)
{
	int i = 0;

	LOG("tx cec msg:");
	for (i = 0; i < len; i++)
		LOG(" 0x%x", buff[i]);
	LOG("\n");
	return 0;
}

int _hdmitx_cmd_cec_on(void)
{
	_init_queue(&hdmi.cec_queue);
	hdmi.cec_enable = 1;
	printk("hdmi tx cec cmd set on\n");
	return 0;
}

int _hdmitx_cmd_cec_off(void)
{
	hdmi.cec_enable = 0;
	printk("hdmi tx cec cmd set off\n");
	return 0;
}

int _hdmitx_cmd_cec_return(void)
{
	bool ret = FALSE;
	TCECMessage cec_msg;
	int i = 0;

	while (1) {
		memset(&cec_msg, 0x0, sizeof(TCECMessage));
		ret = _dequeue(&hdmi.cec_queue, &cec_msg);
		if (ret == FALSE)
			return 0;

		if (cec_msg.ucOperandNum > 0) {
			LOG("cec[%d]:", cec_msg.index);
			for (i = 0; i < cec_msg.ucOperandNum; i++)
				LOG(" 0x%02x", cec_msg.Content.Buffer[i]);
			LOG("\n");
			/* TODO, maybe add rpm_send_to_arm */
		}
	}
	return 0;
}

/*
 * CEC wakeup opcode table:
 *   op2 == 0xFF: single-opcode mode, trigger wakeup on op1 match
 *   op2 != 0xFF: dual-opcode mode, require op1 followed by op2
 */
#define SINGLE_OPCODE_FLAG 0xFF

static const u8 wakeup_opcodes[][2] = {
	{0x87, 0x82},               /* dual: 0x87 -> 0x82 */
	{0x86, SINGLE_OPCODE_FLAG}, /* single: 0x86 (Set Stream Path) */
};

static void trigger_cec_wakeup(u8 opcode)
{
#if SUPPORT_RTC_FLAG
	writel((0xFF400000 | opcode), RTC_GP_DATA(RTC_GP_INDEX));
#endif
	wakeup_source = OneTouchPlay_Wakeup;
	INF("hdmi tx cec wakeup, opcode: 0x%x\n", opcode);
}

int _hdmitx_cec_wakeup_check(u8 *buf)
{
	static u8 matched, match_idx;
	u8 opcode = buf[1], i = 0;

	/* handle second opcode of dual-opcode sequence */
	if (matched) {
		if (wakeup_opcodes[match_idx][1] == opcode) {
			trigger_cec_wakeup(opcode);
			return 1;
		}
		matched = 0;
	}

	/* scan opcode table */
	for (i = 0; i < ARRAY_SIZE(wakeup_opcodes); i++) {
		if (wakeup_opcodes[i][0] != opcode)
			continue;

		if (wakeup_opcodes[i][1] == SINGLE_OPCODE_FLAG) {
			trigger_cec_wakeup(opcode);
			return 1;
		}

		/* first opcode matched, wait for second */
		matched = 1;
		match_idx = i;
		return 0;
	}

	matched = 0;
	return 0;
}

int _hdmitx_cec_message_handler(u8 *buf, u8 len)
{
	TCECMessage cec_msg;

	memcpy(cec_msg.Content.Buffer, buf, len);
	cec_msg.ucOperandNum = len;
	cec_msg.index = hdmi.cec_index;

	hdmi.cec_index++;
	if (hdmi.cec_index > 100)
		hdmi.cec_index = 0;

	_enqueue(&hdmi.cec_queue, cec_msg);
	return 0;
}

void _hdmitx_loop(void)
{
	u8 buf[16], ret;

	if (hdmi.fakeoff) {
		hwcec_init();
	} else {
		if (!hdmi.cec_enable) {
			goto exit;
		}
	}

	if (hwcec_is_receive() == 0)
		goto exit;

	ret = hwcec_msg_receive(buf, ARRAY_SIZE(buf));
	if (ret <= 0)
		goto exit;

	_hdmitx_cec_msg_print(buf, ret);

	if (_hdmitx_cec_wakeup_check(buf))
		goto exit;

	_hdmitx_cec_message_handler(buf, ret);
exit:
	return;
}

void hdmitx_standby_loop(void)
{
	if (hdmi.init_done)
		_hdmitx_loop();
}

void hdmitx_main_loop(void)
{
	if (hdmi.init_done)
		_hdmitx_loop();
}

int hdmitx_cmd_handler(uint8_t length, uint8_t *pdata)
{
	uint8_t cmd = pdata[0];

	switch (cmd) {
	case HOST_TO_MCU_SET_CEC_ON:
		_hdmitx_cmd_cec_on();
		break;
	case HOST_TO_MCU_SET_CEC_OFF:
		_hdmitx_cmd_cec_off();
		break;
	case HOST_TO_MCU_GET_CEC_BUFF:
		_hdmitx_cmd_cec_return();
		break;
	default:
		WRN("hdmi tx unsupport cmd: %d\n", cmd);
		break;
	}
	return 0;
}

void hdmitx_update_type(unsigned char type)
{
	if (type == FAKE_POWER_OFF_REQ)
		hdmi.fakeoff = 1;
	else
		hdmi.fakeoff = 0;
}

int hdmitx_init(void)
{
	memset(&hdmi, 0x0, sizeof(struct hdmi_tx_s));
	hdmi.init_done = 1;
	return 0;
}
