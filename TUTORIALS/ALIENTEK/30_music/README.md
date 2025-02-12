/**
 ***************************************************************************************************
 * 实验简介
 * 实验名称：音乐播放器 实验
 * 实验平台：正点原子 ESP32-S3 开发板
 * 实验目的：学习SDIO驱动SD卡的使用
 * 
 ***************************************************************************************************
 * 硬件资源及引脚分配
 * 1 LED
     LED - IO1
 * 2 独立按键
     KEY0(XL9555) - IO1_7
     KEY1(XL9555) - IO1_6
     KEY2(XL9555) - IO1_5
     KEY3(XL9555) - IO1_4
 * 3 XL9555
     IIC_SCL - IO42
     IIC_SDA - IO41
 * 4 正点原子1.3/2.4寸SPILCD模块
 * 5 SD卡,通过SPI2驱动 
 * 6 ES8388音频CODEC芯片
 * 
 ***************************************************************************************************
 * 实验现象
 * 1 本实验开机后，先初始化各外设，然后检测字库是否存在，如果检测无问题，则开始循环播放SD卡MUSIC文
 *   件夹里面的歌曲（必须在SD卡根目录建立一个MUSIC文件夹，并存放歌曲（仅支持wav格式）在里面），在
 *   TFTLCD上显示歌曲名字、播放时间、歌曲总时间、歌曲总数目、当前歌曲的编号等信息。KEY0用于选择下
 *   一曲，KEY2用于选择上一曲，KEY3用来控制暂停/继续播放
 * 2 LED闪烁，指示程序正在运行
 * 
 ***************************************************************************************************
 * 注意事项
 * USART1的通讯波特率为115200
 * 请使用XCOM串口调试助手，其他串口软件可能控制DTR、RST导致MCU复位、程序不运行
 * 需将SD卡正确插入板载的SD卡槽，才能正常运行本实验例程
 * 
 ***********************************************************************************************************
 * 公司名称：广州市星翼电子科技有限公司（正点原子）
 * 电话号码：020-38271790
 * 传真号码：020-36773971
 * 公司网址：www.alientek.com
 * 购买地址：zhengdianyuanzi.tmall.com
 * 技术论坛：http://www.openedv.com/forum.php
 * 最新资料：www.openedv.com/docs/index.html
 *
 * 在线视频：www.yuanzige.com
 * B 站视频：space.bilibili.com/394620890
 * 公 众 号：mp.weixin.qq.com/s/y--mG3qQT8gop0VRuER9bw
 * 抖    音：douyin.com/user/MS4wLjABAAAAi5E95JUBpqsW5kgMEaagtIITIl15hAJvMO8vQMV1tT6PEsw-V5HbkNLlLMkFf1Bd
 ***********************************************************************************************************
 */