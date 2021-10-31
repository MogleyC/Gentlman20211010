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

#define LED_1_PIN_NUMBER 13
#define LED_2_PIN_NUMBER 12
#define BTN_1_PIN_NUMBER 11

#define RX_PIN_NUMBER 17
#define TX_PIN_NUMBER 16
#define CTS_PIN_NUMBER 15
#define RTS_PIN_NUMBER 14

#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */

#ifdef __cplusplus
}
#endif

#endif //BOARD_CONFIG_H