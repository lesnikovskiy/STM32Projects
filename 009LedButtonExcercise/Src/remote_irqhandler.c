#include "remote_irqhandler.h"

void EXTI1_IRQHandler(void) {
	if (EXTI->PR & (1 << 1)) { // Check if interrupt is at line 1
		EXTI->PR |= (1 << 1); // Reset flag for line 1

		static uint32_t last_time = 0;
		static uint32_t ir_data = 0;
		static int bit_count = 0;

		uint32_t current_time = TIM4->CNT;
		uint32_t diff = current_time - last_time;
		last_time = current_time;

		// 1. Detect Samsung Start Header (~4.5ms + 4.5ms = 9ms -> ~900 ticks)
		if (diff > 850 && diff < 950) {
			bit_count = 0;
			ir_data = 0;
			return;
		}

		// 2. Collect 32 bits after the header
		if (bit_count < 32) {
			// Samsung: '0' - 1.12ms (112 ticks), '1' - 2.25ms (225 ticks)
			// Threshold 1.6ms (160 ticks)
			if (diff > 160) {
				ir_data |= (1UL << (31 - bit_count)); // MSB First
			}
			bit_count++;

			// 3. The package is built
			if (bit_count == 32) {
				usart_send_str("IR Data: ");
				usart_send_hex32(ir_data);
				usart_send_str("\r\n");

				// Example: Control logic based on button code
				// Replace 0x0C with your actual button codes from UART
				switch (ir_data) {
				case (SAMSUNG_ADDR_TV << 16 | BTN_0):
					global_mode = 0;
					update_mode_settings();
					usart_send_str("Mode Normal set\r\n");
					break;
				case (SAMSUNG_ADDR_TV << 16 | BTN_1):
					global_mode = 1;
					update_mode_settings();
					usart_send_str("Mode Fast set\r\n");
					break;
				case (SAMSUNG_ADDR_TV << 16 | BTN_2):
					global_mode = 2;
					update_mode_settings();
					usart_send_str("Mode Slow set\r\n");
					break;
				case (SAMSUNG_ADDR_TV << 16 | BTN_3):
					global_mode = 3;
					update_mode_settings();
					usart_send_str("Mode SHIM set\r\n");
					break;
				case (SAMSUNG_ADDR_TV << 16 | BTN_VOL_UP):
				case (SAMSUNG_ADDR_TV << 16 | BTN_CH_UP):
					if (duty_cycle <= 90)
						duty_cycle += 10;
					else
						duty_cycle = 100;
					usart_send_str("Brightness up\r\n");
					break;
				case (SAMSUNG_ADDR_TV << 16 | BTN_VOL_DOWN):
				case (SAMSUNG_ADDR_TV << 16 | BTN_CH_DOWN):
					if (duty_cycle >= 10)
						duty_cycle -= 10;
					else
						duty_cycle = 0;
					usart_send_str("Brightness DOWN\r\n");
					break;
				case (SAMSUNG_ADDR_TV << 16 | BTN_POWER):
					usart_send_str("!!! CRITICAL SYSTEM ERROR SIMULATED !!!\r\n");
					usart_send_str("System will freeze now. Watchdog should reset in 2s...\r\n");
					while (1) {
						// Processor hung up
					}
					break;
				default:
					usart_send_str("Unknown command for code ");
					usart_send_hex32(ir_data);
					usart_send_str("\r\n");
				}
			}
		}
	}
}

// NEC HX1838 Kit version
//void EXTI1_IRQHandler(void) {
//	if (EXTI->PR & (1 << 1)) { // Check if interrupt is at line 1
//		EXTI->PR |= (1 << 1); // Reset flag for line 1
//
//		static uint32_t last_time = 0;
//		static uint32_t ir_data = 0;
//		static int bit_count = 0;
//
//		uint32_t current_time = TIM4->CNT;
//		uint32_t diff = current_time - last_time;
//		last_time = current_time;
//
//		// 1. Detect NEC Start Header (~13.5ms = 1350 ticks)
//		if (diff > 1300 && diff < 1400) {
//			bit_count = 0;
//			ir_data = 0;
//			return;
//		}
//
//		// 2. Collect 32 bits after the header
//		if (bit_count < 32) {
//			// Threshold between '0' (1.12ms) and '1' (2.25ms)
//			// set to 1.6ms (160 ticks)
//			if (diff < 160) {
//				ir_data |= (1UL << (31 - bit_count));
//			}
//			bit_count++;
//
//			// 3. The package is built
//			if (bit_count == 32) {
//				usart_send_str("IR Data: ");
//				usart_send_hex32(ir_data);
//				usart_send_str("\r\n");
//
//				// NEC Protocol: [Address][Inv Address][Command][Inv Command]
//				uint8_t cmd = (uint8_t) ((ir_data >> 8) & 0xFF);
//
//				usart_send_str("IR CMD: 0x");
//				usart_send_int_raw(cmd);
//				usart_send_str("\r\n");
//
//				// Example: Control logic based on button code
//				// Replace 0x0C with your actual button codes from UART
//				switch (cmd) {
//				case 103:
//					global_mode = 0;
//					update_mode_settings();
//					usart_send_str("Mode Normal set\r\n");
//					break;
//				case 93:
//					global_mode = 1;
//					update_mode_settings();
//					usart_send_str("Mode Fast set\r\n");
//					break;
//				case 157:
//					global_mode = 2;
//					update_mode_settings();
//					usart_send_str("Mode Slow set\r\n");
//					break;
//				case 29:
//					global_mode = 3;
//					update_mode_settings();
//					usart_send_str("Mode SHIM set\r\n");
//					break;
//				default:
//					usart_send_str("Unknown command for code ");
//					usart_send_int_raw(cmd);
//					usart_send_str("\r\n");
//				}
//			}
//		}
//	}
//}
