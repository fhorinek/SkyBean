#include "bui.h"

#define BUTTON_IDLE		0
#define BUTTON_PRESSED	1
#define BUTTON_WAITING	2

#define BUTTON_DEBOUNCE		10
#define BUTTON_LONG			500
#define BUTTON_EXTRA_LONG	2000

uint32_t button_pressed_start = 0;
uint16_t button_delta = 0;
uint8_t button_state = BUTTON_IDLE;

uint8_t beep_blik = 0;

#define BEEP_BLIK_NONE	0b00000000
#define BEEP_BLIK_RED	0b00000001
#define BEEP_BLIK_GREEN	0b00000010
#define BEEP_BLIK_BOTH	0b00000011

void bui_init()
{
	GpioSetDirection(BUTTON_PIN, INPUT);
	GpioSetPull(BUTTON_PIN, gpio_pull_up);

	GpioSetDirection(LEDR, OUTPUT);
	GpioSetDirection(LEDG, OUTPUT);
	LEDR_OFF
	LEDG_OFF
}

bool pressed_from_begining = true;

void button_task()
{
	//pressed
	if (BUTTON_DOWN)
	{
		if (button_state == BUTTON_IDLE)
		{
			button_pressed_start = sys_tick_get();
			button_delta = 0;
			button_state = BUTTON_PRESSED;
		}
		else
			button_delta = sys_tick_get() - button_pressed_start;
	}
	else
	//relased
	{
		if (button_state == BUTTON_PRESSED || button_state == BUTTON_WAITING)
			button_state = BUTTON_IDLE;

		pressed_from_begining = false;
	}
}

bool bui_was_short()
{
	if (button_state == BUTTON_IDLE)
		return (button_delta > BUTTON_DEBOUNCE && button_delta < BUTTON_LONG);
	else
		return false;
}

bool bui_is_short()
{
	if (button_state == BUTTON_PRESSED)
		return (button_delta > BUTTON_DEBOUNCE && button_delta < BUTTON_LONG);
	else
		return false;
}

bool bui_was_long()
{
	if (button_state == BUTTON_IDLE)
		return (button_delta > BUTTON_LONG);
	else
		return false;
}

bool bui_is_long()
{
	if (button_state == BUTTON_PRESSED)
		return (button_delta > BUTTON_LONG);
	else
		return false;
}


bool bui_is_extra_long()
{
	if (button_state == BUTTON_PRESSED)
		return (button_delta > BUTTON_EXTRA_LONG);
	else
		return false;
}

void bui_button_clear()
{
	button_delta = 0;
	button_state = BUTTON_WAITING;
}

#define BUI_MODE_IDLE		0
#define BUI_MODE_SETTINGS	1
#define BUI_MODE_LIFT		2
#define BUI_MODE_SINK		3
#define BUI_MODE_UNLOCK		4
#define BUI_MODE_PROFILE	5

#define BUI_SHORT_WAIT		300
#define BUI_WAIT			3000
#define BUI_LONG_WAIT		6000

uint8_t bui_mode = BUI_MODE_IDLE;

extern uint8_t sleep_now;

ISR(portc_interrupt)
{
	//Dummy button IRQ just wake the device
}

uint8_t beep_count;

void volume_toggle()
{
	cfg.buzzer_volume++;
	if (cfg.buzzer_volume > 4)
		cfg.buzzer_volume = 1;

	beep_blik = BEEP_BLIK_BOTH;
	beep_count = cfg.buzzer_volume;
	start_sound(SOUND_BEEP);

	StoreVolume();
}


extern uint8_t buzzer_override;
extern uint16_t buzzer_override_tone;

uint8_t sound_type = SOUND_NONE;
uint16_t sound_duration = 0; //1 = 20ms

#define if_in_range(from, to, freq)	((sound_duration >= from && sound_duration < to) ? freq : buzzer_override_tone)

#define BEEP_DURATION 10

void sound_task()
{
	if (sound_duration == 0)
		return;

	switch (sound_type)
	{
	case (SOUND_OFF):
		//power off sequence
		buzzer_override_tone = 0;
		buzzer_override_tone = if_in_range(0, 10, 300);
		buzzer_override_tone = if_in_range(20, 30, 600);
		buzzer_override_tone = if_in_range(40, 50, 900);

		if (buzzer_override_tone)
			LEDR_ON
		else
			LEDR_OFF

		if (sound_duration == 1)
		{
			sleep_now = true;
			LEDR_OFF;
		}
	break;

	case (SOUND_ADC):
		//warn user that battery is dying
		buzzer_override_tone = 1000;

		if (sound_duration == 1)
		{
			LEDR_OFF;
		}

//		if (sound_duration == 1)
//		{
//			sleep_now = true;
//		}
	break;

	case (SOUND_TIM):
		//distinguish between auto power off and power off (not used right now)
		buzzer_override_tone = 50;

		if (sound_duration == 1)
		{
			sleep_now = true;
		}
	break;

	case (SOUND_ON):
		//power on sequence

		buzzer_override_tone = 0;

		if (cfg.supress_startup)
		{
			buzzer_override_tone = if_in_range(450, 460, 900);
			buzzer_override_tone = if_in_range(470, 480, 600);
			buzzer_override_tone = if_in_range(490, 500, 300);
		}
		else
		{
			buzzer_override_tone = if_in_range(10, 20, 900);
			buzzer_override_tone = if_in_range(30, 40, 600);
			buzzer_override_tone = if_in_range(50, 60, 300);
		}

		if ((sound_duration / 10) % 2 == 1)
			LEDG_ON
		else
			LEDG_OFF

	break;

	case (SOUND_LIFT):
		//enter lift configuration menu
		buzzer_override_tone = 500 - sound_duration;
	break;

	case (SOUND_SINK):
		//enter sink configuration menu
		buzzer_override_tone = 400 + sound_duration;
	break;

	case (SOUND_BEEP):
		//beep feedback
		if (sound_duration % (BEEP_DURATION * 2) <= BEEP_DURATION)
		{
			buzzer_override_tone = 0;
			LEDR_OFF;
			LEDG_OFF;
		}
		else
		{
			buzzer_override_tone = 700;

			if (beep_blik & BEEP_BLIK_RED)
				LEDR_ON;
			if (beep_blik & BEEP_BLIK_GREEN)
				LEDG_ON;
		}
	break;
	}

//	printf("S %d, %d\n", sound_duration, buzzer_override_tone);

	//decrement buzzer counter
	sound_duration--;

	//sound effect done, return to normal mode
	if (sound_duration == 0)
	{
		buzzer_override = false;
		buzzer_override_tone = 0;
	}
}

void start_sound(uint8_t sound)
{
	sound_type = sound;

	switch (sound_type)
	{
	case (SOUND_NONE):
		sound_duration = 0;
		buzzer_override = false;
		buzzer_override_tone = 0;
		break;

	case (SOUND_OFF):
		LEDG_OFF;
		sound_duration = 50;
		break;

	case (SOUND_ADC):
		LEDR_ON;
		sound_duration = 50;
		break;

	case (SOUND_TIM):
		sound_duration = 50;
		break;

	case (SOUND_ON):
		sound_duration = 60;
		if (cfg.supress_startup)
			sound_duration = 500;
		break;

	case (SOUND_LIFT):
		sound_duration = 100;
		break;

	case (SOUND_SINK):
		sound_duration = 100;
		break;

	case (SOUND_BEEP):
		sound_duration = BEEP_DURATION * (beep_count * 2 + 1);
		break;
	}
	buzzer_override = true;
}

uint32_t next_step_wait = 0;
uint8_t first_time = true;

uint8_t play_cfg;

extern float ram_sink_begin;
extern float ram_lift_begin;

//load threshold values for lift & sink
void LiftSinkRefresh()
{
	ram_lift_begin = cfg.lift_steps[prof.lift_treshold] / 100.0;
	ram_sink_begin = cfg.sink_steps[prof.sink_treshold] / 100.0;
}

#define LED_MODE_IDLE	0
#define LED_MODE_ALT	1
#define LED_MODE_R		2
#define LED_MODE_G		3
#define LED_MODE_G_ON	4
#define LED_MODE_R_ON	5
#define LED_MODE_BOTH	6
#define LED_MODE_BLINK	7

uint8_t blik = 0;
uint8_t led_mode = LED_MODE_IDLE;

//visual feedback
void led_task()
{
	blik = (blik + 1) % 250;

	switch (led_mode)
	{
		case(LED_MODE_IDLE):
			LEDG_OFF
			LEDR_OFF
		break;

		case(LED_MODE_BOTH):
			LEDG_ON
			LEDR_ON
		break;

		case(LED_MODE_G_ON):
			LEDG_ON
			LEDR_OFF
		break;

		case(LED_MODE_R_ON):
			LEDG_OFF
			LEDR_ON
		break;

		//both blinking (for profile select)
		case(LED_MODE_BLINK):
			if (blik % 10 < 5)
			{
				LEDR_ON
				LEDG_ON
			}
			else
			{
				LEDG_OFF
				LEDR_OFF
			}
		break;

		//alternate blinking (for menu)
		case(LED_MODE_ALT):
			if (blik % 10 < 5)
			{
				LEDR_ON
				LEDG_OFF
			}
			else
			{
				LEDG_ON
				LEDR_OFF
			}
		break;

		//blink red (for sink menu)
		case(LED_MODE_R):
			if (blik % 10 < 5)
			{
				LEDR_ON
			}
			else
			{
				LEDR_OFF
			}
			LEDG_OFF
		break;

		//blink green (for lift menu)
		case(LED_MODE_G):
			if (blik % 10 < 5)
			{
				LEDG_ON
			}
			else
			{
				LEDG_OFF
			}
			LEDR_OFF
		break;
	}
}

extern float altitude0;

volatile uint32_t auto_start_time = 0;
volatile int16_t auto_start_value = 0;

void auto_off_reset()
{
	auto_start_time = sys_tick_get();
	auto_start_value = altitude0;
}

void auto_off_step()
{
	if (cfg.auto_poweroff == 0)
		return;

	if (abs(auto_start_value - altitude0) > AUTO_THOLD)
	{
		auto_off_reset();
	}
	else
	{
		if ((uint32_t)sys_tick_get() - (uint32_t)auto_start_time > ((uint32_t)cfg.auto_poweroff * 1000))
		{
			start_sound(SOUND_OFF);
		}
	}
}

bool first_click = false;


//BUI loop
// - button user interface :-)
void bui_step()
{
	//play power up sequence
	if (first_time)
	{
		start_sound(SOUND_ON);
		first_time = false;
	}

	//make some noise
	// - audio feedbacks from menu, not the vario sounds!
	sound_task();

	//block loop if sound is playing
	if (sound_duration > 0)
	{
		//but reset wait timer
		next_step_wait = sys_tick_get();
		return;
	}

	//handle buttons
	button_task();
	//handle leds
	led_task();

	//enable sound when vario is idle
	if (bui_mode == BUI_MODE_IDLE)
	{
		buzzer_override = false;
		auto_off_step();
	}
	else
	//enable sound in menu
	{
		buzzer_override = true;
		buzzer_override_tone = 0;
		auto_off_reset();
	}


	switch (bui_mode)
	{

	//idle
	case(BUI_MODE_IDLE):

		if (bui_is_extra_long())
		{
			if (pressed_from_begining)
			{
				//button is still pressed -> go to profile setup
				next_step_wait = sys_tick_get();
				bui_mode = BUI_MODE_PROFILE;
				play_cfg = true;
				bui_button_clear();
			}
			else
			{
				//power off
				start_sound(SOUND_OFF);
				bui_button_clear();
			}
		}


		//if double click go to menu
		if (bui_was_short())
		{
			if (first_click)
			{
				//second click -> go to menu
				led_mode = LED_MODE_ALT;
				next_step_wait = sys_tick_get();
				bui_mode = BUI_MODE_UNLOCK;
				bui_button_clear();
			}
			else
			{
				//first click
				first_click = true;
				next_step_wait = sys_tick_get();
				bui_button_clear();
			}
		}

		//too slow, reset click state
		if (sys_tick_get() - next_step_wait > BUI_SHORT_WAIT && button_state == BUTTON_IDLE)
		{
			first_click = false;
		}


	break;

	//menu
	case(BUI_MODE_UNLOCK):

		//if short click in menu toggle volume settings
		if (bui_was_short())
		{
			bui_button_clear();
			volume_toggle();
			auto_off_reset();
			next_step_wait = sys_tick_get();
		}

		//visual feedback for entering lift & sink menu
		if (bui_is_long())
		{
			led_mode = LED_MODE_BOTH;
		}

		//next step is to choose lift or sink menu
		if (bui_was_long())
		{
			next_step_wait = sys_tick_get();
			bui_mode = BUI_MODE_SETTINGS;
			bui_button_clear();
		}

		//no action for a while, reset to idle state
		if (sys_tick_get() - next_step_wait > BUI_LONG_WAIT && button_state == BUTTON_IDLE)
		{
			bui_mode = BUI_MODE_IDLE;
			led_mode = LED_MODE_IDLE;
		}
	break;

	//go to lift or sink?
	case(BUI_MODE_SETTINGS):
		//visual feedbacks
		if (bui_is_short())
			led_mode = LED_MODE_G_ON;
		if (bui_is_long())
			led_mode = LED_MODE_R_ON;

		//second click was short -> lift menu
		if (bui_was_short())
		{
			bui_mode = BUI_MODE_LIFT;
			play_cfg = true;
			bui_button_clear();
			led_mode = LED_MODE_G_ON;
			start_sound(SOUND_LIFT);
			next_step_wait = sys_tick_get();
		}

		//second click was long -> sink menu
		if (bui_was_long())
		{
			bui_mode = BUI_MODE_SINK;
			play_cfg = true;
			bui_button_clear();
			led_mode = LED_MODE_R_ON;
			start_sound(SOUND_SINK);
			next_step_wait = sys_tick_get();
		}

		//no action for a while, reset to idle state
		if (sys_tick_get() - next_step_wait > BUI_WAIT && button_state == BUTTON_IDLE)
		{
			bui_mode = BUI_MODE_IDLE;
			led_mode = LED_MODE_IDLE;
		}
	break;

	//lift menu
	case(BUI_MODE_LIFT):
		//blinking green
		led_mode = LED_MODE_G_ON;

		//toggle configuration on short click
		if (bui_was_short())
		{
			bui_button_clear();
			next_step_wait = sys_tick_get();

			prof.lift_treshold++;
			if (prof.lift_treshold > 5)
				prof.lift_treshold = 1;

			LiftSinkRefresh();

			play_cfg = true;
		}

		//the menu was just opened or changed
		// playback the actual configuration
		if (play_cfg)
		{
			play_cfg = false;
			beep_count = prof.lift_treshold;
			beep_blik = BEEP_BLIK_GREEN;
			start_sound(SOUND_BEEP);
		}

		//no action for a while, reset to idle state
		if (sys_tick_get() - next_step_wait > BUI_LONG_WAIT && button_state == BUTTON_IDLE)
		{
			bui_mode = BUI_MODE_IDLE;
			led_mode = LED_MODE_IDLE;
			StoreLift();
		}
	break;

	//sink menu
	case(BUI_MODE_SINK):
		led_mode = LED_MODE_R_ON;

		//toggle configuration on short click
		if (bui_was_short())
		{
			bui_button_clear();
			next_step_wait = sys_tick_get();

			prof.sink_treshold++;
			if (prof.sink_treshold > 5)
				prof.sink_treshold = 1;

			LiftSinkRefresh();

			play_cfg = true;
		}

		//the menu was just opened or changed
		// playback the actual configuration
		if (play_cfg)
		{
			play_cfg = false;
			beep_count = prof.sink_treshold;
			beep_blik = BEEP_BLIK_RED;
			start_sound(SOUND_BEEP);
		}

		//no action for a while, reset to idle state
		if (sys_tick_get() - next_step_wait > BUI_LONG_WAIT && button_state == BUTTON_IDLE)
		{
			bui_mode = BUI_MODE_IDLE;
			led_mode = LED_MODE_IDLE;
			StoreSink();
		}
	break;

	//sink menu
	case(BUI_MODE_PROFILE):
		led_mode = LED_MODE_BLINK;

		//toggle configuration on short click
		if (bui_was_short())
		{
			bui_button_clear();
			next_step_wait = sys_tick_get();

			cfg.selected_profile++;
			if (cfg.selected_profile > 2)
				cfg.selected_profile = 0;

			play_cfg = true;
		}

		//the menu was just opened or changed
		// playback the actual configuration
		if (play_cfg)
		{
			play_cfg = false;
			beep_count = cfg.selected_profile + 1;
			beep_blik = BEEP_BLIK_BOTH;
			start_sound(SOUND_BEEP);
		}

		//no action for a while, reset to idle state
		if (sys_tick_get() - next_step_wait > BUI_LONG_WAIT && button_state == BUTTON_IDLE)
		{
			bui_mode = BUI_MODE_IDLE;
			led_mode = LED_MODE_IDLE;
			LoadProfile();
		}
	break;

	}
}
