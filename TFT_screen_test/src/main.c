/*
 * ************************************************
 * 
 *              STM32 blink gcc demo
 * 
 *  CPU: STM32F103C8
 *  PIN: PA1
 * 
 * ************************************************
*/

#define TFT_RST_LOW()  GPIOA->BRR  = GPIO_PIN_3
#define TFT_RST_HIGH() GPIOA->BSRR = GPIO_PIN_3
#define TFT_DC_CMD()   GPIOA->BRR  = GPIO_PIN_2  // AO引脚
#define TFT_DC_DATA()  GPIOA->BSRR = GPIO_PIN_2
#define TFT_CS_LOW()   GPIOA->BRR  = GPIO_PIN_4
#define TFT_CS_HIGH()  GPIOA->BSRR = GPIO_PIN_4

// 简单的SPI发送字节函数 (假设你已经配置好了SPI1)
void SPI_WriteByte(uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE)); // 等待发送缓冲区空
    SPI1->DR = data;                  // 发送数据
    while (SPI1->SR & SPI_SR_BSY);    // 等待传输完成
}

// 写指令
void LCD_WriteCmd(uint8_t cmd) {
    TFT_DC_CMD();
    TFT_CS_LOW();
    SPI_WriteByte(cmd);
    TFT_CS_HIGH();
}

// 写数据
void LCD_WriteData(uint8_t data) {
    TFT_DC_DATA();
    TFT_CS_LOW();
    SPI_WriteByte(data);
    TFT_CS_HIGH();
}

// 屏幕初始化（ST7735最简指令序列）
void LCD_Init(void) {
    TFT_RST_LOW();
    // 延迟约100ms...
    TFT_RST_HIGH();
    
    LCD_WriteCmd(0x01); // 软件复位
    LCD_WriteCmd(0x11); // 退出睡眠
    // ... 这里省略部分配置指令 ...
    LCD_WriteCmd(0x29); // 开启显示
}

int main(void) {
    // 1. 初始化系统时钟
    // 2. 初始化 GPIO (PA2, PA3, PA4) 和 SPI1 (PA5, PA7)
    // 3. 执行屏幕初始化
    LCD_Init();
    
    while(1) {
        // 刷屏或显示逻辑
    }
}
