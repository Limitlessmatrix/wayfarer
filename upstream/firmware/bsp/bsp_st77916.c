#include "pico/stdlib.h"

#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#include "pio_qspi.h"

#include "bsp_st77916.h"

static bsp_display_interface_t *g_display_if;
static bsp_display_info_t *g_display_info;

static uint slice_num;
static uint pwm_channel;

typedef struct
{
    uint8_t reg;           /*<! The specific LCD command */
    uint8_t *data;         /*<! Buffer that holds the command specific data */
    size_t data_bytes;     /*<! Size of `data` in memory, in bytes */
    unsigned int delay_ms; /*<! Delay in milliseconds after this command */
} bsp_st77916_cmd_t;

static void bsp_st77916_tx_param(bsp_st77916_cmd_t *cmds, size_t cmd_len)
{
    for (int i = 0; i < cmd_len; i++)
    {
        uint8_t write_buffer[4 + cmds[i].data_bytes];
        gpio_put(BSP_LCD_CS_PIN, 0);

        write_buffer[0] = 0x02;
        write_buffer[1] = 0x00;
        write_buffer[2] = cmds[i].reg;
        write_buffer[3] = 0x00;
        for (int j = 0; j < cmds[i].data_bytes; j++)
        {
            write_buffer[4 + j] = cmds[i].data[j];
        }
        pio_qspi_1bit_write_data_blocking(write_buffer, 4 + cmds[i].data_bytes);

        gpio_put(BSP_LCD_CS_PIN, 1);
        // printf("delay_ms:%d\r\n", cmds[i].delay_ms);
        if (cmds[i].delay_ms > 0)
        {
            // printf("delay_ms:%d\r\n", cmds[i].delay_ms);
            sleep_ms(cmds[i].delay_ms);
        }
    }
}

static void bsp_st77916_set_window(bsp_display_area_t *area)
{
    bsp_st77916_cmd_t cmds[2];

    uint16_t x_start = area->x1 + g_display_info->x_offset;
    uint16_t x_end = area->x2 + g_display_info->x_offset;

    uint16_t y_start = area->y1 + g_display_info->y_offset;
    uint16_t y_end = area->y2 + g_display_info->y_offset;

    uint8_t x_data[4];
    uint8_t y_data[4];
    x_data[0] = (x_start >> 8) & 0xFF;
    x_data[1] = x_start & 0xFF;
    x_data[2] = (x_end >> 8) & 0xFF;
    x_data[3] = x_end & 0xFF;

    y_data[0] = (y_start >> 8) & 0xFF;
    y_data[1] = y_start & 0xFF;
    y_data[2] = (y_end >> 8) & 0xFF;
    y_data[3] = y_end & 0xFF;

    cmds[0].reg = 0x2a;
    cmds[0].data = x_data;
    cmds[0].data_bytes = 4;
    cmds[0].delay_ms = 0;

    cmds[1].reg = 0x2b;
    cmds[1].data = y_data;
    cmds[1].data_bytes = 4;
    cmds[1].delay_ms = 0;

    bsp_st77916_tx_param(cmds, 2);
}

void bsp_lcd_set_brightness(uint8_t percent)
{
    if (percent > 100)
    {
        percent = 100;
    }
    pwm_set_chan_level(slice_num, pwm_channel, PWM_WRAP / 100 * percent);
    g_display_info->brightness = percent;
}

static void bsp_st77916_set_rotation(uint16_t rotation)
{
    uint8_t data;
    g_display_info->rotation = rotation;

    bsp_st77916_cmd_t cmd;
    cmd.reg = 0x36;
    cmd.data_bytes = 1;
    switch (rotation)
    {
    case 90:
        data = 0x60;
        break;
    case 180:
        data = 0xC0;
        break;
    case 270:
        data = 0xA0;
        break;
    default:
        g_display_info->rotation = 0;
        data = 0x0;
        break;
    }
    cmd.data = &data;
    bsp_st77916_tx_param(&cmd, 1);
}

static uint16_t bsp_st77916_get_rotation()
{
    return g_display_info->rotation;
}

static uint8_t bsp_lcd_get_brightness()
{
    return g_display_info->brightness;
}

static void bsp_st77916_flush(bsp_display_area_t *area, uint16_t *color_p)
{
}

static void bsp_st77916_flush_dma(bsp_display_area_t *area, uint16_t *color_p)
{
    uint8_t cmd_arr[4];
    cmd_arr[0] = 0x32;
    cmd_arr[1] = 0x00;
    cmd_arr[2] = 0x2c;
    cmd_arr[3] = 0x00;
    size_t color_len = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1);
    bsp_st77916_set_window(area);
    gpio_put(BSP_LCD_CS_PIN, 0);
    pio_qspi_1bit_write_data_blocking(cmd_arr, 4);
    pio_qspi_4bit_write_data((uint8_t *)color_p, color_len * 2);
}

static void __no_inline_not_in_flash_func(bsp_st77916_flush_dma_done_cb)(void)
{
    __asm__ volatile("nop");
    __asm__ volatile("nop");
    gpio_put(BSP_LCD_CS_PIN, 1);
    if (g_display_info->dma_flush_done_cb)
    {
        g_display_info->dma_flush_done_cb();
    }
}

static void bsp_st77916_gpio_init(void)
{
    gpio_init(BSP_LCD_CS_PIN);
    gpio_init(BSP_LCD_RST_PIN);
    gpio_init(BSP_LCD_TE_PIN);

    gpio_set_dir(BSP_LCD_CS_PIN, GPIO_OUT);
    gpio_set_dir(BSP_LCD_RST_PIN, GPIO_OUT);
    gpio_set_dir(BSP_LCD_TE_PIN, GPIO_IN);

    gpio_pull_up(BSP_LCD_TE_PIN);
}

static void bsp_lcd_brightness_init(void)
{
    float sys_clk = clock_get_hz(clk_sys);
    gpio_set_function(BSP_LCD_BL_PIN, GPIO_FUNC_PWM);
    // Find out which PWM slice is connected to GPIO 0 (it's slice 0)
    slice_num = pwm_gpio_to_slice_num(BSP_LCD_BL_PIN);

    pwm_channel = pwm_gpio_to_channel(BSP_LCD_BL_PIN);

    pwm_set_clkdiv(slice_num, sys_clk / (PWM_FREQ * PWM_WRAP));

    pwm_set_wrap(slice_num, PWM_WRAP);

    pwm_set_chan_level(slice_num, pwm_channel, 0);

    pwm_set_enabled(slice_num, true);
}

static void bsp_lcd_reset(void)
{
    gpio_put(BSP_LCD_RST_PIN, 0);
    sleep_ms(100);
    gpio_put(BSP_LCD_RST_PIN, 1);
    //gpio_put(BSP_LCD_BL_PIN, 1);
    sleep_ms(100);
}

static void bsp_st77916_init(void)
{
    bsp_st77916_gpio_init();
    bsp_lcd_brightness_init();

    bsp_lcd_reset();
    pio_qspi_init(BSP_LCD_SCLK_PIN, BSP_LCD_D0_PIN, 80 * 1000 * 1000, bsp_st77916_flush_dma_done_cb);
    sleep_ms(100);
    bsp_st77916_cmd_t st77916_init_cmds[] = {
        /*{cmd, { data }, data_size, delay_ms}*/
        {0xF0, (uint8_t []){0x28}, 1, 0},
        {0xF2, (uint8_t []){0x28}, 1, 0},
        {0x73, (uint8_t []){0xF0}, 1, 0},
        {0x7C, (uint8_t []){0xD1}, 1, 0},
        {0x83, (uint8_t []){0xE0}, 1, 0},
        {0x84, (uint8_t []){0x61}, 1, 0},
        {0xF2, (uint8_t []){0x82}, 1, 0},
        {0xF0, (uint8_t []){0x00}, 1, 0},
        {0xF0, (uint8_t []){0x01}, 1, 0},
        {0xF1, (uint8_t []){0x01}, 1, 0},
        {0xB0, (uint8_t []){0x56}, 1, 0},
        {0xB1, (uint8_t []){0x4D}, 1, 0},
        {0xB2, (uint8_t []){0x24}, 1, 0},
        {0xB4, (uint8_t []){0x87}, 1, 0},
        {0xB5, (uint8_t []){0x44}, 1, 0},
        {0xB6, (uint8_t []){0x8B}, 1, 0},
        {0xB7, (uint8_t []){0x40}, 1, 0},
        {0xB8, (uint8_t []){0x86}, 1, 0},
        {0xBA, (uint8_t []){0x00}, 1, 0},
        {0xBB, (uint8_t []){0x08}, 1, 0},
        {0xBC, (uint8_t []){0x08}, 1, 0},
        {0xBD, (uint8_t []){0x00}, 1, 0},
        {0xC0, (uint8_t []){0x80}, 1, 0},
        {0xC1, (uint8_t []){0x10}, 1, 0},
        {0xC2, (uint8_t []){0x37}, 1, 0},
        {0xC3, (uint8_t []){0x80}, 1, 0},
        {0xC4, (uint8_t []){0x10}, 1, 0},
        {0xC5, (uint8_t []){0x37}, 1, 0},
        {0xC6, (uint8_t []){0xA9}, 1, 0},
        {0xC7, (uint8_t []){0x41}, 1, 0},
        {0xC8, (uint8_t []){0x01}, 1, 0},
        {0xC9, (uint8_t []){0xA9}, 1, 0},
        {0xCA, (uint8_t []){0x41}, 1, 0},
        {0xCB, (uint8_t []){0x01}, 1, 0},
        {0xD0, (uint8_t []){0x91}, 1, 0},
        {0xD1, (uint8_t []){0x68}, 1, 0},
        {0xD2, (uint8_t []){0x68}, 1, 0},
        {0xF5, (uint8_t []){0x00, 0xA5}, 2, 0},
        {0xDD, (uint8_t []){0x4F}, 1, 0},
        {0xDE, (uint8_t []){0x4F}, 1, 0},
        {0xF1, (uint8_t []){0x10}, 1, 0},
        {0xF0, (uint8_t []){0x00}, 1, 0},
        {0xF0, (uint8_t []){0x02}, 1, 0},
        {0xE0, (uint8_t []){0xF0, 0x0A, 0x10, 0x09, 0x09, 0x36, 0x35, 0x33, 0x4A, 0x29, 0x15, 0x15, 0x2E, 0x34}, 14, 0},
        {0xE1, (uint8_t []){0xF0, 0x0A, 0x0F, 0x08, 0x08, 0x05, 0x34, 0x33, 0x4A, 0x39, 0x15, 0x15, 0x2D, 0x33}, 14, 0},
        {0xF0, (uint8_t []){0x10}, 1, 0},
        {0xF3, (uint8_t []){0x10}, 1, 0},
        {0xE0, (uint8_t []){0x07}, 1, 0},
        {0xE1, (uint8_t []){0x00}, 1, 0},
        {0xE2, (uint8_t []){0x00}, 1, 0},
        {0xE3, (uint8_t []){0x00}, 1, 0},
        {0xE4, (uint8_t []){0xE0}, 1, 0},
        {0xE5, (uint8_t []){0x06}, 1, 0},
        {0xE6, (uint8_t []){0x21}, 1, 0},
        {0xE7, (uint8_t []){0x01}, 1, 0},
        {0xE8, (uint8_t []){0x05}, 1, 0},
        {0xE9, (uint8_t []){0x02}, 1, 0},
        {0xEA, (uint8_t []){0xDA}, 1, 0},
        {0xEB, (uint8_t []){0x00}, 1, 0},
        {0xEC, (uint8_t []){0x00}, 1, 0},
        {0xED, (uint8_t []){0x0F}, 1, 0},
        {0xEE, (uint8_t []){0x00}, 1, 0},
        {0xEF, (uint8_t []){0x00}, 1, 0},
        {0xF8, (uint8_t []){0x00}, 1, 0},
        {0xF9, (uint8_t []){0x00}, 1, 0},
        {0xFA, (uint8_t []){0x00}, 1, 0},
        {0xFB, (uint8_t []){0x00}, 1, 0},
        {0xFC, (uint8_t []){0x00}, 1, 0},
        {0xFD, (uint8_t []){0x00}, 1, 0},
        {0xFE, (uint8_t []){0x00}, 1, 0},
        {0xFF, (uint8_t []){0x00}, 1, 0},
        {0x60, (uint8_t []){0x40}, 1, 0},
        {0x61, (uint8_t []){0x04}, 1, 0},
        {0x62, (uint8_t []){0x00}, 1, 0},
        {0x63, (uint8_t []){0x42}, 1, 0},
        {0x64, (uint8_t []){0xD9}, 1, 0},
        {0x65, (uint8_t []){0x00}, 1, 0},
        {0x66, (uint8_t []){0x00}, 1, 0},
        {0x67, (uint8_t []){0x00}, 1, 0},
        {0x68, (uint8_t []){0x00}, 1, 0},
        {0x69, (uint8_t []){0x00}, 1, 0},
        {0x6A, (uint8_t []){0x00}, 1, 0},
        {0x6B, (uint8_t []){0x00}, 1, 0},
        {0x70, (uint8_t []){0x40}, 1, 0},
        {0x71, (uint8_t []){0x03}, 1, 0},
        {0x72, (uint8_t []){0x00}, 1, 0},
        {0x73, (uint8_t []){0x42}, 1, 0},
        {0x74, (uint8_t []){0xD8}, 1, 0},
        {0x75, (uint8_t []){0x00}, 1, 0},
        {0x76, (uint8_t []){0x00}, 1, 0},
        {0x77, (uint8_t []){0x00}, 1, 0},
        {0x78, (uint8_t []){0x00}, 1, 0},
        {0x79, (uint8_t []){0x00}, 1, 0},
        {0x7A, (uint8_t []){0x00}, 1, 0},
        {0x7B, (uint8_t []){0x00}, 1, 0},
        {0x80, (uint8_t []){0x48}, 1, 0},
        {0x81, (uint8_t []){0x00}, 1, 0},
        {0x82, (uint8_t []){0x06}, 1, 0},
        {0x83, (uint8_t []){0x02}, 1, 0},
        {0x84, (uint8_t []){0xD6}, 1, 0},
        {0x85, (uint8_t []){0x04}, 1, 0},
        {0x86, (uint8_t []){0x00}, 1, 0},
        {0x87, (uint8_t []){0x00}, 1, 0},
        {0x88, (uint8_t []){0x48}, 1, 0},
        {0x89, (uint8_t []){0x00}, 1, 0},
        {0x8A, (uint8_t []){0x08}, 1, 0},
        {0x8B, (uint8_t []){0x02}, 1, 0},
        {0x8C, (uint8_t []){0xD8}, 1, 0},
        {0x8D, (uint8_t []){0x04}, 1, 0},
        {0x8E, (uint8_t []){0x00}, 1, 0},
        {0x8F, (uint8_t []){0x00}, 1, 0},
        {0x90, (uint8_t []){0x48}, 1, 0},
        {0x91, (uint8_t []){0x00}, 1, 0},
        {0x92, (uint8_t []){0x0A}, 1, 0},
        {0x93, (uint8_t []){0x02}, 1, 0},
        {0x94, (uint8_t []){0xDA}, 1, 0},
        {0x95, (uint8_t []){0x04}, 1, 0},
        {0x96, (uint8_t []){0x00}, 1, 0},
        {0x97, (uint8_t []){0x00}, 1, 0},
        {0x98, (uint8_t []){0x48}, 1, 0},
        {0x99, (uint8_t []){0x00}, 1, 0},
        {0x9A, (uint8_t []){0x0C}, 1, 0},
        {0x9B, (uint8_t []){0x02}, 1, 0},
        {0x9C, (uint8_t []){0xDC}, 1, 0},
        {0x9D, (uint8_t []){0x04}, 1, 0},
        {0x9E, (uint8_t []){0x00}, 1, 0},
        {0x9F, (uint8_t []){0x00}, 1, 0},
        {0xA0, (uint8_t []){0x48}, 1, 0},
        {0xA1, (uint8_t []){0x00}, 1, 0},
        {0xA2, (uint8_t []){0x05}, 1, 0},
        {0xA3, (uint8_t []){0x02}, 1, 0},
        {0xA4, (uint8_t []){0xD5}, 1, 0},
        {0xA5, (uint8_t []){0x04}, 1, 0},
        {0xA6, (uint8_t []){0x00}, 1, 0},
        {0xA7, (uint8_t []){0x00}, 1, 0},
        {0xA8, (uint8_t []){0x48}, 1, 0},
        {0xA9, (uint8_t []){0x00}, 1, 0},
        {0xAA, (uint8_t []){0x07}, 1, 0},
        {0xAB, (uint8_t []){0x02}, 1, 0},
        {0xAC, (uint8_t []){0xD7}, 1, 0},
        {0xAD, (uint8_t []){0x04}, 1, 0},
        {0xAE, (uint8_t []){0x00}, 1, 0},
        {0xAF, (uint8_t []){0x00}, 1, 0},
        {0xB0, (uint8_t []){0x48}, 1, 0},
        {0xB1, (uint8_t []){0x00}, 1, 0},
        {0xB2, (uint8_t []){0x09}, 1, 0},
        {0xB3, (uint8_t []){0x02}, 1, 0},
        {0xB4, (uint8_t []){0xD9}, 1, 0},
        {0xB5, (uint8_t []){0x04}, 1, 0},
        {0xB6, (uint8_t []){0x00}, 1, 0},
        {0xB7, (uint8_t []){0x00}, 1, 0},
        
        {0xB8, (uint8_t []){0x48}, 1, 0},
        {0xB9, (uint8_t []){0x00}, 1, 0},
        {0xBA, (uint8_t []){0x0B}, 1, 0},
        {0xBB, (uint8_t []){0x02}, 1, 0},
        {0xBC, (uint8_t []){0xDB}, 1, 0},
        {0xBD, (uint8_t []){0x04}, 1, 0},
        {0xBE, (uint8_t []){0x00}, 1, 0},
        {0xBF, (uint8_t []){0x00}, 1, 0},
        {0xC0, (uint8_t []){0x10}, 1, 0},
        {0xC1, (uint8_t []){0x47}, 1, 0},
        {0xC2, (uint8_t []){0x56}, 1, 0},
        {0xC3, (uint8_t []){0x65}, 1, 0},
        {0xC4, (uint8_t []){0x74}, 1, 0},
        {0xC5, (uint8_t []){0x88}, 1, 0},
        {0xC6, (uint8_t []){0x99}, 1, 0},
        {0xC7, (uint8_t []){0x01}, 1, 0},
        {0xC8, (uint8_t []){0xBB}, 1, 0},
        {0xC9, (uint8_t []){0xAA}, 1, 0},
        {0xD0, (uint8_t []){0x10}, 1, 0},
        {0xD1, (uint8_t []){0x47}, 1, 0},
        {0xD2, (uint8_t []){0x56}, 1, 0},
        {0xD3, (uint8_t []){0x65}, 1, 0},
        {0xD4, (uint8_t []){0x74}, 1, 0},
        {0xD5, (uint8_t []){0x88}, 1, 0},
        {0xD6, (uint8_t []){0x99}, 1, 0},
        {0xD7, (uint8_t []){0x01}, 1, 0},
        {0xD8, (uint8_t []){0xBB}, 1, 0},
        {0xD9, (uint8_t []){0xAA}, 1, 0},
        {0xF3, (uint8_t []){0x01}, 1, 0},

        {0xF0, (uint8_t[]){0x00}, 1, 0},
        {0x4D, (uint8_t[]){0x00}, 1, 0},
        {0x4E, (uint8_t[]){0x00}, 1, 0},
        {0x4F, (uint8_t[]){0x00}, 1, 0},
        {0x4C, (uint8_t[]){0x01}, 1, 10},
        {0x4C, (uint8_t[]){0x00}, 1, 10},
        {0x3a, (uint8_t[]){0x55}, 1, 0},

        {0xF0, (uint8_t[]){0x00}, 1, 0},
        // {0x35, (uint8_t[]){0x01}, 1, 0},
        {0x44, (uint8_t[]){0x67, 0x01}, 1, 0},
        {0x2A, (uint8_t[]){0x00, 0x00, 0x01, 0x67}, 4, 0},
        {0x2B, (uint8_t[]){0x00, 0x00, 0x01, 0x67}, 4, 0},
        {0x21, (uint8_t[]){0x00}, 0, 120},
        {0x11, (uint8_t[]){0x00}, 0, 120},
        {0x29, (uint8_t[]){0x00}, 0, 120},
    };

    bsp_st77916_tx_param(st77916_init_cmds, sizeof(st77916_init_cmds) / sizeof(bsp_st77916_cmd_t));
    bsp_lcd_set_brightness(g_display_info->brightness);
    bsp_st77916_set_rotation(g_display_info->rotation);
}

bool bsp_display_new_st77916(bsp_display_interface_t **interface, bsp_display_info_t *info)
{
    if (info == NULL)
        return false;

    static bsp_display_interface_t display_if;
    static bsp_display_info_t display_info;

    memcpy(&display_info, info, sizeof(bsp_display_info_t));

    display_if.init = bsp_st77916_init;
    display_if.reset = bsp_lcd_reset;
    display_if.set_rotation = bsp_st77916_set_rotation;
    display_if.set_brightness = bsp_lcd_set_brightness;
    display_if.set_window = bsp_st77916_set_window;
    display_if.get_brightness = bsp_lcd_get_brightness;
    display_if.get_rotation = bsp_st77916_get_rotation;
    display_if.flush = bsp_st77916_flush;
    display_if.flush_dma = bsp_st77916_flush_dma;

    *interface = &display_if;
    g_display_if = &display_if;
    g_display_info = &display_info;
    return true;
}
