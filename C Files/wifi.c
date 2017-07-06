#include "camera.h"
#include "wifi.h"
#include "timer_interface.h"
#include <string.h>
#include <stdio.h>

volatile uint32_t received_byte_wifi = 0;
volatile bool new_rx_wifi = false;

volatile bool wifi_web_setup_flag = false;
volatile unsigned int input_pos_wifi = 0;
volatile bool wifi_comm_success = false;
volatile bool wifi_socket_success = false;
volatile bool wifi_file_create_success = false;
volatile bool wifi_file_delete_success = false;
volatile bool wifi_file_open_success = false;
volatile bool wifi_file_write_success = false;
volatile bool wifi_streams_unavailable = false;
volatile bool wifi_stream0_available = false;
volatile bool wifi_stream1_available = false;
volatile bool wifi_stream2_available = false;
volatile bool wifi_stream3_available = false;
volatile bool wifi_stream4_available = false;
volatile bool wifi_stream5_available = false;
volatile bool wifi_stream6_available = false;
volatile bool wifi_stream7_available = false;

volatile char wifi_pan = 0;
volatile char wifi_tilt = 0;
volatile char wifi_zoom = 0;

volatile char motor_p = 0;
volatile char motor_t = 0;
volatile char motor_z = 0;

volatile char wifi_pan1 = 0;
volatile char wifi_pan2 = 0;
volatile char wifi_tilt1 = 0;
volatile char wifi_tilt2 = 0;
volatile char wifi_zoom1 = 0;
volatile char wifi_zoom2 = 0;

volatile char wifi_j1 = 0;
volatile char wifi_j2 = 0;
volatile char wifi_j3 = 0;
volatile char wifi_j4 = 0;
volatile char wifi_j5 = 0;
volatile char wifi_j6 = 0;
volatile char wifi_j7 = 0;


void WIFI_USART_HANDLER(void)
{
	uint32_t ul_status;

	/* Read USART status. */
	ul_status = usart_get_status(WIFI_USART);

	/* Receive buffer is full. */
	if (ul_status & US_CSR_RXBUFF) {
		usart_read(WIFI_USART, &received_byte_wifi);
		new_rx_wifi = true;
		processIncomingByte_wifi((uint8_t)received_byte_wifi);
	}
}

void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask)
{
	unused(ul_id);
	unused(ul_mask);
	
	wifi_comm_success = true;
	process_data_wifi();
	for (int jj=0;jj<MAX_INPUT_WIFI;jj++) input_line_wifi[jj] = 0;
	input_pos_wifi = 0;
}

void wifi_web_setup_handler(uint32_t ul_id, uint32_t ul_mask)
{
	unused(ul_id);
	unused(ul_mask);
	
	wifi_web_setup_flag = true;
}

void configure_usart_wifi(void)
{
	gpio_configure_pin(PIN_USART0_RXD_IDX, PIN_USART0_RXD_FLAGS);
	gpio_configure_pin(PIN_USART0_TXD_IDX, PIN_USART0_TXD_FLAGS);
	gpio_configure_pin(PIN_USART0_CTS_IDX, PIN_USART0_CTS_FLAGS);
	
	static uint32_t ul_sysclk;
	const sam_usart_opt_t wifi_usart_settings = {
		WIFI_USART_BAUDRATE,
		WIFI_USART_CHAR_LENGTH,
		WIFI_USART_PARITY,
		WIFI_USART_STOP_BITS,
		WIFI_USART_MODE,
		/* This field is only used in IrDA mode. */
		0
	};
	
	/* Get system clock. */
	ul_sysclk = sysclk_get_cpu_hz();
	
	pmc_enable_periph_clk(WIFI_USART_ID);
	usart_init_rs232(WIFI_USART,&wifi_usart_settings,ul_sysclk); // turn off flow control. RTS pin doesn't work right.
	//usart_init_hw_handshaking(WIFI_USART,&wifi_usart_settings,ul_sysclk);

	/* Disable all the interrupts. */
	usart_disable_interrupt(WIFI_USART, ALL_INTERRUPT_MASK);
	
	/* Enable TX & RX function. */
	usart_enable_tx(WIFI_USART);
	usart_enable_rx(WIFI_USART);

	usart_enable_interrupt(WIFI_USART, US_IER_RXRDY);

	/* Configure and enable interrupt of USART. */
	NVIC_EnableIRQ(WIFI_USART_IRQn);
}

void configure_wifi_comm_pin(void)
{
	/* Configure PIO clock. */
	pmc_enable_periph_clk(WIFI_COMM_ID);

	/* Initialize PIO interrupt handler, see PIO definition in conf_board.h
	**/
	pio_handler_set(WIFI_COMM_PIO, WIFI_COMM_ID, WIFI_COMM_PIN_NUM, WIFI_COMM_ATTR, wifi_command_response_handler);

	/* Enable PIO controller IRQs. */
	NVIC_EnableIRQ((IRQn_Type)WIFI_COMM_ID);

	/* Enable PIO interrupt lines. */
	pio_enable_interrupt(WIFI_COMM_PIO, WIFI_COMM_PIN_NUM);
}

void configure_wifi_web_setup_pin(void)
{
	/* Configure PIO clock. */
	pmc_enable_periph_clk(WIFI_WEB_SETUP_ID);

	/* Initialize PIO interrupt handler, see PIO definition in conf_board.h
	**/
	pio_handler_set(WIFI_WEB_SETUP_PIO, WIFI_WEB_SETUP_ID, WIFI_WEB_SETUP_PIN_NUM, WIFI_WEB_SETUP_ATTR, wifi_web_setup_handler);

	/* Enable PIO controller IRQs. */
	NVIC_EnableIRQ((IRQn_Type)WIFI_WEB_SETUP_ID);

	/* Enable PIO interrupt lines. */
	pio_enable_interrupt(WIFI_WEB_SETUP_PIO, WIFI_WEB_SETUP_PIN_NUM);
}

void write_wifi_command(char* comm, uint8_t cnt)
{
	wifi_comm_success = false;
	usart_write_line(WIFI_USART, comm);
	counts = 0;
	while(!wifi_comm_success && counts<cnt);
}


void write_image_to_file(void)
{
	if (len_image == 0) return;
	else {
		char file_create_string[100];
		char socket_string[100];
		
		/*
		wifi_file_delete_success = false;
		usart_write_line(WIFI_USART, "fde webcam/images/img.jpg\r\n");
		counts = 0;
		while(!wifi_file_delete_success && counts<10);
		

		wifi_file_create_success = false;
		sprintf(file_create_string,"fcr webcam/images/img.jpg %d\r",len_image);
		
		usart_write_line(WIFI_USART, file_create_string);
		for (uint32_t jj=0;jj<len_image;jj++) {
			usart_putchar(WIFI_USART, image_buf[soi_addr+jj]);
		}
			
		counts = 0;
		while(!wifi_file_create_success && counts<10);
		*/
		
		// HTTP version
		write_wifi_command("close 0\r\n", 3);
		sprintf(socket_string,"http_post -i wlan -l %d http://73.44.245.13:1881/posturl image/jpeg\r",len_image);
		usart_write_line(WIFI_USART, socket_string);
		sprintf(socket_string,"write 0 %d\r\n",len_image);
		usart_write_line(WIFI_USART, socket_string);
		for (uint32_t jj=0;jj<len_image;jj++) {
			usart_putchar(WIFI_USART, image_buf[soi_addr+jj]);
		}
		usart_write_line(WIFI_USART, "close 0\r\n");
		delay_ms(75);
		usart_write_line(WIFI_USART, "fop junk.txt\r\n");
		
		/*
		// Websocket version
		sprintf(socket_string,"write 0 %d\r\n",len_image);
		usart_write_line(WIFI_USART, socket_string);
		for (uint32_t kk=0;kk<len_image;kk++) {
			usart_putchar(WIFI_USART, image_buf[soi_addr+kk]);
		}
		*/
		
		/*
		usart_write_line(WIFI_USART, "write 0 5\r\n");	
		usart_putchar(WIFI_USART, 75);
		usart_putchar(WIFI_USART, 76);
		usart_putchar(WIFI_USART, 77);
		usart_putchar(WIFI_USART, 78);
		usart_putchar(WIFI_USART, 79);
		*/
		
		counts = 0;
		while(!wifi_file_create_success && counts<5){};	
		return;
	}
}

volatile void processIncomingByte_wifi(uint8_t inByte)
{
	
	static char input_line [MAX_INPUT_WIFI];
	static unsigned int input_pos = 0;
	
	input_line_wifi[input_pos_wifi++] = inByte;
}

volatile void process_data_wifi ()
{
	char* sub;
	
	if (strstr(input_line_wifi, "File deleted") || strstr(input_line_wifi, "File not found")) {
		wifi_file_delete_success = true;
	}
	else if (strstr(input_line_wifi,"File created") || strstr(input_line_wifi,"File already exists")) {
		wifi_file_create_success = true;
	}
	else if (strstr(input_line_wifi, "Opened")) {
		wifi_file_open_success = true;
	}
	else if (strstr(input_line_wifi, "Success")) {
		wifi_file_write_success = true;
	}
	else if (strstr(input_line_wifi, "None")) {
		wifi_streams_unavailable = true;
	}
	if (strstr(input_line_wifi, "# 0")) {
		wifi_stream0_available = true;
	}
	if (strstr(input_line_wifi, "# 1")) {
		wifi_stream1_available = true;
	}
	if (strstr(input_line_wifi, "# 2")) {
		wifi_stream2_available = true;
	}
	if (strstr(input_line_wifi, "# 3")) {
		wifi_stream3_available = true;
	}
	if (strstr(input_line_wifi, "# 4")) {
		wifi_stream4_available = true;
	}
	if (strstr(input_line_wifi, "# 5")) {
		wifi_stream5_available = true;
	}
	if (strstr(input_line_wifi, "# 6")) {
		wifi_stream6_available = true;
	}
	if (strstr(input_line_wifi, "# 7")) {
		wifi_stream7_available = true;
	}
	
	if (strstr(input_line_wifi, "~~,")) {
		wifi_j1 = input_line_wifi[0];			// /n
		wifi_j2 = input_line_wifi[1];			// /r
		wifi_j3 = input_line_wifi[2];			// ~
		wifi_j4 = input_line_wifi[3];			// ~
		wifi_j5 = input_line_wifi[4];			// ,
		wifi_pan1 = input_line_wifi[5] - 33;		// Pan1
		wifi_pan2 = input_line_wifi[6] - 33;		// Pan2
		wifi_j6 = input_line_wifi[7];			// ,
		wifi_tilt1 = input_line_wifi[8] - 33;		// Tilt1
		wifi_tilt2 = input_line_wifi[9] - 33;		// Tilt2
		wifi_j7 = input_line_wifi[10];			// ,
		wifi_zoom1 = input_line_wifi[11] - 33;		// Zoom1
		wifi_zoom2 = input_line_wifi[12] - 33;		// Zoom2
		
		wifi_pan = wifi_pan1 + wifi_pan2 + ((!wifi_pan2 && wifi_pan1) * 90);
		wifi_tilt = wifi_tilt1 + wifi_tilt2 + ((!wifi_tilt2 && wifi_tilt1) * 90);
		wifi_zoom = wifi_zoom1 + wifi_zoom2 + ((!wifi_zoom2 && wifi_zoom1)* 90);
		
		motor_p = (wifi_pan / 2) + 50;
		motor_t = (wifi_tilt / 2) + 50;
		motor_z = (wifi_zoom / 2) + 50;

	}
	
	if (strstr(input_line_wifi,"Unknown command")) {
		return;
	}
}
