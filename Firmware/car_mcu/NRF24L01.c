/*
 * NRF24L01.c
 *
 *  Created on: May 23, 2026
 */



// The NRF24L01 functions were taken from a tutorial video series, parts 1, 2, and 3. [https://www.youtube.com/watch?v=mB7LsiscM78&t=996s]


#include "stm32f4xx_hal.h"
#include "NRF24L01.h"

extern SPI_HandleTypeDef hspi1; // defined in main.c but want to use here
#define NRF24_SPI &hspi1

#define NRF24_CE_PORT GPIOA
#define NRF24_CE_PIN GPIO_PIN_4

#define NRF24_CSN_PORT GPIOA
#define NRF24_CSN_PIN GPIO_PIN_8

void CSN_Select(void) {
	HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_RESET);
}

void CSN_UnSelect(void) {
	HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_SET);
}

void CE_Enable(void) {
	HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_SET);
}

void CE_Disable(void) {
	HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_RESET);
}

// can't directly access registers in code, need commands over SPI
// write single byte to the particular register
void nrf24_WriteReg(uint8_t Reg, uint8_t Data) {
	uint8_t buf[2];
	buf[0] = Reg | 1 << 5; // move 1 to the fifth bit in buf[0]
	buf[1] = Data;

	// Pull the CSN pin low to select the device
	CSN_Select();

	HAL_SPI_Transmit(NRF24_SPI, buf, 2, 1000); // sends contents of buf over SPI

	// Pull the CSN pin high to release device
	CSN_UnSelect();
}

// write multiple bytes starting from a particular register
void nrf24_WriteRegMulti(uint8_t Reg, uint8_t *data, int size) {
	uint8_t buf[2];
	buf[0] = Reg | 1 << 5; // move 1 to the fifth bit in buf[0]

	// Pull the CSN pin low to select the device
	CSN_Select();

	HAL_SPI_Transmit(NRF24_SPI, buf, 1, 100); // transmits the register address byte MOSI
	HAL_SPI_Transmit(NRF24_SPI, data, size, 1000);

	// Pull the CSN pin high to release device
	CSN_UnSelect();
}
// reads single byte from the register
uint8_t nrf24_ReadReg(uint8_t Reg) {

	uint8_t data = 0;

	// Pull the CSN pin low to select the device
	CSN_Select();

	HAL_SPI_Transmit(NRF24_SPI, &Reg, 1, 100); // send register address from which data is read
	HAL_SPI_Receive(NRF24_SPI, &data, 1, 100); // begins MISO, stores data collected at &data

	// Pull the CSN pin high to release device
	CSN_UnSelect();

	return data;
}
// reads multiple bytes from the register
void nrf24_ReadRegMulti(uint8_t Reg, uint8_t *data, int size) { // caller must initialize a pointer array *data

	// Pull the CSN pin low to select the device
	CSN_Select();

	HAL_SPI_Transmit(NRF24_SPI, &Reg, 1, 100); // send register address from which data is read
	HAL_SPI_Receive(NRF24_SPI, data, size, 1000); // begins MISO, stores data collected at &data

	// Pull the CSN pin high to release device
	CSN_UnSelect();
}
// sends single command byte to the NRF24L01
void nrfSendCmd(uint8_t cmd) {

	// Pull the CSN pin low to select the device
	CSN_Select();

	HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 100); // sends contents of buf over SPI

	// Pull the CSN pin high to release device
	CSN_UnSelect();
}
// initializes the NRF24, done alongside other initializations
void NRF24_Init(void) {
	// disable the chip before configuring the device
	CE_Disable();

	nrf24_WriteReg(CONFIG, 0); // will be configured later

	nrf24_WriteReg(EN_AA, 0); // No auto ACK

	nrf24_WriteReg(EN_RXADDR, 0); // Not enabling any data pipe right now

	nrf24_WriteReg(SETUP_AW, 0x03); // 5 bytes for the TX/RX address

	nrf24_WriteReg(SETUP_RETR, 0); // no transmission

	nrf24_WriteReg(RF_CH, 0); // Will be set up during TX/RX

	nrf24_WriteReg(RF_SETUP, 0x0E); // Power = 0db, data rate = 2Mbps

	// enable the chip after configuring the device
	CE_Enable();
}

// set up the TX mode

void NRF24_TxMode(uint8_t *Address, uint8_t channel) {
	// disable the chip before configuring the device
	CE_Disable();

	nrf24_WriteReg(RF_CH, channel); // select the channel

	nrf24_WriteRegMulti(TX_ADDR, Address, 5); // writes Address to TX_ADDR (register)

	// power up the device
	uint8_t config = nrf24_ReadReg(CONFIG);
	config = config | (1 << 1);
	nrf24_WriteReg(CONFIG, config);

	// enable the chip after configuring the device
	CE_Enable();
}

//transmit the data
uint8_t NRF24_Transmit(uint8_t *data) {

	uint8_t cmdtosend = 0;

	// select the device
	CSN_Select();

	// payload command
	cmdtosend = W_TX_PAYLOAD;
	HAL_SPI_Transmit(NRF24_SPI, &cmdtosend, 1, 100); // needs memory address of the command, reads 1 byte and sends it over SPI

	// send the payload
	HAL_SPI_Transmit(NRF24_SPI, data, 32, 1000); // maximum payload of 32 bytes

	// unselect the device
	CSN_UnSelect();

	HAL_Delay(1);

	uint8_t fifostatus = nrf24_ReadReg(FIFO_STATUS); // reads the FIFO status register and stores it in a variable

	if ((fifostatus & (1 << 4)) && (!(fifostatus & (1 << 3)))) { // if TX FIFO is empty, meaning the payload was transmitted successfully
		cmdtosend = FLUSH_TX;
		nrfSendCmd(cmdtosend);
		return 1;
	}

	return 0;
}

void NRF24_RxMode(uint8_t *Address, uint8_t channel) { // must be a pointer since its a 5 byte array
	// disable the chip before configuring the device
	CE_Disable();

	nrf24_WriteReg(RF_CH, channel); // select the channel

	uint8_t en_rxaddr = nrf24_ReadReg(EN_RXADDR); // returns current value stored in the register of the NRF24
	en_rxaddr = en_rxaddr | (1 << 1);
	nrf24_WriteReg(EN_RXADDR, en_rxaddr); // select data pipe 1 by setting first bit to 1, writing 0x02 would overwrite other bits

	nrf24_WriteRegMulti(RX_ADDR_P1, Address, 5); // write address (determined by user for communication) to RX P1 ADDR

	nrf24_WriteReg(RX_PW_P1, 32); // 32 byte payload size for pipe 1

	// power up the device in Rx mode
	uint8_t config = nrf24_ReadReg(CONFIG);
	config = config | (1 << 1) | (1 << 0);
	nrf24_WriteReg(CONFIG, config);

	// enable the chip after configuring the device
	CE_Enable();
}

uint8_t isDataAvailable(int pipenum) {
	uint8_t status = nrf24_ReadReg(STATUS);

	if ((status & (1 << 6)) && (status & (pipenum << 1))) {
		nrf24_WriteReg(STATUS, (1 << 6)); // NRF interprets 1 in 6th position as a clear
		return 1;
	}
	return 0;
}

void NRF24_Receive(uint8_t *data) {

	uint8_t cmdtosend = 0;

	// select the device
	CSN_Select();

	// payload command
	cmdtosend = R_RX_PAYLOAD; // tells NRF wants to read data out of its RX FIFO
	HAL_SPI_Transmit(NRF24_SPI, &cmdtosend, 1, 100); // needs memory address of the command, reads 1 byte and sends it over SPI

	// receive the payload
	HAL_SPI_Receive(NRF24_SPI, data, 32, 1000); // hspi tells SPI_Receive which SPI to use

	// unselect the device
	CSN_UnSelect();

	HAL_Delay(1);

	cmdtosend = FLUSH_RX;
	nrfSendCmd(cmdtosend);

}

