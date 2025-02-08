/**
 ****************************************************************************************************
 * @file        emission.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       RMT红外解码驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 ESP32-S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __REMOTE_H__
#define __REMOTE_H__

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_idf_version.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"
#include "renc.h"
#include "key.h"
#include "xl9555.h"


/* 引脚定义 */
#define RMT_RX_PIN                  GPIO_NUM_2  /* 连接RX_PIN的GPIO端口 */
#define RMT_TX_PIN                  GPIO_NUM_8  /* 连接TX_PIN的GPIO端口 */
#define RMT_RESOLUTION_HZ           1000000     /* 1MHz, 1 tick = 1us */
#define RMT_NEC_DECODE_MARGIN       200         /* 判断NEC时序时长的容差值，小于（值+此值），大于（值-此值）为正确 */

/* NEC 协议时序时间，协议头9.5ms 4.5ms 逻辑0两个电平时长，逻辑1两个电平时长，重复码两个电平时长 */
#define NEC_LEADING_CODE_DURATION_0 9000
#define NEC_LEADING_CODE_DURATION_1 4500
#define NEC_PAYLOAD_ZERO_DURATION_0 560
#define NEC_PAYLOAD_ZERO_DURATION_1 560
#define NEC_PAYLOAD_ONE_DURATION_0  560
#define NEC_PAYLOAD_ONE_DURATION_1  1690
#define NEC_REPEAT_CODE_DURATION_0  9000
#define NEC_REPEAT_CODE_DURATION_1  2250

/* 函数声明 */
void emission_init(void);
void example_parse_nec_frame(rmt_symbol_word_t *rmt_nec_symbols, size_t symbol_num);
bool RMT_Rx_Done_Callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data);

#endif