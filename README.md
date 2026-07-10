# APM32F103 USART 空闲中断与 DMA 收发示例 / APM32F103 USART Idle-Line and DMA Transfer Example

[中文](#中文说明) | [English](#english)

## 中文说明

本项目演示 APM32F103 使用 USART 空闲中断配合 DMA 进行不定长数据接收与回传。

### 工作原理

- USART1 使用 DMA 接收数据
- USART 空闲中断用于判断一帧数据接收结束
- 根据 DMA 剩余计数计算实际接收长度
- 接收到的数据通过 DMA 重新发送
- 默认接收缓冲区长度为 256 字节

### 默认配置

| 项目 | 配置 |
| --- | --- |
| 目标板 | APM32F103 MINI |
| 串口 | USART1 |
| 波特率 | 115200 |
| 数据位 | 8 bit |
| 停止位 | 1 bit |
| 校验位 | None |
| 硬件流控 | Disabled |
| DMA | DMA1 Channel 4/5 |
| 工程 | MDK-ARM / IAR EWARM |

仓库工程基于 APM32F10x SDK V1.8；随附说明记录的 IDE 版本为 MDK-ARM V5.36 和 EWARM V8.50.5.26295。

### 使用方法

1. 打开 MDK 或 IAR 工程。
2. 检查 USART1 引脚、时钟及调试板配置。
3. 编译并下载程序至 APM32F103 MINI。
4. 使用 USB 转串口工具连接 USART1，串口参数设置为 `115200 8N1`。
5. 发送长度不超过 256 字节的数据，程序将在空闲中断触发后回传已接收内容。

### 注意事项

- 若单帧数据超过缓冲区长度，请先调整 `MAX_LEN` 并重新检查内存占用。
- 修改波特率、DMA 通道或 USART 实例后，需同步调整外设和中断配置。
- 正式应用中建议增加缓冲区溢出、DMA 错误及连续帧处理机制。

### 技术支持

更多产品资料与技术支持请访问：[www.geehy.com](https://www.geehy.com/)。

---

## English

This project demonstrates variable-length USART reception and echo transmission on the APM32F103 using a USART idle-line interrupt together with DMA.

### How it works

- USART1 receives data through DMA
- The USART idle-line interrupt detects the end of a frame
- The received length is calculated from the remaining DMA count
- Received data is transmitted back through DMA
- The default receive-buffer size is 256 bytes

### Default configuration

| Item | Configuration |
| --- | --- |
| Target board | APM32F103 MINI |
| USART | USART1 |
| Baud rate | 115200 |
| Data bits | 8 bit |
| Stop bits | 1 bit |
| Parity | None |
| Hardware flow control | Disabled |
| DMA | DMA1 Channel 4/5 |
| Projects | MDK-ARM / IAR EWARM |

The repository is based on APM32F10x SDK V1.8. The bundled documentation lists MDK-ARM V5.36 and EWARM V8.50.5.26295.

### Usage

1. Open the MDK or IAR project.
2. Verify the USART1 pins, clocks, and target-board configuration.
3. Build and program the example to an APM32F103 MINI.
4. Connect a USB-to-serial adapter to USART1 and use `115200 8N1`.
5. Send no more than 256 bytes per frame. The example echoes the received data after the idle-line interrupt occurs.

### Notes

- If a frame may exceed the buffer size, update `MAX_LEN` and recheck memory usage.
- When changing the baud rate, DMA channels, or USART instance, update the peripheral and interrupt configuration together.
- Production applications should add buffer-overflow, DMA-error, and back-to-back-frame handling.

### Support

For product information and technical support, visit [www.geehy.com](https://www.geehy.com/).
