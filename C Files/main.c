#include <asf.h>
#include <stdio.h>
#include <string.h>
#include "wifi.h"
#include "asf.h"
#include "camera.h"
#include "conf_board.h"
#include "conf_clock.h"
#include "timer_interface.h"

/** PWM frequency in Hz */
#define PWM_FREQUENCY      250
/** Period value of PWM output waveform */
#define PERIOD_VALUE       250
/** Initial duty cycle value */
#define INIT_DUTY_VALUE    0

/** PWM channel instance for LEDs */
pwm_channel_t g_pwm_channel_led;

char junk[100];


int main (void)
{
	// System initialization
	sysclk_init();
	wdt_disable(WDT);
	board_init();
	
	// Timeout timer initialization
	configure_tc();
	tc_start(TC0, 0);
	
	/* -----------------------------------
	 * Begin PWM Configuration
	 * -----------------------------------
	 */
	
	/* Enable PWM peripheral clock */
	pmc_enable_periph_clk(ID_PWM);
	
	/* Disable PWM channels for LEDs */
	pwm_channel_disable(PWM, PIN_PWM_LED0_CHANNEL);
	pwm_channel_disable(PWM, PIN_PWM_LED1_CHANNEL);
	pwm_channel_disable(PWM, PIN_PWM_LED2_CHANNEL);
	
	/* Set PWM clock A as PWM_FREQUENCY*PERIOD_VALUE (clock B is not used) */
	pwm_clock_t clock_setting = {
		.ul_clka = PWM_FREQUENCY * PERIOD_VALUE,
		.ul_clkb = 0,
		.ul_mck = sysclk_get_cpu_hz()
	};
	
	pwm_init(PWM, &clock_setting);
	
	/* Initialize PWM channel for LED0 */
	/* Period is left-aligned */
	g_pwm_channel_led.alignment = PWM_ALIGN_LEFT;
	/* Output waveform starts at a low level */
	g_pwm_channel_led.polarity = PWM_HIGH;
	/* Use PWM clock A as source clock */
	g_pwm_channel_led.ul_prescaler = PWM_CMR_CPRE_CLKA;
	/* Period value of output waveform */
	g_pwm_channel_led.ul_period = PERIOD_VALUE;
	/* Duty cycle value of output waveform */
	g_pwm_channel_led.ul_duty = INIT_DUTY_VALUE;
	g_pwm_channel_led.channel = PIN_PWM_LED0_CHANNEL;
	pwm_channel_init(PWM, &g_pwm_channel_led);
	
	/* Enable channel counter event interrupt */
	pwm_channel_disable_interrupt(PWM, PIN_PWM_LED0_CHANNEL, 0);
	
	
	/* Initialize PWM channel for LED1 */
	/* Period is center-aligned */
	g_pwm_channel_led.alignment = PWM_ALIGN_LEFT;
	/* Output waveform starts at a high level */
	g_pwm_channel_led.polarity = PWM_HIGH;
	/* Use PWM clock A as source clock */
	g_pwm_channel_led.ul_prescaler = PWM_CMR_CPRE_CLKA;
	/* Period value of output waveform */
	g_pwm_channel_led.ul_period = PERIOD_VALUE;
	/* Duty cycle value of output waveform */
	g_pwm_channel_led.ul_duty = INIT_DUTY_VALUE;
	g_pwm_channel_led.channel = PIN_PWM_LED1_CHANNEL;
	pwm_channel_init(PWM, &g_pwm_channel_led);
	
	/* Disable channel counter event interrupt */
	pwm_channel_disable_interrupt(PWM, PIN_PWM_LED1_CHANNEL, 0);
	
	
	/* Initialize PWM channel for LED2 */
	/* Period is center-aligned */
	g_pwm_channel_led.alignment = PWM_ALIGN_LEFT;
	/* Output waveform starts at a high level */
	g_pwm_channel_led.polarity = PWM_HIGH;
	/* Use PWM clock A as source clock */
	g_pwm_channel_led.ul_prescaler = PWM_CMR_CPRE_CLKA;
	/* Period value of output waveform */
	g_pwm_channel_led.ul_period = PERIOD_VALUE;
	/* Duty cycle value of output waveform */
	g_pwm_channel_led.ul_duty = INIT_DUTY_VALUE;
	g_pwm_channel_led.channel = PIN_PWM_LED2_CHANNEL;
	pwm_channel_init(PWM, &g_pwm_channel_led);
	
	/* Disable channel counter event interrupt */
	pwm_channel_disable_interrupt(PWM, PIN_PWM_LED2_CHANNEL, 0);
	
	
	/* Enable PWM channels for LEDs */
	pwm_channel_enable(PWM, PIN_PWM_LED0_CHANNEL);
	pwm_channel_enable(PWM, PIN_PWM_LED1_CHANNEL);
	pwm_channel_enable(PWM, PIN_PWM_LED2_CHANNEL);
	
	/* Set final PWM flags */
	static uint32_t ul_count = 0;  /* PWM counter value */
	static uint32_t ul_duty = INIT_DUTY_VALUE;  /* PWM duty cycle rate */
	static uint8_t fade_in = 1;  /* LED fade in flag */
	
	char junk_str[20];
	
	
	/* -----------------------------------
	 * End PWM Configuration
	 * -----------------------------------
	 */
	
	configure_usart_wifi();
	configure_wifi_comm_pin();
	configure_wifi_web_setup_pin();
	ioport_set_pin_level(WIFI_RESET_PIN, false);
	delay_ms(50);
	ioport_set_pin_level(WIFI_RESET_PIN, true);
	delay_ms(100);
	write_wifi_command("set system.cmd.echo off\r\n",3);
	delay_ms(1000);
	
	/*
	while(1){
		g_pwm_channel_led.channel = PIN_PWM_LED0_CHANNEL;
		pwm_channel_update_duty(PWM, &g_pwm_channel_led, 50); //50
		g_pwm_channel_led.channel = PIN_PWM_LED1_CHANNEL;
		pwm_channel_update_duty(PWM, &g_pwm_channel_led, 50);
		
		delay_ms(100); // 3.1
		
		g_pwm_channel_led.channel = PIN_PWM_LED0_CHANNEL;
		pwm_channel_update_duty(PWM, &g_pwm_channel_led, 70); //70
		g_pwm_channel_led.channel = PIN_PWM_LED1_CHANNEL;
		pwm_channel_update_duty(PWM, &g_pwm_channel_led, 70);
		
		delay_ms(100); // 2.1
		
		g_pwm_channel_led.channel = PIN_PWM_LED0_CHANNEL;
		pwm_channel_update_duty(PWM, &g_pwm_channel_led, 95); //95
		g_pwm_channel_led.channel = PIN_PWM_LED1_CHANNEL;
		pwm_channel_update_duty(PWM, &g_pwm_channel_led, 95);
		
		delay_ms(100); // 1.1
		
		g_pwm_channel_led.channel = PIN_PWM_LED0_CHANNEL;
		pwm_channel_update_duty(PWM, &g_pwm_channel_led, 125); //125
		g_pwm_channel_led.channel = PIN_PWM_LED1_CHANNEL;
		pwm_channel_update_duty(PWM, &g_pwm_channel_led, 125);
		
		delay_ms(100); // 0.8
		
		g_pwm_channel_led.channel = PIN_PWM_LED0_CHANNEL;
		pwm_channel_update_duty(PWM, &g_pwm_channel_led, 160); //160
		g_pwm_channel_led.channel = PIN_PWM_LED1_CHANNEL;
		pwm_channel_update_duty(PWM, &g_pwm_channel_led, 160);
		
		delay_ms(100); // ??
		
	}
	*/
	
	/*
	while (1) {
		ul_count++;
		
		// Fade in/out
		if (ul_count == (PWM_FREQUENCY / (PERIOD_VALUE - INIT_DUTY_VALUE))) {
			// Fade in
			if (fade_in) {
				ul_duty++;
				if (ul_duty == PERIOD_VALUE) {
					fade_in = 0;
				}
			} else {
				// Fade out
				ul_duty--;
				if (ul_duty == INIT_DUTY_VALUE) {
					fade_in = 1;
				}
			}
			
			
			// Set new duty cycle
			ul_count = 0;
			
			sprintf(junk_str,"%d\r\n",ul_duty);
			usart_write_line(WIFI_USART, junk_str);
			
			g_pwm_channel_led.channel = PIN_PWM_LED0_CHANNEL;
			pwm_channel_update_duty(PWM, &g_pwm_channel_led, ul_duty);
			
			g_pwm_channel_led.channel = PIN_PWM_LED1_CHANNEL;
			pwm_channel_update_duty(PWM, &g_pwm_channel_led, ul_duty);
			
			g_pwm_channel_led.channel = PIN_PWM_LED2_CHANNEL;
			pwm_channel_update_duty(PWM, &g_pwm_channel_led, ul_duty);
			
			delay_ms(50);
		}
	}
	*/
	
	
	/* -----------------------------------
	 * Begin WiFI Configuration
	 * -----------------------------------
	 */
	
	/*
	while(1){
			ioport_set_pin_level(WIFI_RESET_PIN, false);
			delay_ms(100);
		};
	*/	
		
	// WiFi initialization
	configure_usart_wifi(); // baud set to 8000000
	configure_wifi_comm_pin();
	configure_wifi_web_setup_pin();
	
	//write_wifi_command("set uart.baud 1 115200 raw\r\n", 3);
	write_wifi_command("set sy i g wlan 20\r\n", 3);
	write_wifi_command("set sy i g network 21\r\n", 3);
	write_wifi_command("set sy i g softap 22\r\n", 3);
	write_wifi_command("set sy c g 4\r\n", 3);
	write_wifi_command("set wl n o 3\r\n", 3);
	write_wifi_command("set bus.command.rx_bufsize 5000\r\n", 3);
	write_wifi_command("set system.cmd.prompt_enabled 0\r\n",3);
	write_wifi_command("set system.cmd.echo off\r\n",3);
	
	write_wifi_command("set mdns.enabled 1\r\n", 3);
	write_wifi_command("save\r\n", 3);

	/*
	// Reset WiFi module and wait for connection
	ioport_set_pin_level(WIFI_RESET_PIN, false);
	delay_ms(100);
	ioport_set_pin_level(WIFI_RESET_PIN, true);
	delay_ms(2000);
	write_wifi_command("ver\r\n", 3);
	delay_ms(100);
	*/
	
	/*
	wifi_web_setup_flag = false;
	while (!ioport_get_pin_level(WIFI_NETWORK_PIN)) {
		if (wifi_web_setup_flag) {
			usart_write_line(WIFI_USART, "setup web\r\n");
			wifi_web_setup_flag = false;
		}
	}
	*/
	
	
	
	// Camera initialization
	init_camera();
	ioport_set_pin_level(CAM_RST_PIN, true);
	delay_ms(50);
	configure_camera();
	delay_ms(2000);
	
	
	// Initialize websocket
	write_wifi_command("webc -i wlan ws://73.44.245.13:1881/ws/imgStream\r\n", 10);
	
	write_wifi_command("close all\r\n", 3);	
	delay_ms(200);
	write_wifi_command("set tc c t 2000\r\n", 3);
	
	/*
	// Websocket version
	write_wifi_command("webc -i wlan -f bin ws://73.44.245.13:1881/ws/imgStream\r\n", 10);
	*/
	
	// HTTP version
	write_wifi_command("http_post -i wlan -l 5 http://73.44.245.13:1881/posturl text/plain\r\n", 3);
	//write_wifi_command("write 0 5\r\nhello", 3);
	write_wifi_command("close 0\r\n", 3);
	write_wifi_command("fop junk.txt\r\n", 3);
	
	//delay_ms(3000);
	
	write_wifi_command("webc -i wlan ws://73.44.245.13:1881/ws/imgStream\r\n", 10);
	write_wifi_command("\r\n", 3);
	write_wifi_command("\r\n", 3);
	write_wifi_command("list\r\n", 3);
	
	delay_ms(100);
	
	while (1) { // Main loop
		/*
		if (wifi_web_setup_flag) {
			usart_write_line(WIFI_USART, "setup web\r\n");
			wifi_web_setup_flag = false;
		}
		*/
		
		
		// If user requests video, send video
		if (ioport_get_pin_level(WIFI_NETWORK_PIN)) {
			wifi_streams_unavailable = false;
			write_wifi_command("poll all\r\n",10);
			
			
			if(wifi_streams_unavailable) {
				delay_ms(1000);
			}
			else {
				// Take picture and write it to WiFi module
				
				if(start_capture()) {
					write_image_to_file();
				}
				
				
				// Check streams
				wifi_stream0_available = false;
				wifi_stream1_available = false;
				wifi_stream2_available = false;
				wifi_stream3_available = false;
				wifi_stream4_available = false;
				wifi_stream5_available = false;
				wifi_stream6_available = false;
				wifi_stream7_available = false;
				write_wifi_command("stream_list\r\n",10);
				
				// Check stream 0
				if (wifi_stream0_available) {
					//write_wifi_command("write 0 1\rh",10);
				}
				
				// Check stream 1
				if (wifi_stream1_available) {
					write_wifi_command("write 1 1\rh",10);
				}
				
				// Check stream 2
				if (wifi_stream2_available) {
					write_wifi_command("write 2 1\rh",10);
				}
				
				// Check stream 3
				if (wifi_stream3_available) {
					write_wifi_command("write 3 1\rh",10);
				}
				
				// Check stream 4
				if (wifi_stream4_available) {
					write_wifi_command("write 4 1\rh",10);
				}
				
				// Check stream 5
				if (wifi_stream5_available) {
					write_wifi_command("write 5 1\rh",10);
				}
				
				// Check stream 6
				if (wifi_stream6_available) {
					write_wifi_command("write 6 1\rh",10);
				}
				
				// Check stream 7
				if (wifi_stream7_available) {
					write_wifi_command("write 7 1\rh",10);
				}
						
			}
			
			for (uint8_t mm=0;mm<4;mm++) {
				write_wifi_command("read 1 50\r\n", 3); // read websocket for ptz vals
		
				g_pwm_channel_led.channel = PIN_PWM_LED0_CHANNEL;
				pwm_channel_update_duty(PWM, &g_pwm_channel_led, motor_p);
			
				delay_ms(100);
				g_pwm_channel_led.channel = PIN_PWM_LED1_CHANNEL;
				pwm_channel_update_duty(PWM, &g_pwm_channel_led, motor_t);
			}
			
		}
	}
}
