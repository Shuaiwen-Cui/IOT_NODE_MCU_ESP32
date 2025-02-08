﻿/**
 ****************************************************************************************************
 * @file        image.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-12-01
 * @brief       图片库 代码(提供image_update_image和images_init用于图片库更新和初始化)
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

#include "image.h"
#include "lcd.h"
#include "ff.h"
#include <string.h>

/* 图片库区域占用的总扇区数大小 */
#define IMAGESECSIZE         90

/* 图片库存放起始地址 */
#define IMAGEINFOADDR        0

/* 每次操作限制在 4K 之内 */
#define SECTOR_SIZE          0X1000

/* 用来保存图片库基本信息，地址，大小等 */
_image_info g_ftinfo;

static const char *TAG = "storage_partition";
const esp_partition_t *storage_partition;

/* 图片库存放在磁盘中的路径 */
char *const IMAGE_GBK_PATH[10] =
{
    "/SYSTEM/LVGLBIN/lv_camera.BIN",
    "/SYSTEM/LVGLBIN/lv_file.BIN",
    "/SYSTEM/LVGLBIN/lv_video.BIN",
    "/SYSTEM/LVGLBIN/lv_setting.BIN",
    "/SYSTEM/LVGLBIN/lv_weather.BIN",
    "/SYSTEM/LVGLBIN/lv_measure.BIN",
    "/SYSTEM/LVGLBIN/lv_photo.BIN",
    "/SYSTEM/LVGLBIN/lv_music.BIN",
    "/SYSTEM/LVGLBIN/lv_calendar.BIN",
    "/SYSTEM/LVGLBIN/lv_background.BIN",
};

/* 更新时的提示信息 */
char *const IMAGE_UPDATE_REMIND_TBL[10] =
{
    "Updating lv_camera.BIN",
    "Updating lv_file.BIN",
    "Updating lv_video.BIN",
    "Updating lv_setting.BIN",
    "Updating lv_weather.BIN",
    "Updating lv_measure.BIN",
    "Updating lv_photo.BIN",
    "Updating lv_music.BIN",
    "Updating lv_calendar.BIN",
    "Updating lv_background.BIN",
};

#define IMAGE_GBK_NUM           (int)(sizeof(IMAGE_GBK_PATH) / sizeof(IMAGE_GBK_PATH[0]))
#define IMAGE_UPDATE_REMIND_NUM (int)(sizeof(IMAGE_UPDATE_REMIND_TBL) / sizeof(IMAGE_UPDATE_REMIND_TBL[0]))

/**
 * @brief       分区表读取数据
 * @param       buffer    : 读取数据的存储区
 * @param       offset    : 读取数据的起始地址
 * @param       length    : 读取大小
 * @retval      ESP_OK:表示成功;其他:表示失败
 */
esp_err_t images_partition_read(void *buffer, uint32_t offset, uint32_t length)
{
    esp_err_t err;

    if (buffer == NULL)
    {
        ESP_LOGE(TAG, "ESP_ERR_INVALID_ARG");
        return ESP_ERR_INVALID_ARG;
    }

    err = esp_partition_read(storage_partition, offset, buffer, length);
    
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Flash read failed.");
        return err;
    }
    
    return err;
}

/**
 * @brief       分区表写入数据
 * @param       buffer    : 写入数据的存储区
 * @param       offset    : 写入数据的起始地址
 * @param       length    : 写入大小
 * @retval      ESP_OK:表示成功;其他:表示失败
 */
esp_err_t images_partition_write(void *buffer, uint32_t offset, uint32_t length)
{
    esp_err_t err;

    if (buffer == NULL)
    {
        ESP_LOGE(TAG, "ESP_ERR_INVALID_ARG");
        return ESP_ERR_INVALID_ARG;
    }

    err = esp_partition_write(storage_partition, offset, buffer, length);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Flash write failed.");
        return err;
    }

    return err;
}

/**
 * @brief       擦除某个扇区
 * @param       offset    : 擦除起始地址
 * @retval      ESP_OK:表示成功;其他:表示失败
 */
esp_err_t images_partition_erase_sector(uint32_t offset)
{
    esp_err_t err;

    err = esp_partition_erase_range(storage_partition, offset, SECTOR_SIZE);
    
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Flash erase failed.");
        return err;
    }

    return err;
}

/**
 * @brief       显示当前图片更新进度
 * @param       x, y    : 坐标
 * @param       size    : 图片大小
 * @param       totsize : 整个文件大小
 * @param       pos     : 当前文件指针位置
 * @param       color   : 图片颜色
 * @retval      无
 */
static void images_progress_show(uint16_t x, uint16_t y, uint8_t size, uint32_t totsize, uint32_t pos, uint16_t color)
{
    float prog;
    uint8_t t = 0XFF;
    prog = (float)pos / totsize;
    prog *= 100;

    if (t != prog)
    {
        lcd_show_string(x + 3 * size / 2, y, 240, 320, size, "%", color);
        t = prog;

        if (t > 100) t = 100;

        lcd_show_num(x, y, t, 3, size, color);  /* 显示数值 */
    }
}

/**
 * @brief       更新某一个图片库
 * @param       x, y    : 提示信息的显示地址
 * @param       size    : 提示信息图片大小
 * @param       fpath   : 图片路径
 * @param       fx      : 更新的内容
 *   @arg                 0, atk01;
 *   @Arg                 1, atk02;
 *   @arg                 2, atk03;
 *   @arg                 3, atk04;
 *   @arg                 4, atk05;
 * @param       color   : 图片颜色
 * @retval      0, 成功; 其他, 错误代码;
 */
static uint8_t images_update_imagex(uint16_t x, uint16_t y, uint8_t size, uint8_t *fpath, uint8_t fx, uint16_t color)
{
    uint32_t flashaddr = 0;
    FIL *fftemp;
    uint8_t *tempbuf;
    uint8_t res;
    uint16_t bread;
    uint32_t offx = 0;
    uint8_t rval = 0;

    fftemp = (FIL *)malloc(sizeof(FIL));  /* 分配内存 */

    if (fftemp == NULL)rval = 1;

    tempbuf = malloc(4096);               /* 分配4096个字节空间 */

    if (tempbuf == NULL)rval = 1;

    res = f_open(fftemp, (const TCHAR *)fpath, FA_READ);

    if (res) rval = 2;   /* 打开文件失败 */

    if (rval == 0)
    {
        switch (fx)
        {
            case 0:
                g_ftinfo.lvgl_camera_addr = IMAGEINFOADDR + sizeof(g_ftinfo);
                g_ftinfo.lvgl_camera_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_camera_addr;
                break;
            case 1:
                g_ftinfo.lvgl_file_addr = g_ftinfo.lvgl_camera_addr + g_ftinfo.lvgl_camera_size;
                g_ftinfo.lvgl_file_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_file_addr;
                break;
            case 2:
                g_ftinfo.lvgl_video_addr = g_ftinfo.lvgl_file_addr + g_ftinfo.lvgl_file_size;
                g_ftinfo.lvgl_video_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_video_addr;
                break;
            case 3:
                g_ftinfo.lvgl_setting_addr = g_ftinfo.lvgl_video_addr + g_ftinfo.lvgl_video_size;
                g_ftinfo.lvgl_setting_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_setting_addr;
                break;
            case 4:
                g_ftinfo.lvgl_weather_addr = g_ftinfo.lvgl_setting_addr + g_ftinfo.lvgl_setting_size;
                g_ftinfo.lvgl_weather_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_weather_addr;
                break;
            case 5:
                g_ftinfo.lvgl_measure_addr = g_ftinfo.lvgl_weather_addr + g_ftinfo.lvgl_weather_size;
                g_ftinfo.lvgl_measure_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_measure_addr;
                break;
            case 6:
                g_ftinfo.lvgl_photo_addr = g_ftinfo.lvgl_measure_addr + g_ftinfo.lvgl_measure_size;
                g_ftinfo.lvgl_photo_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_photo_addr;
                break;
            case 7:
                g_ftinfo.lvgl_music_addr = g_ftinfo.lvgl_photo_addr + g_ftinfo.lvgl_photo_size;
                g_ftinfo.lvgl_music_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_music_addr;
                break;
            case 8:
                g_ftinfo.lvgl_calendar_addr = g_ftinfo.lvgl_music_addr + g_ftinfo.lvgl_music_size;
                g_ftinfo.lvgl_calendar_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_calendar_addr;
                break;
            case 9:
                g_ftinfo.lvgl_background_addr = g_ftinfo.lvgl_calendar_addr + g_ftinfo.lvgl_calendar_size;
                g_ftinfo.lvgl_background_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_background_addr;
                break;
        }

        while (res == FR_OK)            /* 死循环执行 */
        {
            res = f_read(fftemp, tempbuf, 4096, (UINT *)&bread);                /* 读取数据 */

            if (res != FR_OK) break;    /* 执行错误 */

            images_partition_write(tempbuf, offx + flashaddr, bread);          /* 从0开始写入bread个数据 */
            offx += bread;
            images_progress_show(x, y, size, fftemp->obj.objsize, offx, color); /* 进度显示 */

            if (bread != 4096) break;   /* 读完了 */
        }

        f_close(fftemp);
    }

    free(fftemp);     /* 释放内存 */
    free(tempbuf);    /* 释放内存 */
    return res;
}

/**
 * @brief       更新图片文件
 *   @note      所有图片库一起更新(UNIGBK,GBK12,GBK16,GBK24,GBK32)
 * @param       x, y    : 提示信息的显示地址
 * @param       size    : 提示信息图片大小
 * @param       src     : 图片库来源磁盘
 *   @arg                 "0:", SD卡;
 *   @arg                 "1:", FLASH盘
 * @param       color   : 图片颜色
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t images_update_image(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color)
{
    uint8_t *pname;
    uint32_t *buf;
    uint8_t res = 0;
    uint16_t i, j;
    FIL *fftemp;
    uint8_t rval = 0;
    res = 0XFF;
    g_ftinfo.imageok = 0XFF;

    pname = malloc(100);                    /* 申请100字节内存 */
    buf = malloc(4096);                     /* 申请4K字节内存 */
    fftemp = (FIL *)malloc(sizeof(FIL));    /* 分配内存 */

    if (buf == NULL || pname == NULL || fftemp == NULL)
    {
        free(fftemp);
        free(pname);
        free(buf);
        return 5;   /* 内存申请失败 */
    }

    for (i = 0; i < IMAGE_GBK_NUM; i++)     /* 先查找文件atk01,atk02,atk03,money是否正常 */
    {
        strcpy((char *)pname, (char *)src);                  /* copy src内容到pname */
        strcat((char *)pname, (char *)IMAGE_GBK_PATH[i]);    /* 追加具体文件路径 */
        res = f_open(fftemp, (const TCHAR *)pname, FA_READ); /* 尝试打开 */

        if (res)
        {
            rval |= 1 << 7;     /* 标记打开文件失败 */
            break;              /* 出错了,直接退出 */
        }
    }

    free(fftemp);               /* 释放内存 */

    if (rval == 0)  /* 图片库文件都存在 */
    {
        lcd_show_string(x, y, 240, 320, size, "Erasing sectors... ", color);    /* 提示正在擦除扇区 */

        for (i = 0; i < IMAGESECSIZE; i++)          /* 先擦除图片库区域,提高写入速度 */
        {
            images_progress_show(x + 20 * size / 2, y, size, IMAGESECSIZE, i, color);           /* 进度显示 */
            images_partition_read((uint8_t *)buf, ((IMAGEINFOADDR / 4096) + i) * 4096, 4096);   /* 读出整个扇区的内容 */

            for (j = 0; j < 1024; j++)              /* 校验数据 */
            {
                if (buf[j] != 0XFFFFFFFF) break;    /* 需要擦除 */
            }

            if (j != 1024)
            {
                images_partition_erase_sector(((IMAGEINFOADDR / 4096) + i) * 4096);     /* 需要擦除的扇区 */
            }
        }

        for (i = 0; i < IMAGE_UPDATE_REMIND_NUM; i++) /* 依次更新UNIGBK,GBK12,GBK16,GBK24 */
        {
            lcd_show_string(x, y, 240, 320, size, IMAGE_UPDATE_REMIND_TBL[i], color);
            strcpy((char *)pname, (char *)src);                 /* copy src内容到pname */
            strcat((char *)pname, (char *)IMAGE_GBK_PATH[i]);   /* 追加具体文件路径 */
            res = images_update_imagex(x + 20 * size / 2, y, size, pname, i, color);    /* 更新字库 */

            if (res)
            {
                free(buf);
                free(pname);
                return 1 + i;
            }
        }

        /* 全部更新好了 */
        g_ftinfo.imageok = 0xBB;
        images_partition_write((uint8_t *)&g_ftinfo, IMAGEINFOADDR, sizeof(g_ftinfo));    /* 保存字库信息 */
    }

    free(pname);    /* 释放内存 */
    free(buf);      /* 释放内存 */

    return rval;  
}

/**
 * @brief       初始化图片
 * @param       无
 * @retval      0, 图片库完好; 其他, 图片库丢失;
 */
uint8_t images_init(void)
{
    uint8_t t = 0;

    storage_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "storage");
    
    if (storage_partition == NULL)
    {
        ESP_LOGE(TAG, "Flash partition not found.");
        return 1;
    }

    while (t < 10)  /* 连续读取10次,都是错误,说明确实是有问题,得更新图片库了 */
    {
        t++;
        images_partition_read((uint8_t *)&g_ftinfo, IMAGEINFOADDR, sizeof(g_ftinfo)); /* 连续读取10次,都是错误,说明确实是有问题,得更新图片库了 */

        if (g_ftinfo.imageok == 0xBB)
        {
            break;
        }
        
        vTaskDelay(20);
    }

    if (g_ftinfo.imageok != 0xBB)
    {
        return 1;
    }
    
    return 0;
}
