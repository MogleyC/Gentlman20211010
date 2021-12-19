#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

//Project Option->Preprocessor Definitions
//NRF52
//NRF52832_XXAA
//NRF52_PAN_74
//CONFIG_GPIO_AS_PINRESET
//INITIALIZE_USER_SECTIONS
//FLOAT_ABI_HARD
//S112
//SOFTDEVICE_PRESENT
//NO_VTOR_CONFIG
//NRF_SD_BLE_API_VERSION=7
//APP_TIMER_V2
//APP_TIMER_V2_RTC1_ENABLED

//!!FLOAT_ABI_HARD is need undef in nrf52810!!

#define LED_1_PIN_NUMBER 16
#define LED_2_PIN_NUMBER 17
#define BTN_1_PIN_NUMBER 15

#define RX_PIN_NUMBER 11
#define TX_PIN_NUMBER 12
#define CTS_PIN_NUMBER 13
#define RTS_PIN_NUMBER 14

//#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
//#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */
#define UART_TX_BUF_SIZE 512                         /**< UART TX buffer size for RSSI. */
#define UART_RX_BUF_SIZE 2048                         /**< UART RX buffer size for RSSI. */

#ifdef __cplusplus
}
#endif

#endif //BOARD_CONFIG_H