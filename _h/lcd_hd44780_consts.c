//
const uint8_t LCD_CMD_CLEAR             = 0b00000001;

const uint8_t LCD_CMD_HOME              = 0b00000010;

const uint8_t LCD_CMD_SET_ENTRY_MODE    = 0b00000100;
const uint8_t LCD_CMD_EM_SHIFT_CURSOR   = 0;
const uint8_t LCD_CMD_EM_SHIFT_DISPLAY	= 0b00000001;
const uint8_t LCD_CMD_EM_DECREMENT		= 0;
const uint8_t LCD_CMD_EM_INCREMENT		= 0b00000010;

const uint8_t LCD_CMD_SET_DISPLAY       = 0b00001000;
const uint8_t LCD_CMD_DISPLAY_OFF		= 0;
const uint8_t LCD_CMD_DISPLAY_ON		= 0b00000100;
const uint8_t LCD_CMD_CURSOR_OFF		= 0;
const uint8_t LCD_CMD_CURSOR_ON			= 0b00000010;
const uint8_t LCD_CMD_CURSOR_BLINK_OFF	= 0;
const uint8_t LCD_CMD_CURSOR_BLINK_ON	= 0b00000001;

const uint8_t LCD_CMD_SHIFT             = 0b00010000;
const uint8_t LCD_CMD_SHIFT_CURSOR		= 0;
const uint8_t LCD_CMD_SHIFT_DISPLAY		= 0b00001000;
const uint8_t LCD_CMD_SHIFT_LEFT	    = 0;
const uint8_t LCD_CMD_SHIFT_RIGHT       = 0b00000100;

const uint8_t LCD_CMD_SET_FUNCTION		= 0b00100000;
const uint8_t LCD_CMD_FONT_REGULAR		= 0;
const uint8_t LCD_CMD_FONT_LARGE		= 0b00000100;
const uint8_t LCD_CMD_1_LINE			= 0;
const uint8_t LCD_CMD_2_LINES			= 0b00001000;
const uint8_t LCD_CMD_4_BIT				= 0;
const uint8_t LCD_CMD_8_BIT				= 0b00010000;

const uint8_t LCD_CMD_SET_CGRAM			= 0b01000000;

const uint8_t LCD_CMD_SET_DDRAM			= 0b10000000;

const uint8_t LCD_BUSY_BIT			    = (1<< 7);

const uint8_t LCD_LINE_1_OFFSET         = 0x00;
const uint8_t LCD_LINE_2_OFFSET         = 0x40;
const uint8_t LCD_LINE_3_OFFSET         = 0x14;
const uint8_t LCD_LINE_4_OFFSET         = 0x54;
