/**
 * ATmega328PB 嵌入式面试复习资料
 *
 * 面试常考内容：
 * 1. 芯片基本特性与引脚定义
 * 2. 寄存器操作（GPIO / Timer / UART / ADC）
 * 3. 中断机制
 * 4. PWM / SPI / I2C 等通信协议
 * 5. 常见代码实现
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

/* ================================================================
 * ATmega328PB 基本信息
 * ================================================================
 *
 * ARM: 8-bit AVR RISC processor
 * 频率: 最高 20MHz（通常 16MHz 外晶）
 * 闪存: 32KB
 * RAM: 2KB
 * EEPROM: 1KB
 * 引脚: 32-pin TQFP / 28-pin DIP
 *
 * 主要外设：
 *   - GPIO: 3个port (PORTA/B/C)，共28个可编程I/O
 *   - UART: 2个串口 (USART0/USART1)
 *   - SPI: 1个主/从模式
 *   - I2C: TWI（Two-Wire Interface，兼容I2C）
 *   - ADC: 8通道 10位，Vref支持内部/外部
 *   - Timer: 3个（Timer0/1/2），16位Timer1
 *   - 中断: 26个中断源
 * ================================================================ */

/* ================================================================
 * 1. GPIO 寄存器操作（最基础的面试题）
 * ================================================================
 *
 * ATmega328PB 的 GPIO 由以下寄存器控制：
 *   - DDRx:  Data Direction Register (0=输入 1=输出)
 *   - PORTx: Output Register (写入驱动GPIO)
 *   - PINx:  Input Register (读取GPIO状态)
 *
 * 面试常问：
 *   (1) 如何配置某个pin为输出？
 *   (2) 如何在不改变其他pin的情况下修改某个pin？
 *   (3) 位操作宏实现
 */

/* --- GPIO 位操作宏 --- */
#define SET_BIT(port, bit)    ((port) |=  (1U << (bit)))   /* 置1 */
#define CLR_BIT(port, bit)    ((port) &= ~(1U << (bit)))   /* 清0 */
#define TGL_BIT(port, bit)    ((port) ^=  (1U << (bit)))   /* 翻转 */
#define GET_BIT(port, bit)    (((port) >> (bit)) & 1U)     /* 读取 */

/* 例：PB5是Arduino UNO的LED引脚 */
void gpio_init_pb5_output(void)
{
    /* 伪码：实际编译时需要真实的寄存器地址 */
    /* DDRB |= (1 << 5);    // 配置PB5为输出 */
    /* PORTB &= ~(1 << 5);  // 初始化为低电平 */
}

void gpio_set_pb5_high(void)
{
    /* PORTB |= (1 << 5); */
}

void gpio_set_pb5_low(void)
{
    /* PORTB &= ~(1 << 5); */
}

uint8_t gpio_read_pb5(void)
{
    /* return (PINB >> 5) & 1; */
    return 0;
}

/* ================================================================
 * 2. UART 通信（串口调试最常用）
 * ================================================================
 *
 * 面试重点：
 *   - 波特率计算：UBRR = (f_osc / 16 * BAUD) - 1
 *   - 发送/接收函数实现
 *   - 中断驱动 vs 轮询
 *   - 双buffer环形缓冲
 */

/* 假设F_CPU = 16MHz，波特率 = 9600 */
#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VAL ((F_CPU / (16UL * BAUD)) - 1UL)

/**
 * UART 初始化
 * 面试：解释 UCSRnA/B/C 的各个位的作用
 */
void uart_init(void)
{
    /*
     * 伪码（真实代码需包含 <avr/io.h>）：
     *
     * // 设置波特率
     * UBRR0H = (uint8_t)(UBRR_VAL >> 8);
     * UBRR0L = (uint8_t)UBRR_VAL;
     *
     * // UCSR0C: 帧格式（8bit, 1 stop, no parity）
     * UCSR0C = (0 << UMSEL01) | (0 << UMSEL00) |    // 异步
     *          (0 << UPM01)   | (0 << UPM00)   |    // 无奇偶校验
     *          (0 << USBS0)   |                      // 1 stop bit
     *          (3 << UCSZ00);                        // 8 data bits
     *
     * // UCSR0B: 使能收发中断
     * UCSR0B = (1 << RXEN0)  |    // 使能接收
     *          (1 << TXEN0)  |    // 使能发送
     *          (1 << RXCIE0);     // 接收完成中断使能
     */
}

/**
 * UART 发送单字节（轮询）
 * 面试：为什么要等待 UDRE0？
 */
void uart_tx_byte(uint8_t byte)
{
    /*
     * // 等待发送缓冲空
     * while (!(UCSR0A & (1 << UDRE0))) {
     *     ;
     * }
     * // 发送数据
     * UDR0 = byte;
     */
}

/**
 * UART 接收单字节（轮询）
 * 面试：如何判断接收完成？
 */
uint8_t uart_rx_byte(void)
{
    /*
     * // 等待接收完成
     * while (!(UCSR0A & (1 << RXC0))) {
     *     ;
     * }
     * // 返回接收到的数据
     * return UDR0;
     */
    return 0;
}

/**
 * UART 发送字符串
 * 面试常考
 */
void uart_tx_string(const char *str)
{
    if (!str) return;
    while (*str != '\0') {
        uart_tx_byte((uint8_t)*str++);
    }
}

/* ================================================================
 * 3. 定时器/计数器（Timer0 / Timer1 / Timer2）
 * ================================================================
 *
 * 面试重点：
 *   - 时钟源选择（预分频）
 *   - PWM 模式（Fast / Phase Correct）
 *   - 中断（COMP / OVF）
 *   - CTC 模式（清零对比匹配）
 *
 * Timer1 是 16位，常用于精确延时和PWM
 */

/**
 * Timer1 初始化 - CTC 模式（Clear Timer on Compare）
 *
 * 应用：定时中断，精确延时
 * 公式：T = (1 + OCR1A) * 预分频 / F_CPU
 *
 * 例：F_CPU=16MHz，预分频=64，OCR1A=249
 *     T = (1 + 249) * 64 / 16000000 = 0.001s = 1ms
 */
void timer1_init_1ms(void)
{
    /*
     * // CTC 模式，WGM13:0 = 0100 (CTC with OCR1A)
     * TCCR1A = 0x00;
     * TCCR1B = (1 << WGM12) | (3 << CS10);  // CTC + 64分频
     *
     * // 1ms: OCR1A = (F_CPU / 64 / 1000) - 1 = 249
     * OCR1A = 249;
     *
     * // 使能中断
     * TIMSK1 = (1 << OCIE1A);
     */
}

/**
 * Timer1 PWM 初始化 - 快速PWM模式
 *
 * 应用：LED调亮度、电机速度控制
 * PWM频率：f_PWM = F_CPU / (N * (1 + TOP))
 * 占空比：OCR1A / (TOP + 1)
 */
void timer1_init_pwm(void)
{
    /*
     * // 快速PWM，WGM13:0 = 1110 (Fast PWM, TOP=ICR1)
     * TCCR1A = (1 << COM1A1) | (1 << WGM11);   // OC1A非反向
     * TCCR1B = (1 << WGM13)  | (1 << WGM12) | (1 << CS10);
     *
     * // 设置TOP值
     * ICR1 = 19999;   // 约 400Hz@16MHz
     *
     * // 设置占空比（0~ICR1）
     * OCR1A = 10000;  // 50%占空比
     */
}

/* ================================================================
 * 4. ADC（模数转换）
 * ================================================================
 *
 * 面试重点：
 *   - Vref 选择（AREF / Vcc / 内部1.1V）
 *   - 分频因子（ADPS）
 *   - 单次转换 vs 自由运行
 *   - 中断驱动数据采集
 *
 * 公式：数字值 = (模拟电压 / Vref) * 1023
 */

/**
 * ADC 初始化
 * Vref = Vcc (5V)，分频32（约500kHz@16MHz）
 */
void adc_init(void)
{
    /*
     * // ADMUX: 选择 ADC0，Vref=Vcc
     * ADMUX = (0 << REFS1) | (1 << REFS0) |  // Vref = Vcc
     *         (0 << ADLAR) |                  // 右对齐（获得10位）
     *         (0 << MUX3)  | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);
     *
     * // ADCSRA: 使能ADC，分频32，使能中断
     * ADCSRA = (1 << ADEN)   |    // 使能ADC
     *          (1 << ADIE)   |    // 转换完成中断
     *          (1 << ADPS2)  | (1 << ADPS0);  // 分频32
     */
}

/**
 * ADC 启动单次转换
 */
void adc_start_single(void)
{
    /* ADCSRA |= (1 << ADSC);  // 启动转换 */
}

/**
 * ADC 启动自由运行模式
 */
void adc_start_free_running(void)
{
    /*
     * // ADCSRB: 自由运行模式
     * ADCSRB &= ~((1 << ADTS2) | (1 << ADTS1) | (1 << ADTS0));
     *
     * // 启动自由运行
     * ADCSRA |= (1 << ADATE);  // 自动触发使能
     * ADCSRA |= (1 << ADSC);   // 开始转换
     */
}

/**
 * ADC 读取结果（10位）
 */
uint16_t adc_read(void)
{
    /* return ADC; */  /* 需要读取 ADCL 和 ADCH，自动合并 */
    return 0;
}

/* ================================================================
 * 5. 中断机制（MCU中断系统详解）
 * ================================================================
 *
 * 面试常问：
 *   - 中断分类（外部中断、内部中断）
 *   - 中断优先级
 *   - 中断嵌套
 *   - 中断响应时间
 *
 * ATmega328PB 外部中断：INT0(D2), INT1(D3), PCINT0~23
 */

/**
 * 外部中断 INT0 初始化
 * 下降沿触发
 */
void ext_int0_init(void)
{
    /*
     * // EICRA: 外部中断控制
     * // ISC01:0 = 10: 下降沿
     * EICRA = (1 << ISC01) | (0 << ISC00);
     *
     * // EIMSK: 外部中断掩码
     * EIMSK |= (1 << INT0);
     */
}

/**
 * 外部中断 INT0 的 ISR（中断服务程序）
 * 面试：解释 ISR 执行流程和恢复上下文的方式
 */
/* ISR(INT0_vect) { ... } */

/**
 * 全局中断使能/禁用
 */
void interrupts_enable(void)
{
    /* sei();  // cli() 在 avr/interrupt.h */
}

void interrupts_disable(void)
{
    /* cli(); */
}

/* ================================================================
 * 6. I2C / TWI（两线接口，常用传感器通信）
 * ================================================================
 *
 * 面试重点：
 *   - 主机/从机模式
 *   - START / STOP / ACK / NAK
 *   - 状态机（TWSR状态代码）
 *   - SCL / SDA 引脚
 */

/**
 * I2C 主机初始化
 * Fosc=16MHz, Freq=100kHz
 * TWBR = (Fosc - 16*Freq) / (2*Freq*Prescaler)
 */
void i2c_master_init(void)
{
    /*
     * // 设置速率：100kHz@16MHz
     * TWBR = 72;
     * TWSR = 0;  // 预分频 = 1
     *
     * // 使能TWI
     * TWCR = (1 << TWEN);
     */
}

/**
 * I2C 发送 START 条件
 */
void i2c_start(void)
{
    /*
     * TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
     * while (!(TWCR & (1 << TWINT))) { }
     */
}

/**
 * I2C 发送 STOP 条件
 */
void i2c_stop(void)
{
    /*
     * TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
     */
}

/**
 * I2C 发送字节
 */
void i2c_write_byte(uint8_t byte)
{
    /*
     * TWDR = byte;
     * TWCR = (1 << TWINT) | (1 << TWEN);
     * while (!(TWCR & (1 << TWINT))) { }
     * // 检查 TWSR 的状态代码
     */
}

/**
 * I2C 读取字节（含 ACK）
 */
uint8_t i2c_read_byte_ack(void)
{
    /*
     * TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
     * while (!(TWCR & (1 << TWINT))) { }
     * return TWDR;
     */
    return 0;
}

/**
 * I2C 读取字节（含 NAK）
 */
uint8_t i2c_read_byte_nak(void)
{
    /*
     * TWCR = (1 << TWINT) | (1 << TWEN);
     * while (!(TWCR & (1 << TWINT))) { }
     * return TWDR;
     */
    return 0;
}

/* ================================================================
 * 7. SPI 通信（时速更快，常用Flash/SD卡）
 * ================================================================
 *
 * 面试重点：
 *   - CPOL / CPHA（时钟极性和相位）
 *   - 主/从模式
 *   - 数据方向（MSB/LSB）
 *   - SCK、MOSI、MISO、SS
 *
 * ATmega328PB:
 *   PB5 = SCK
 *   PB4 = MISO
 *   PB3 = MOSI
 *   PB2 = SS
 */

/**
 * SPI 主机模式初始化
 * 频率：Fosc/4 = 4MHz（若Fosc=16MHz）
 */
void spi_master_init(void)
{
    /*
     * // SPCR: SPI 控制寄存器
     * SPCR = (1 << SPE)    |    // 使能SPI
     *        (1 << MSTR)   |    // 主机模式
     *        (0 << SPR1)   | (0 << SPR0);  // 分频4
     *
     * // SPSR: SPE前可选的双速模式
     * // SPSR = (1 << SPI2X);  // 如需更高速率
     *
     * // 配置GPIO
     * // DDRB |= (1 << 5) | (1 << 3);  // SCK + MOSI 为输出
     * // DDRB &= ~(1 << 4);             // MISO 为输入
     */
}

/**
 * SPI 主机发送/接收字节
 */
uint8_t spi_exchange_byte(uint8_t byte)
{
    /*
     * SPDR = byte;
     * while (!(SPSR & (1 << SPIF))) { }
     * return SPDR;
     */
    return 0;
}

/* ================================================================
 * 8. 面试常见的综合题：简单温度监测系统
 * ================================================================
 *
 * 需求：
 *   1. 通过 ADC 读取温度传感器（LM35）
 *   2. 通过 UART 输出温度值
 *   3. 超过 30°C 时点亮 LED（GPIO）
 *   4. 使用 Timer1 每 1s 采样一次
 *
 * 考察点：
 *   - ADC 中断驱动设计
 *   - Timer 中断编程
 *   - 状态机思想
 */

/* 全局状态变量 */
volatile uint16_t adc_result = 0;
volatile uint8_t  sample_ready = 0;

/**
 * 面试题：完整的温度监测系统初始化
 */
void temperature_monitor_init(void)
{
    /* uart_init();           // 初始化串口用于输出调试信息 */
    /* gpio_init_pb5_output();      // 初始化 PB5（LED） */
    /* adc_init();            // 初始化 ADC，采用中断模式 */
    /* timer1_init_1ms();     // 初始化定时器 */
    /* interrupts_enable();   // 全局中断使能 */
}

/**
 * 温度采样主循环（协程思想）
 */
void temperature_monitor_loop(void)
{
    if (sample_ready) {
        sample_ready = 0;

        /* LM35 输出 10mV/°C，ADC 10bit，Vref=5V
         * 温度(°C) = ADC值 * 5.0 / 1024 / 0.01
         *          = ADC值 * 0.4883
         */
        uint16_t temp_x10 = (adc_result * 488) / 100;  /* 避免浮点，保留两位小数 */
        uint8_t temp_int  = temp_x10 / 10;

        /* 输出到串口 */
        char msg[16];
        /* snprintf(msg, 16, "Temp:%d C\r\n", temp_int); */
        /* uart_tx_string(msg); */

        /* 超过 30°C 点亮 LED */
        if (temp_int > 30) {
            gpio_set_pb5_high();
        } else {
            gpio_set_pb5_low();
        }
    }
}

/* ================================================================
 * 面试常见概念性问题汇总
 * ================================================================
 *
 * Q1: ATmega328PB 与 ATmega328 的区别？
 * A:  328PB 多了一个 USART（USART1），引脚和改进。
 *
 * Q2: 如何快速测量代码执行时间？
 * A:  利用 GPIO 在关键代码段前后翻转电平，用示波器测量脉宽。
 *
 * Q3: 为什么不能在 ISR 中调用 uart_tx_byte(轮询) 模式？
 * A:  中断上下文中轮询会卡住，应使用中断驱动或队列。
 *
 * Q4: 浮点运算对 MCU 的影响？
 * A:  消耗 CPU 周期多（无FPU），响应延迟增加。建议用定点数学或
 *      查表法。
 *
 * Q5: 如何验证 Bootloader 完整性？
 * A:  计算 CRC/Checksum，与存储的值对比。
 *
 * Q6: 看门狗定时器的用途？
 * A:  看门狗可以在 MCU 陷入死循环时自动重启。
 *      WDR 指令喂狗，超时未喂则复位。
 *
 * Q7: 为什么要配置 ADC 的预分频因子？
 * A:  ADC 需要特定时钟频率（50~200kHz）以获得最大精度。
 *
 * Q8: I2C 和 SPI 的对比？
 * A:  I2C: 低速（100kHz~400kHz），线少(2)，可多主多从
 *      SPI: 高速（几MHz），线多(4)，单主一个从
 */

int main(void)
{
    printf("ATmega328PB 嵌入式面试复习资料\n");
    printf("覆盖：GPIO / UART / Timer / ADC / I2C / SPI\n");
    printf("常见面试题目均已标注\n");

    return 0;
}
