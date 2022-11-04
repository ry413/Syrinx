
#include "st7701s.h"


//硬件 或 软件SPI
#define hardware_spi

#define LCD_PIN_MOSI        9  /*!< for 1-line SPI, this also refered as MOSI */
#define LCD_PIN_SCLK		10
#define LCD_PIN_CS          0


#if	defined(hardware_spi)
static spi_device_handle_t g_spi;
#else
#define Delay(t) vTaskDelay(pdMS_TO_TICKS(t))
#define gpio_set_value(_p, _v) gpio_set_level(_p, _v)
#define delay_ms(_t) esp_rom_delay_us((_t)*1000)
#define delay_us(_t) esp_rom_delay_us(_t)

#define LCD_CS_Clr()    gpio_set_level(LCD_PIN_CS, 0)
#define LCD_CS_Set()    gpio_set_level(LCD_PIN_CS, 1)
#define LCD_SCK_Clr()   gpio_set_level(LCD_PIN_SCLK, 0)
#define LCD_SCK_Set()   gpio_set_level(LCD_PIN_SCLK, 1)
#define LCD_SDA_Clr()   gpio_set_level(LCD_PIN_MOSI, 0)
#define LCD_SDA_Set()   gpio_set_level(LCD_PIN_MOSI, 1)

static void spi_soft_write_9bits(uint16_t data)
{
	uint8_t i;
	LCD_CS_Clr();
	for(i = 0; i < 9; i++)
	{
        LCD_SCK_Clr();
		if(data & 0x100)   LCD_SDA_Set();
		else               LCD_SDA_Clr();
		delay_us(30);
        LCD_SCK_Set();
		delay_us(30);
		data <<= 1;
	}
	LCD_CS_Set();;
}
#endif
static void __spi_send_cmd(uint8_t cmd)
{
#if	defined(hardware_spi)
    uint16_t tmp_cmd = (cmd | 0x0000);;
    spi_transaction_ext_t trans = (spi_transaction_ext_t)
    {
        .base =
        {
            .flags = SPI_TRANS_VARIABLE_CMD,
            .cmd = tmp_cmd,
        },
        .command_bits = 9,
    };
    spi_device_transmit(g_spi, (spi_transaction_t *)&trans);
#else
	uint16_t temp = 0;
	temp = temp | cmd;
	spi_soft_write_9bits(temp);
#endif
}

static void __spi_send_data(uint8_t data)
{
#if	defined(hardware_spi)
    uint16_t tmp_data = (data | 0x0100);
    spi_transaction_ext_t trans = (spi_transaction_ext_t){
        .base = {
            .flags = SPI_TRANS_VARIABLE_CMD,
            .cmd = tmp_data,
        },
        .command_bits = 9,
    };
    spi_device_transmit(g_spi, (spi_transaction_t *)&trans);
#else
	uint16_t temp = 0x100;
	temp = temp | data;
	spi_soft_write_9bits(temp);
#endif
}

static void __lcd_init(void)
{
#if defined(CONFIG_EXAMPLE_LCD_PANEL_ST7701S)//深圳博虎 ST7701S 480*480屏
	//Software reset
	__spi_send_cmd (0x11);
	__spi_send_data (0x00);
	vTaskDelay(pdMS_TO_TICKS(500));
   //PAGE1
	__spi_send_cmd(0xFF);
	__spi_send_data(0x77);
	__spi_send_data(0x01);
	__spi_send_data(0x00);
	__spi_send_data(0x00);
	__spi_send_data(0x10);

	__spi_send_cmd(0xC0);
	__spi_send_data(0x3B);//屏的分辨率
	__spi_send_data(0x00);

	__spi_send_cmd(0xC1);
	__spi_send_data(0x0D);//VBP 如果配置为>0B屏幕上边有黑边显示不完整
	__spi_send_data(0x02);

	__spi_send_cmd(0xC2);
	__spi_send_data(0x31);
	__spi_send_data(0x05);

	__spi_send_cmd(0xCd);
	__spi_send_data(0x08);

	__spi_send_cmd(0xB0);
	__spi_send_data(0x00); //Positive Voltage Gamma Control
	__spi_send_data(0x11);
	__spi_send_data(0x18);
	__spi_send_data(0x0E);
	__spi_send_data(0x11);
	__spi_send_data(0x06);
	__spi_send_data(0x07);
	__spi_send_data(0x08);
	__spi_send_data(0x07);
	__spi_send_data(0x22);
	__spi_send_data(0x04);
	__spi_send_data(0x12);
	__spi_send_data(0x0F);
	__spi_send_data(0xAA);
	__spi_send_data(0x31);
	__spi_send_data(0x18);

	__spi_send_cmd(0xB1);
	__spi_send_data(0x00); //Negative Voltage Gamma Control
	__spi_send_data(0x11);
	__spi_send_data(0x19);
	__spi_send_data(0x0E);
	__spi_send_data(0x12);
	__spi_send_data(0x07);
	__spi_send_data(0x08);
	__spi_send_data(0x08);
	__spi_send_data(0x08);
	__spi_send_data(0x22);
	__spi_send_data(0x04);
	__spi_send_data(0x11);
	__spi_send_data(0x11);
	__spi_send_data(0xA9);
	__spi_send_data(0x32);
	__spi_send_data(0x18);

	//PAGE1
	__spi_send_cmd(0xFF);
	__spi_send_data(0x77);
	__spi_send_data(0x01);
	__spi_send_data(0x00);
	__spi_send_data(0x00);
	__spi_send_data(0x11);

	__spi_send_cmd(0xB0);    __spi_send_data(0x60); //Vop=4.7375v
	__spi_send_cmd(0xB1);    __spi_send_data(0x32); //VCOM=32
	__spi_send_cmd(0xB2);    __spi_send_data(0x07); //VGH=15v
	__spi_send_cmd(0xB3);    __spi_send_data(0x80);
	__spi_send_cmd(0xB5);    __spi_send_data(0x49); //VGL=-10.17v
	__spi_send_cmd(0xB7);    __spi_send_data(0x85);
	__spi_send_cmd(0xB8);    __spi_send_data(0x21); //AVDD=6.6 & AVCL=-4.6
	__spi_send_cmd(0xC1);    __spi_send_data(0x78);
	__spi_send_cmd(0xC2);    __spi_send_data(0x78);

	__spi_send_cmd(0xE0);
	__spi_send_data(0x00);
	__spi_send_data(0x1B);
	__spi_send_data(0x02);

	__spi_send_cmd(0xE1);
	__spi_send_data(0x08);
	__spi_send_data(0xA0);
	__spi_send_data(0x00);
	__spi_send_data(0x00);
	__spi_send_data(0x07);
	__spi_send_data(0xA0);
	__spi_send_data(0x00);
	__spi_send_data(0x00);
	__spi_send_data(0x00);
	__spi_send_data(0x44);
	__spi_send_data(0x44);

	__spi_send_cmd(0xE2);
	__spi_send_data(0x11);
	__spi_send_data(0x11);
	__spi_send_data(0x44);
	__spi_send_data(0x44);
	__spi_send_data(0xED);
	__spi_send_data(0xA0);
	__spi_send_data(0x00);
	__spi_send_data(0x00);
	__spi_send_data(0xEC);
	__spi_send_data(0xA0);
	__spi_send_data(0x00);
	__spi_send_data(0x00);

	__spi_send_cmd(0xE3);
	__spi_send_data(0x00);
	__spi_send_data(0x00);
	__spi_send_data(0x11);
	__spi_send_data(0x11);

	__spi_send_cmd(0xE4);
	__spi_send_data(0x44);
	__spi_send_data(0x44);

	__spi_send_cmd(0xE5);
	__spi_send_data(0x0A);
	__spi_send_data(0xE9);
	__spi_send_data(0xD8);
	__spi_send_data(0xA0);
	__spi_send_data(0x0C);
	__spi_send_data(0xEB);
	__spi_send_data(0xD8);
	__spi_send_data(0xA0);
	__spi_send_data(0x0E);
	__spi_send_data(0xED);
	__spi_send_data(0xD8);
	__spi_send_data(0xA0);
	__spi_send_data(0x10);
	__spi_send_data(0xEF);
	__spi_send_data(0xD8);
	__spi_send_data(0xA0);

	__spi_send_cmd(0xE6);
	__spi_send_data(0x00);
	__spi_send_data(0x00);
	__spi_send_data(0x11);
	__spi_send_data(0x11);

	__spi_send_cmd(0xE7);
	__spi_send_data(0x44);
	__spi_send_data(0x44);

	__spi_send_cmd(0xE8);
	__spi_send_data(0x09);
	__spi_send_data(0xE8);
	__spi_send_data(0xD8);
	__spi_send_data(0xA0);
	__spi_send_data(0x0B);
	__spi_send_data(0xEA);
	__spi_send_data(0xD8);
	__spi_send_data(0xA0);
	__spi_send_data(0x0D);
	__spi_send_data(0xEC);
	__spi_send_data(0xD8);
	__spi_send_data(0xA0);
	__spi_send_data(0x0F);
	__spi_send_data(0xEE);
	__spi_send_data(0xD8);
	__spi_send_data(0xA0);

	__spi_send_cmd(0xEB);
	__spi_send_data(0x02);
	__spi_send_data(0x00);
	__spi_send_data(0xE4);
	__spi_send_data(0xE4);
	__spi_send_data(0x88);
	__spi_send_data(0x00);
	__spi_send_data(0x40);

	__spi_send_cmd(0xEC);
	__spi_send_data(0x3C);
	__spi_send_data(0x00);

	__spi_send_cmd(0xED);
	__spi_send_data(0xAB);
	__spi_send_data(0x89);
	__spi_send_data(0x76);
	__spi_send_data(0x54);
	__spi_send_data(0x02);
	__spi_send_data(0xFF);
	__spi_send_data(0xFF);
	__spi_send_data(0xFF);
	__spi_send_data(0xFF);
	__spi_send_data(0xFF);
	__spi_send_data(0xFF);
	__spi_send_data(0x20);
	__spi_send_data(0x45);
	__spi_send_data(0x67);
	__spi_send_data(0x98);
	__spi_send_data(0xBA);

	__spi_send_cmd(0x36);
	__spi_send_data(0x00);

	//-----------VAP & VAN---------------
	__spi_send_cmd(0xFF);
	__spi_send_data(0x77);
	__spi_send_data(0x01);
	__spi_send_data(0x00);
	__spi_send_data(0x00);
	__spi_send_data(0x13);

	__spi_send_cmd(0xE5);
	__spi_send_data(0xE4);

	__spi_send_cmd(0xFF);
	__spi_send_data(0x77);
	__spi_send_data(0x01);
	__spi_send_data(0x00);
	__spi_send_data(0x00);
	__spi_send_data(0x00);

	__spi_send_cmd(0x3A);   //0x70 RGB888, 0x60 RGB666, 0x50 RGB565
	__spi_send_data(0x60);

	__spi_send_cmd(0x21);   //Display Inversion On

	__spi_send_cmd(0x11);   //Sleep Out
	vTaskDelay(pdMS_TO_TICKS(100));

	__spi_send_cmd(0x29);   //Display On
	vTaskDelay(pdMS_TO_TICKS(50));	
#elif defined(CONFIG_EXAMPLE_LCD_PANEL_GC9503V)	//启明GC9503V 480*480屏
	__spi_send_cmd (0xF0);
	__spi_send_data (0x55);
	__spi_send_data (0xAA);
	__spi_send_data (0x52);
	__spi_send_data (0x08);
	__spi_send_data (0x00);

	__spi_send_cmd (0xF6);
	__spi_send_data (0x5A);
	__spi_send_data (0x87);

	__spi_send_cmd (0xC1);
	__spi_send_data (0x3F);

	__spi_send_cmd (0xC2);
	__spi_send_data (0x0E);

	__spi_send_cmd (0xC6);
	__spi_send_data (0xF8);

	__spi_send_cmd (0xC9);
	__spi_send_data (0x10);

	__spi_send_cmd (0xCD);
	__spi_send_data (0x25);

	__spi_send_cmd (0xF8);
	__spi_send_data (0x8A);

	__spi_send_cmd (0xAC);
	__spi_send_data (0x45);

	__spi_send_cmd (0xA0);
	__spi_send_data (0xDD);

	__spi_send_cmd (0xA7);
	__spi_send_data (0x47);

	__spi_send_cmd (0xFA);
	__spi_send_data (0x00);
	__spi_send_data (0x00);
	__spi_send_data (0x00);
	__spi_send_data (0x04);

	__spi_send_cmd (0xA3);
	__spi_send_data (0xEE);

	__spi_send_cmd (0xFD);
	__spi_send_data (0x28);
	__spi_send_data (0x28);
	__spi_send_data (0x00);

	__spi_send_cmd (0x71);
	__spi_send_data (0x48);

	__spi_send_cmd (0x72);
	__spi_send_data (0x48);

	__spi_send_cmd (0x73);
	__spi_send_data (0x00);
	__spi_send_data (0x44);

	__spi_send_cmd (0x97);
	__spi_send_data (0xEE);

	__spi_send_cmd (0x83);
	__spi_send_data (0x93);

	__spi_send_cmd (0x9A);
	__spi_send_data (0x72);

	__spi_send_cmd (0x9B);
	__spi_send_data (0x5a);

	__spi_send_cmd (0x82);
	__spi_send_data (0x2c);
	__spi_send_data (0x2c);

	__spi_send_cmd (0xB1);
	__spi_send_data (0x10);

	__spi_send_cmd (0x6D);
	__spi_send_data (0x00);
	__spi_send_data (0x1F);
	__spi_send_data (0x19);
	__spi_send_data (0x1A);
	__spi_send_data (0x10);
	__spi_send_data (0x0e);
	__spi_send_data (0x0c);
	__spi_send_data (0x0a);
	__spi_send_data (0x02);
	__spi_send_data (0x07);
	__spi_send_data (0x1E);
	__spi_send_data (0x1E);
	__spi_send_data (0x1E);
	__spi_send_data (0x1E);
	__spi_send_data (0x1E);
	__spi_send_data (0x1E);
	__spi_send_data (0x1E);
	__spi_send_data (0x1E);
	__spi_send_data (0x1E);
	__spi_send_data (0x1E);
	__spi_send_data (0x1E);
	__spi_send_data (0x1E);
	__spi_send_data (0x08);
	__spi_send_data (0x01);
	__spi_send_data (0x09);
	__spi_send_data (0x0b);
	__spi_send_data (0x0D);
	__spi_send_data (0x0F);
	__spi_send_data (0x1a);
	__spi_send_data (0x19);
	__spi_send_data (0x1f);
	__spi_send_data (0x00);

	__spi_send_cmd (0x64);
	__spi_send_data (0x38);
	__spi_send_data (0x05);
	__spi_send_data (0x01);
	__spi_send_data (0xdb);
	__spi_send_data (0x03);
	__spi_send_data (0x03);
	__spi_send_data (0x38);
	__spi_send_data (0x04);
	__spi_send_data (0x01);
	__spi_send_data (0xdc);
	__spi_send_data (0x03);
	__spi_send_data (0x03);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);

	__spi_send_cmd (0x65);
	__spi_send_data (0x38);
	__spi_send_data (0x03);
	__spi_send_data (0x01);
	__spi_send_data (0xdd);
	__spi_send_data (0x03);
	__spi_send_data (0x03);
	__spi_send_data (0x38);
	__spi_send_data (0x02);
	__spi_send_data (0x01);
	__spi_send_data (0xde);
	__spi_send_data (0x03);
	__spi_send_data (0x03);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);

	__spi_send_cmd (0x66);
	__spi_send_data (0x38);
	__spi_send_data (0x01);
	__spi_send_data (0x01);
	__spi_send_data (0xdf);
	__spi_send_data (0x03);
	__spi_send_data (0x03);
	__spi_send_data (0x38);
	__spi_send_data (0x00);
	__spi_send_data (0x01);
	__spi_send_data (0xe0);
	__spi_send_data (0x03);
	__spi_send_data (0x03);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);

	__spi_send_cmd (0x67);
	__spi_send_data (0x30);
	__spi_send_data (0x01);
	__spi_send_data (0x01);
	__spi_send_data (0xe1);
	__spi_send_data (0x03);
	__spi_send_data (0x03);
	__spi_send_data (0x30);
	__spi_send_data (0x02);
	__spi_send_data (0x01);
	__spi_send_data (0xe2);
	__spi_send_data (0x03);
	__spi_send_data (0x03);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);

	__spi_send_cmd (0x68);
	__spi_send_data (0x00);
	__spi_send_data (0x08);
	__spi_send_data (0x15);
	__spi_send_data (0x08);
	__spi_send_data (0x15);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);
	__spi_send_data (0x08);
	__spi_send_data (0x15);
	__spi_send_data (0x08);
	__spi_send_data (0x15);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);

	__spi_send_cmd (0x60);
	__spi_send_data (0x38);
	__spi_send_data (0x08);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);
	__spi_send_data (0x38);
	__spi_send_data (0x09);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);

	__spi_send_cmd (0x63);
	__spi_send_data (0x31);
	__spi_send_data (0xe4);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);
	__spi_send_data (0x31);
	__spi_send_data (0xe5);
	__spi_send_data (0x7A);
	__spi_send_data (0x7A);

	__spi_send_cmd (0x6B);
	__spi_send_data (0x07);

	__spi_send_cmd (0x7A);
	__spi_send_data (0x08);
	__spi_send_data (0x13);

	__spi_send_cmd (0x7B);
	__spi_send_data (0x08);
	__spi_send_data (0x13);

	__spi_send_cmd (0xD1);
	__spi_send_data (0x00);
	__spi_send_data (0x00);
	__spi_send_data (0x00);
	__spi_send_data (0x04);
	__spi_send_data (0x00);
	__spi_send_data (0x12);
	__spi_send_data (0x00);
	__spi_send_data (0x18);
	__spi_send_data (0x00);
	__spi_send_data (0x21);
	__spi_send_data (0x00);
	__spi_send_data (0x2a);
	__spi_send_data (0x00);
	__spi_send_data (0x35);
	__spi_send_data (0x00);
	__spi_send_data (0x47);
	__spi_send_data (0x00);
	__spi_send_data (0x56);
	__spi_send_data (0x00);
	__spi_send_data (0x90);
	__spi_send_data (0x00);
	__spi_send_data (0xe5);
	__spi_send_data (0x01);
	__spi_send_data (0x68);
	__spi_send_data (0x01);
	__spi_send_data (0xd5);
	__spi_send_data (0x01);
	__spi_send_data (0xd7);
	__spi_send_data (0x02);
	__spi_send_data (0x36);
	__spi_send_data (0x02);
	__spi_send_data (0xa6);
	__spi_send_data (0x02);
	__spi_send_data (0xee);
	__spi_send_data (0x03);
	__spi_send_data (0x48);
	__spi_send_data (0x03);
	__spi_send_data (0xa0);
	__spi_send_data (0x03);
	__spi_send_data (0xba);
	__spi_send_data (0x03);
	__spi_send_data (0xc5);
	__spi_send_data (0x03);
	__spi_send_data (0xd0);
	__spi_send_data (0x03);
	__spi_send_data (0xE0);
	__spi_send_data (0x03);
	__spi_send_data (0xea);
	__spi_send_data (0x03);
	__spi_send_data (0xFa);
	__spi_send_data (0x03);
	__spi_send_data (0xFF);

	__spi_send_cmd (0xD2);
	__spi_send_data (0x00);
	__spi_send_data (0x00);
	__spi_send_data (0x00);
	__spi_send_data (0x04);
	__spi_send_data (0x00);
	__spi_send_data (0x12);
	__spi_send_data (0x00);
	__spi_send_data (0x18);
	__spi_send_data (0x00);
	__spi_send_data (0x21);
	__spi_send_data (0x00);
	__spi_send_data (0x2a);
	__spi_send_data (0x00);
	__spi_send_data (0x35);
	__spi_send_data (0x00);
	__spi_send_data (0x47);
	__spi_send_data (0x00);
	__spi_send_data (0x56);
	__spi_send_data (0x00);
	__spi_send_data (0x90);
	__spi_send_data (0x00);
	__spi_send_data (0xe5);
	__spi_send_data (0x01);
	__spi_send_data (0x68);
	__spi_send_data (0x01);
	__spi_send_data (0xd5);
	__spi_send_data (0x01);
	__spi_send_data (0xd7);
	__spi_send_data (0x02);
	__spi_send_data (0x36);
	__spi_send_data (0x02);
	__spi_send_data (0xa6);
	__spi_send_data (0x02);
	__spi_send_data (0xee);
	__spi_send_data (0x03);
	__spi_send_data (0x48);
	__spi_send_data (0x03);
	__spi_send_data (0xa0);
	__spi_send_data (0x03);
	__spi_send_data (0xba);
	__spi_send_data (0x03);
	__spi_send_data (0xc5);
	__spi_send_data (0x03);
	__spi_send_data (0xd0);
	__spi_send_data (0x03);
	__spi_send_data (0xE0);
	__spi_send_data (0x03);
	__spi_send_data (0xea);
	__spi_send_data (0x03);
	__spi_send_data (0xFa);
	__spi_send_data (0x03);
	__spi_send_data (0xFF);

	__spi_send_cmd (0xD3);
	__spi_send_data (0x00);
	__spi_send_data (0x00);
	__spi_send_data (0x00);
	__spi_send_data (0x04);
	__spi_send_data (0x00);
	__spi_send_data (0x12);
	__spi_send_data (0x00);
	__spi_send_data (0x18);
	__spi_send_data (0x00);
	__spi_send_data (0x21);
	__spi_send_data (0x00);
	__spi_send_data (0x2a);
	__spi_send_data (0x00);
	__spi_send_data (0x35);
	__spi_send_data (0x00);
	__spi_send_data (0x47);
	__spi_send_data (0x00);
	__spi_send_data (0x56);
	__spi_send_data (0x00);
	__spi_send_data (0x90);
	__spi_send_data (0x00);
	__spi_send_data (0xe5);
	__spi_send_data (0x01);
	__spi_send_data (0x68);
	__spi_send_data (0x01);
	__spi_send_data (0xd5);
	__spi_send_data (0x01);
	__spi_send_data (0xd7);
	__spi_send_data (0x02);
	__spi_send_data (0x36);
	__spi_send_data (0x02);
	__spi_send_data (0xa6);
	__spi_send_data (0x02);
	__spi_send_data (0xee);
	__spi_send_data (0x03);
	__spi_send_data (0x48);
	__spi_send_data (0x03);
	__spi_send_data (0xa0);
	__spi_send_data (0x03);
	__spi_send_data (0xba);
	__spi_send_data (0x03);
	__spi_send_data (0xc5);
	__spi_send_data (0x03);
	__spi_send_data (0xd0);
	__spi_send_data (0x03);
	__spi_send_data (0xE0);
	__spi_send_data (0x03);
	__spi_send_data (0xea);
	__spi_send_data (0x03);
	__spi_send_data (0xFa);
	__spi_send_data (0x03);
	__spi_send_data (0xFF);

	__spi_send_cmd (0xD4);
	__spi_send_data (0x00);
	__spi_send_data (0x00);
	__spi_send_data (0x00);
	__spi_send_data (0x04);
	__spi_send_data (0x00);
	__spi_send_data (0x12);
	__spi_send_data (0x00);
	__spi_send_data (0x18);
	__spi_send_data (0x00);
	__spi_send_data (0x21);
	__spi_send_data (0x00);
	__spi_send_data (0x2a);
	__spi_send_data (0x00);
	__spi_send_data (0x35);
	__spi_send_data (0x00);
	__spi_send_data (0x47);
	__spi_send_data (0x00);
	__spi_send_data (0x56);
	__spi_send_data (0x00);
	__spi_send_data (0x90);
	__spi_send_data (0x00);
	__spi_send_data (0xe5);
	__spi_send_data (0x01);
	__spi_send_data (0x68);
	__spi_send_data (0x01);
	__spi_send_data (0xd5);
	__spi_send_data (0x01);
	__spi_send_data (0xd7);
	__spi_send_data (0x02);
	__spi_send_data (0x36);
	__spi_send_data (0x02);
	__spi_send_data (0xa6);
	__spi_send_data (0x02);
	__spi_send_data (0xee);
	__spi_send_data (0x03);
	__spi_send_data (0x48);
	__spi_send_data (0x03);
	__spi_send_data (0xa0);
	__spi_send_data (0x03);
	__spi_send_data (0xba);
	__spi_send_data (0x03);
	__spi_send_data (0xc5);
	__spi_send_data (0x03);
	__spi_send_data (0xd0);
	__spi_send_data (0x03);
	__spi_send_data (0xE0);
	__spi_send_data (0x03);
	__spi_send_data (0xea);
	__spi_send_data (0x03);
	__spi_send_data (0xFa);
	__spi_send_data (0x03);
	__spi_send_data (0xFF);

	__spi_send_cmd (0xD5);
	__spi_send_data (0x00);
	__spi_send_data (0x00);
	__spi_send_data (0x00);
	__spi_send_data (0x04);
	__spi_send_data (0x00);
	__spi_send_data (0x12);
	__spi_send_data (0x00);
	__spi_send_data (0x18);
	__spi_send_data (0x00);
	__spi_send_data (0x21);
	__spi_send_data (0x00);
	__spi_send_data (0x2a);
	__spi_send_data (0x00);
	__spi_send_data (0x35);
	__spi_send_data (0x00);
	__spi_send_data (0x47);
	__spi_send_data (0x00);
	__spi_send_data (0x56);
	__spi_send_data (0x00);
	__spi_send_data (0x90);
	__spi_send_data (0x00);
	__spi_send_data (0xe5);
	__spi_send_data (0x01);
	__spi_send_data (0x68);
	__spi_send_data (0x01);
	__spi_send_data (0xd5);
	__spi_send_data (0x01);
	__spi_send_data (0xd7);
	__spi_send_data (0x02);
	__spi_send_data (0x36);
	__spi_send_data (0x02);
	__spi_send_data (0xa6);
	__spi_send_data (0x02);
	__spi_send_data (0xee);
	__spi_send_data (0x03);
	__spi_send_data (0x48);
	__spi_send_data (0x03);
	__spi_send_data (0xa0);
	__spi_send_data (0x03);
	__spi_send_data (0xba);
	__spi_send_data (0x03);
	__spi_send_data (0xc5);
	__spi_send_data (0x03);
	__spi_send_data (0xd0);
	__spi_send_data (0x03);
	__spi_send_data (0xE0);
	__spi_send_data (0x03);
	__spi_send_data (0xea);
	__spi_send_data (0x03);
	__spi_send_data (0xFa);
	__spi_send_data (0x03);
	__spi_send_data (0xFF);

	__spi_send_cmd (0xD6);
	__spi_send_data (0x00);
	__spi_send_data (0x00);
	__spi_send_data (0x00);
	__spi_send_data (0x04);
	__spi_send_data (0x00);
	__spi_send_data (0x12);
	__spi_send_data (0x00);
	__spi_send_data (0x18);
	__spi_send_data (0x00);
	__spi_send_data (0x21);
	__spi_send_data (0x00);
	__spi_send_data (0x2a);
	__spi_send_data (0x00);
	__spi_send_data (0x35);
	__spi_send_data (0x00);
	__spi_send_data (0x47);
	__spi_send_data (0x00);
	__spi_send_data (0x56);
	__spi_send_data (0x00);
	__spi_send_data (0x90);
	__spi_send_data (0x00);
	__spi_send_data (0xe5);
	__spi_send_data (0x01);
	__spi_send_data (0x68);
	__spi_send_data (0x01);
	__spi_send_data (0xd5);
	__spi_send_data (0x01);
	__spi_send_data (0xd7);
	__spi_send_data (0x02);
	__spi_send_data (0x36);
	__spi_send_data (0x02);
	__spi_send_data (0xa6);
	__spi_send_data (0x02);
	__spi_send_data (0xee);
	__spi_send_data (0x03);
	__spi_send_data (0x48);
	__spi_send_data (0x03);
	__spi_send_data (0xa0);
	__spi_send_data (0x03);
	__spi_send_data (0xba);
	__spi_send_data (0x03);
	__spi_send_data (0xc5);
	__spi_send_data (0x03);
	__spi_send_data (0xd0);
	__spi_send_data (0x03);
	__spi_send_data (0xE0);
	__spi_send_data (0x03);
	__spi_send_data (0xea);
	__spi_send_data (0x03);
	__spi_send_data (0xFa);
	__spi_send_data (0x03);
	__spi_send_data (0xFF);

	__spi_send_cmd (0x3a);
	__spi_send_data (0x66);

	__spi_send_cmd (0x11);
	vTaskDelay(pdMS_TO_TICKS(200));
	__spi_send_cmd (0x29);
#else
	#error "no support panel"	
#endif
}

void st7701s_init(void)
{
	#if	defined(hardware_spi)
    spi_bus_config_t buscfg = {
        .sclk_io_num = LCD_PIN_SCLK,
        .mosi_io_num = LCD_PIN_MOSI,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 10 * 1024,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg =
    {
        .clock_speed_hz = SPI_MASTER_FREQ_10M,  //Clock out at 10 MHz
        .mode = 0,                              //SPI mode 0
        .spics_io_num = LCD_PIN_CS,             //CS pin
        .queue_size = 7,                        //We want to be able to queue 7 transactions at a time
    };
    
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &g_spi));
    __lcd_init();
    //st7701s初始化完成后，如果SCLK MOSI复用 释放SPI 让CS置于高电平
    spi_bus_remove_device(g_spi);
	spi_bus_free(SPI2_HOST);
#else
    gpio_reset_pin(LCD_PIN_CS);
	gpio_set_direction(LCD_PIN_CS, GPIO_MODE_OUTPUT);
    gpio_set_level(LCD_PIN_CS, 1);

    gpio_reset_pin(LCD_PIN_SCLK);
	gpio_set_direction(LCD_PIN_SCLK, GPIO_MODE_OUTPUT);
    gpio_set_level(LCD_PIN_SCLK, 1);

    gpio_reset_pin(LCD_PIN_MOSI);
	gpio_set_direction(LCD_PIN_MOSI, GPIO_MODE_OUTPUT);
    gpio_set_level(LCD_PIN_MOSI, 1);

    __lcd_init();
#endif
    //如果SCLK MOSI复用 释放SPI 让CS置于高电平
//    gpio_reset_pin(LCD_PIN_CS);
    gpio_reset_pin(LCD_PIN_SCLK);
    gpio_reset_pin(LCD_PIN_MOSI);

    gpio_reset_pin(LCD_PIN_CS);
	gpio_set_direction(LCD_PIN_CS, GPIO_MODE_OUTPUT);
    gpio_set_level(LCD_PIN_CS, 1);
}
