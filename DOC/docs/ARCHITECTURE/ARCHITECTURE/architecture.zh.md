# 架构

## 硬件架构

主控 + 感知 + 通信 + 交互 + 供电

![HARDWARE-ABSTRACTION](HARDWARE-ABSTRACTION.png)

![HARDWARE-ABSTRACTION-DETAIL](HARDWARE-ABSTRACTION-DETAIL.png)

!!! note
    以上为硬件抽象层的示意图，具体模块和功能以实际硬件为准。

## 软件架构

硬件层 + 平台抽象层 + 系统服务层 + 应用层

```shell
+----------------------------------------+
| 应用层 (Application Layer)              | <-- 应用代码
+----------------------------------------+
| 系统服务层 (System Services Layer)      | <-- 操作系统 + 中间件
+----------------------------------------+
| 平台抽象层 (Platform Abstraction Layer) | <-- 启动代码 + 硬件抽象层 (寄存器级) + 板级支持包 (开发板级)
+----------------------------------------+
| 硬件层 (Hardware Layer)                 | <-- 主控单元 + 外设
+----------------------------------------+
```

![SOFTWARE-ARCHITECTURE](SOFTWARE-ARCHITECTURE.png)