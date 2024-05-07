#include <stdio.h>

#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include "pico/time.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "tusb.h"
#include "usb_descriptors.h"

#define ADDR 0x52 
const uint BLUE_LED_PIN = 20;
const uint GREEN_LED_PIN = 19;
const uint RED_LED_PIN = 18;

void hid_task(void);
void send_hid_report(uint8_t report_id);

typedef struct {
    int ax;
    int ay;
    int az;
    int z;
    int c;
    int jx;
    int jy;
} NunchuckState;

NunchuckState ns = {0};

#define DEAD_ZONE_SIZE 6
#define JOY_X_CENTER 122
#define JOY_Y_CENTER 128

//Accel value for "up"
#define ACCEL_CENTER 512
#define ACCEL_MIN 125

#define MOUSE_EXPONENT 2.0f
#define SCROLL_EXPONENT 1.5f

int main() {
    stdio_init_all();
    tusb_init();

       
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);
    
    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);

    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);

    // I2C is "open drain", pull ups to keep signal high when no data is being sent
    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);

	//
    uint8_t startup_sequence1[] = {0xF0,0x55};
    // uint8_t startup_sequence1[] = {0x55,0xF0};
    uint8_t startup_sequence2[] = {0xFB,0x00};
    // uint8_t startup_sequence2[] = {0x00,0xFB};
    uint8_t startup_sequence3[] = {0x40,0x00};

    uint8_t buf[6];
    i2c_write_blocking(i2c1, ADDR, startup_sequence1, 2, false);  // true to keep master control of bus
    sleep_ms(100);
    i2c_write_blocking(i2c1, ADDR, startup_sequence2, 2, false);  // true to keep master control of bus
    // i2c_write_blocking(i2c1, ADDR, startup_sequence3, 2, false);  // true to keep master control of bus
    uint16_t count = 0;
    uint8_t blink = 0;
    while (1) {
	tud_task();

	uint8_t read_cmd[] = {0x00};
	int w1 = i2c_write_blocking(i2c1, ADDR, read_cmd, 1, false);
	sleep_ms(2);
    	int w2 = i2c_read_blocking(i2c1, ADDR, buf, 6, false);  // false, we're done reading
	// count++;
	if (count > 50) {
		printf("ax: %d ay: %d az: %d jx: %d jy: %d z: %d c: %d\n",ns.ax,ns.ay,ns.az,ns.jx,ns.jy,ns.z,ns.c);
		count = 0;
	}
	ns.ax = ((buf[5] & 0xC0) >> 6) | (buf[2] << 2);
	ns.ay = ((buf[5] & 0x30) >> 4) | (buf[3] << 2);
	ns.az = ((buf[5] & 0x0C) >> 2) | (buf[4] << 2);
	ns.jx = buf[0];
	ns.jy = buf[1];
	ns.z = buf[5] & 0x01;
	ns.c = (buf[5] & 0x02)>>1;

	sleep_ms(6);
	hid_task();
    }
}

int8_t apply_exponential_curve(int joystick_value, float exponent) {

    if (abs(joystick_value) < DEAD_ZONE_SIZE){
      joystick_value = 0;
    }

	// Normalize joystick value from -1024 to 1024 to -1 to 1
    float normalized_input = joystick_value / 128.0f;

    // Apply exponential curve
    float output = pow(fabs(normalized_input), exponent);

    // Preserve the sign of the input
    output *= (normalized_input < 0) ? -1.0f : 1.0f;

    // Scale output to max range (e.g., -127 to 127)
    output *= 127.0f;

    return (int8_t)output;
}


// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) len;

  uint8_t next_report_id = report[0] + 1;

  if (next_report_id < REPORT_ID_COUNT)
  {
    send_hid_report(next_report_id);
  }
}


// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;

  // if (report_type == HID_REPORT_TYPE_OUTPUT)
  // {
  //   // Set keyboard LED e.g Capslock, Numlock etc...
  //   if (report_id == REPORT_ID_KEYBOARD)
  //   {
  //     // bufsize should be (at least) 1
  //     if ( bufsize < 1 ) return;
  //
  //     uint8_t const kbd_leds = buffer[0];
  //
  //     if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
  //     {
  //       // Capslock On: disable blink, turn led on
  //       blink_interval_ms = 0;
  //       // board_led_write(true);
  //     }else
  //     {
  //       // Caplocks Off: back to normal blink
  //       // board_led_write(false);
  //       // blink_interval_ms = BLINK_MOUNTED;
  //     }
  //   }
  // }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

void send_hid_report(uint8_t report_id)
{
  // skip if hid is not ready yet
  if ( !tud_hid_ready() ) return;

  switch(report_id)
  {
    case REPORT_ID_KEYBOARD:
    {
	tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);

      // // use to avoid send multiple consecutive zero report for keyboard
      // static bool has_keyboard_key = false;
      //
      // if ( btn )
      // {
      //   uint8_t keycode[6] = { 0 };
      //   keycode[0] = HID_KEY_A;
      //
      //   tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
      //   has_keyboard_key = true;
      // }else
      // {
      //   // send empty key report if previously has key pressed
      //   if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
      //   has_keyboard_key = false;
      // }
    }
    break;

    case REPORT_ID_MOUSE:
    {
      int8_t scroll, mouse_x, mouse_y = 0;
      uint8_t buttons = (!(ns.c & 0x01 )) | (!(ns.z & 0x01) << 1);
       
      if (abs(ns.ax - ACCEL_CENTER) > ACCEL_MIN){
	gpio_put(GREEN_LED_PIN,1);
	scroll = apply_exponential_curve(((int8_t) ( (int16_t)ns.jy - 128)),SCROLL_EXPONENT); 
      }
      else{
  	  mouse_x = ((int8_t) ( (int16_t)ns.jx - 122));
          mouse_y = -((int8_t) ( (int16_t)ns.jy - 128));
          gpio_put(GREEN_LED_PIN,0);
	  mouse_x = apply_exponential_curve(mouse_x,MOUSE_EXPONENT);
	  mouse_y = apply_exponential_curve(mouse_y,MOUSE_EXPONENT);      
     }


 // no button, right + down, no scroll, no pan
      tud_hid_mouse_report(REPORT_ID_MOUSE, buttons, mouse_x, mouse_y, scroll, 0);
    }
    break;

    // case REPORT_ID_CONSUMER_CONTROL:
    // {
    //   // use to avoid send multiple consecutive zero report
    //   static bool has_consumer_key = false;
    //
    //   if ( btn )
    //   {
    //     // volume down
    //     uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
    //     tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_down, 2);
    //     has_consumer_key = true;
    //   }else
    //   {
    //     // send empty key report (release key) if previously has key pressed
    //     uint16_t empty_key = 0;
    //     if (has_consumer_key) tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
    //     has_consumer_key = false;
    //   }
    // }
    // break;
    //
    // case REPORT_ID_GAMEPAD:
    // {
    //   // use to avoid send multiple consecutive zero report for keyboard
    //   static bool has_gamepad_key = false;
    //
    //   hid_gamepad_report_t report =
    //   {
    //     .x   = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0,
    //     .hat = 0, .buttons = 0
    //   };
    //
    //   if ( btn )
    //   {
    //     report.hat = GAMEPAD_HAT_UP;
    //     report.buttons = GAMEPAD_BUTTON_A;
    //     tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
    //
    //     has_gamepad_key = true;
    //   }else
    //   {
    //     report.hat = GAMEPAD_HAT_CENTERED;
    //     report.buttons = 0;
    //     if (has_gamepad_key) tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
    //     has_gamepad_key = false;
    //   }
    // }
    // break;

    default: break;
  }
}


// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;
  uint32_t milis = to_ms_since_boot(get_absolute_time());
  if ( milis - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  // Remote wakeup
  if ( tud_suspended() )
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  }else
  {
    // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
    send_hid_report(REPORT_ID_KEYBOARD);
  }
}
