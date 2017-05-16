#ifndef BUILD_LK
    #include <linux/string.h>
#endif

#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)
#define REGFLAG_DELAY             							0xAB
#define REGFLAG_END_OF_TABLE      							0xAA   // END OF REGISTERS MARKER

#define LCM_ID_RM69052 (0x90)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    

int setbacklight_level = 0; 

struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
    { 0x28, 0x01, {0x00}},
    { REGFLAG_DELAY, 0x00, {0x00}},
    { 0x28, 0x01, {0x00}},
    { 0x10, 0x01, {0x00}},
    { REGFLAG_DELAY, 0x00, {0x00}},
    { REGFLAG_END_OF_TABLE, 0x00, {0x00}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
  unsigned int i;
  for(i = 0; i < count; i++) {
    unsigned cmd;
    cmd = table[i].cmd;
    switch (cmd) {
      case REGFLAG_DELAY :
        MDELAY(table[i].count);
        break;
      case REGFLAG_END_OF_TABLE :
        break;
      default:
        dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
    }
  }
};

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
  memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
};

static void lcm_get_params(LCM_PARAMS *params)
{
  memset(params, 0, sizeof(LCM_PARAMS));
  params->type = 2;
  params->dbi.te_edge_polarity = 0;
  params->dsi.data_format.color_order = 0;
  params->dsi.data_format.trans_seq = 0;
  params->dsi.data_format.padding = 0;
  params->dsi.data_format.format = 2;
  params->dsi.intermediat_buffer_num = 2;
  params->dsi.PS = 2;
  params->dsi.pll_div1 = 0;
  params->dsi.packet_size = 256;
  params->width = 720;
  params->height = 1280;
  params->dbi.te_mode = 1;
  params->dsi.mode = 1;
  params->dsi.LANE_NUM = 4;
  params->dsi.vertical_sync_active = 5;
  params->dsi.vertical_backporch = 11;
  params->dsi.vertical_frontporch = 16;
  params->dsi.vertical_active_line = 1280;
  params->dsi.horizontal_sync_active = 5;
  params->dsi.horizontal_backporch = 11;
  params->dsi.horizontal_frontporch = 16;
  params->dsi.horizontal_blanking_pixel = 60;
  params->dsi.horizontal_active_pixel = 720;
  params->dsi.pll_div2 = 1;
  params->dsi.fbk_div = 17;
};

static void lcm_init(void)
{
  int data_array[16];
  
  SET_RESET_PIN(1);
  SET_RESET_PIN(0);
  MDELAY(20);
  SET_RESET_PIN(1);
  MDELAY(50);
  data_array[0] = 0x62902;
  data_array[1] = 0x52AA55F0;
  data_array[2] = 0x8;
  dsi_set_cmdq(data_array, 3, 1);
  data_array[0] = 0x42902;
  data_array[1] = 0x101000B0;
  dsi_set_cmdq(data_array, 2, 1);
  data_array[0] = 0x60BA2300;
  dsi_set_cmdq(data_array, 1, 1);
  data_array[0] = 0x82902;
  data_array[1] = 0x777777BB;
  data_array[2] = 0x77777777;
  dsi_set_cmdq(data_array, 3, 1);
  data_array[2] = 0x208;
  data_array[0] = 0x62902;
  data_array[1] = 0x52AA55F0;
  dsi_set_cmdq(data_array, 3, 1);
  data_array[0] = 0x4CA2300;
  dsi_set_cmdq(data_array, 1, 1);
  data_array[0] = 0xE12300;
  dsi_set_cmdq(data_array, 1, 1);
  data_array[0] = 0xAE22300;
  dsi_set_cmdq(data_array, 1, 1);
  data_array[0] = 0x40E32300;
  dsi_set_cmdq(data_array, 1, 1);
  data_array[0] = 0x62902;
  data_array[1] = 0xE7;
  data_array[2] = 0x0;
  dsi_set_cmdq(data_array, 3, 1);
  data_array[3] = 0x8;
  data_array[0] = 0x92902;
  data_array[1] = 0xE00048ED;
  data_array[2] = 0x91000813;
  dsi_set_cmdq(data_array, 4, 1);
  data_array[2] = 0x10000;
  data_array[0] = 0x72902;
  data_array[1] = 0x1C0800FD;
  dsi_set_cmdq(data_array, 3, 1);
  data_array[0] = 0xC3902;
  data_array[1] = 0x42411C3;
  data_array[2] = 0x4020A;
  data_array[3] = 0xF0101C;
  dsi_set_cmdq(data_array, 4, 1);
  data_array[0] = 0x62902;
  data_array[1] = 0x52AA55F0;
  data_array[2] = 0x308;
  dsi_set_cmdq(data_array, 3, 1);
  data_array[0] = 0x72902;
  data_array[2] = 0x10000;
  data_array[1] = 0xF1;
  dsi_set_cmdq(data_array, 3, 1);
  data_array[0] = 0xE02300;
  dsi_set_cmdq(data_array, 1, 1);
  data_array[0] = 0x8F62300;
  dsi_set_cmdq(data_array, 1, 1);
  data_array[0] = 0x62902;
  data_array[1] = 0x52AA55F0;
  data_array[2] = 0x508;
  dsi_set_cmdq(data_array, 3, 1);
  data_array[0] = 0x32902;
  data_array[1] = 0x1400C4;
  dsi_set_cmdq(data_array, 2, 1);
  data_array[0] = 0x4C92300;
  dsi_set_cmdq(data_array, 1, 1);
  data_array[0] = 0x62902;
  data_array[1] = 0x52AA55F0;
  data_array[2] = 0x108;
  dsi_set_cmdq(data_array, 3, 1);
  data_array[0] = 0x42902;
  data_array[1] = 0x60606B0;
  dsi_set_cmdq(data_array, 2, 1);
  data_array[0] = 0x42902;
  data_array[1] = 0x141414B1;
  dsi_set_cmdq(data_array, 2, 1);
  data_array[1] = 0xB2;
  data_array[0] = 0x42902;
  dsi_set_cmdq(data_array, 2, 1);
  data_array[0] = 0x42902;
  data_array[1] = 0x666666B4;
  dsi_set_cmdq(data_array, 2, 1);
  data_array[0] = 0x42902;
  data_array[1] = 0x444444B5;
  dsi_set_cmdq(data_array, 2, 1);
  data_array[0] = 0x42902;
  data_array[1] = 0x545454B6;
  dsi_set_cmdq(data_array, 2, 1);
  data_array[0] = 0x42902;
  data_array[1] = 0x242424B7;
  dsi_set_cmdq(data_array, 2, 1);
  data_array[0] = 0x42902;
  data_array[1] = 0x40404B9;
  dsi_set_cmdq(data_array, 2, 1);
  data_array[0] = 0x42902;
  data_array[1] = 0x141414BA;
  dsi_set_cmdq(data_array, 2, 1);
  data_array[0] = 0x42902;
  data_array[1] = 0x783822BE;
  dsi_set_cmdq(data_array, 2, 1);
  data_array[0] = 0x62902;
  data_array[1] = 0x52AA55F0;
  data_array[2] = 0x208;
  dsi_set_cmdq(data_array, 3, 1);
  data_array[0] = 0xE12300;
  dsi_set_cmdq(data_array, 1, 1);
  MDELAY(5);
  data_array[0] = 0x22902;
  data_array[1] = (setbacklight_level << 8) | 0x51;
  dsi_set_cmdq(data_array, 2, 1);
  data_array[0] = 0x352300;
  dsi_set_cmdq(data_array, 1, 1);
  data_array[0] = 0x110500;
  dsi_set_cmdq(data_array, 1, 1);
  MDELAY(150);
  data_array[0] = 0x290500;
  dsi_set_cmdq(data_array, 1, 1);
};

static void lcm_suspend(void)
{
  push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
};

static void lcm_resume(void)
{
  printk("lcm_resume  \n");
  lcm_init();
};

static unsigned int lcm_compare_id(void)
{
  return 1;
};

static void lcm_setbacklight(unsigned int level)
{
  int data_array[4];
  
  setbacklight_level = level;
  data_array[0] = 0x22902;
  data_array[1] = (setbacklight_level << 8) | 0x51;
  dsi_set_cmdq(data_array, 2, 1);
  MDELAY(1);
}



LCM_DRIVER rm69052_oled_lcm_drv = 
{
    .name			= "rm69052_oled",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id    = lcm_compare_id,
	.set_backlight  = lcm_setbacklight,
};
