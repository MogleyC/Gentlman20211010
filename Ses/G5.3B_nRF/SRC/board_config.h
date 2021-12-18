#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

//Project Option->Preprocessor Definitions
//#define BOARD_PCA10040
//#define BSP_DEFINES_ONLY
//#define CONFIG_GPIO_AS_PINRESET
//#define FLOAT_ABI_HARD
//#define INITIALIZE_USER_SECTIONS
//#define NO_VTOR_CONFIG
//#define NRF52
//#define NRF52832_XXAA
//#define NRF52_PAN_74

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