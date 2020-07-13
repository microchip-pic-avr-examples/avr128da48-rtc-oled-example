/**
 * \file
 *
 * \brief SPI basic driver.
 *
 (c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/**
 * \defgroup doc_driver_spi_basic SPI Basic
 * \ingroup doc_driver_spi
 *
 * \section doc_driver_spi_basic_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/builtins.h>
#include "spi_basic.h"
#include "atmel_start_pins.h"


typedef struct {
	// hardware stuff that controls SPI mode
	// hardware stuff that controls SPI baud rate
	uint8_t CTRLAvalue;
	uint8_t CTRLBvalue;
} SPI_MASTER_configuration_t;

typedef struct SPI_MASTER_descriptor_s {
	spi_transfer_status_t status;
} SPI_MASTER_descriptor_t;

SPI_MASTER_configuration_t SPI_MASTER_configurations[] = {
    {

        .CTRLAvalue = 0 << SPI_CLK2X_bp    /* Enable Double Speed: disabled */
                      | 0 << SPI_DORD_bp   /* Data Order Setting: disabled */
                      | 1 << SPI_ENABLE_bp /* Enable Module: enabled */
                      | 1 << SPI_MASTER_bp /* SPI module in master mode */
                      | SPI_PRESC_DIV4_gc  /* System Clock / 4 */

        ,

        .CTRLBvalue = 0 << SPI_BUFEN_bp   /* Buffer Mode Enable: disabled */
                      | 0 << SPI_BUFWR_bp /* Buffer Write Mode: disabled */
                      | SPI_MODE_0_gc     /* SPI Mode 0 */
                      | 0 << SPI_SSD_bp   /* Slave Select Disable: disabled */

    },
};

static SPI_MASTER_descriptor_t SPI_MASTER_desc;

/**
 * \brief Initialize SPI interface
 * If module is configured to disabled state, the clock to the SPI is disabled
 * if this is supported by the device's clock system.
 *
 * \return Initialization status.
 * \retval 0 the SPI init was successful
 * \retval 1 the SPI init was not successful
 */
void SPI_MASTER_init()
{

	SPI0.CTRLA = 0 << SPI_CLK2X_bp    /* Enable Double Speed: disabled */
	             | 0 << SPI_DORD_bp   /* Data Order Setting: disabled */
	             | 1 << SPI_ENABLE_bp /* Enable Module: enabled */
	             | 1 << SPI_MASTER_bp /* SPI module in master mode */
	             | SPI_PRESC_DIV4_gc; /* System Clock / 4 */

	// SPI0.CTRLB = 0 << SPI_BUFEN_bp /* Buffer Mode Enable: disabled */
	//		 | 0 << SPI_BUFWR_bp /* Buffer Write Mode: disabled */
	//		 | SPI_MODE_0_gc /* SPI Mode 0 */
	//		 | 0 << SPI_SSD_bp; /* Slave Select Disable: disabled */

	// SPI0.INTCTRL = 0 << SPI_DREIE_bp /* Data Register Empty Interrupt Enable: disabled */
	//		 | 0 << SPI_IE_bp /* Interrupt Enable: disabled */
	//		 | 0 << SPI_RXCIE_bp /* Receive Complete Interrupt Enable: disabled */
	//		 | 0 << SPI_SSIE_bp /* Slave Select Trigger Interrupt Enable: disabled */
	//		 | 0 << SPI_TXCIE_bp; /* Transfer Complete Interrupt Enable: disabled */

	SPI_MASTER_desc.status = SPI_FREE;
}

/**
 * \brief Enable SPI_MASTER
 * 1. If supported by the clock system, enables the clock to the SPI
 * 2. Enables the SPI module by setting the enable-bit in the SPI control register
 *
 * \return Nothing
 */
void SPI_MASTER_enable()
{
	SPI0.CTRLA |= SPI_ENABLE_bm;
}

/**
 * \brief Disable SPI_MASTER
 * 1. Disables the SPI module by clearing the enable-bit in the SPI control register
 * 2. If supported by the clock system, disables the clock to the SPI
 *
 * \return Nothing
 */
void SPI_MASTER_disable()
{
	SPI0.CTRLA &= ~SPI_ENABLE_bm;
}

/**
 * \brief Open the SPI SPI_MASTER for communication
 *
 * \param[in] configuration The configuration to use in the transfer
 *
 * \return Initialization status.
 * \retval false The SPI open was successful
 * \retval true  The SPI open was successful
 */
bool SPI_MASTER_open(SPI_MASTER_configuration_name configuration)
{
	if (SPI_MASTER_desc.status == SPI_FREE) {
		SPI_MASTER_desc.status = SPI_IDLE;
		SPI0.CTRLA             = SPI_MASTER_configurations[configuration].CTRLAvalue;
		SPI0.CTRLB             = SPI_MASTER_configurations[configuration].CTRLBvalue;
		return true;
	} else {
		return false;
	}
}

/**
 * \brief Close the SPI SPI_MASTER for communication
 *
 * \return Nothing
 */
void SPI_MASTER_close(void)
{
	SPI_MASTER_desc.status = SPI_FREE;
}

/**
 * \brief Exchange one byte over SPI SPI_MASTER. Blocks until done.
 *
 * \param[in] data The byte to transfer
 *
 * \return Received data byte.
 */
uint8_t SPI_MASTER_exchange_byte(uint8_t data)
{
	// Blocking wait for SPI free makes the function work
	// seamlessly also with IRQ drivers.
	while (SPI_MASTER_desc.status == SPI_BUSY)
		;
	SPI0.DATA = data;
	while (!(SPI0.INTFLAGS & SPI_RXCIF_bm))
		;
	return SPI0.DATA;
}

/**
 * \brief Exchange a buffer over SPI SPI_MASTER. Blocks if using polled driver.
 *
 * \param[inout] block The buffer to transfer. Received data is returned here.
 * \param[in] size The size of buffer to transfer
 *
 * \return Nothing.
 */
void SPI_MASTER_exchange_block(void *block, uint8_t size)
{
	uint8_t *b = (uint8_t *)block;
	while (size--) {
		SPI0.DATA = *b;
		while (!(SPI0.INTFLAGS & SPI_RXCIF_bm))
			;
		*b = SPI0.DATA;
		b++;
	}
}

/**
 * \brief Write a buffer over SPI SPI_MASTER. Blocks if using polled driver.
 *
 * \param[in] block The buffer to transfer
 * \param[in] size The size of buffer to transfer
 *
 * \return Nothing.
 */
void SPI_MASTER_write_block(void *block, uint8_t size)
{
	uint8_t *b = (uint8_t *)block;
	while (size--) {
		SPI0.DATA = *b;
		while (!(SPI0.INTFLAGS & SPI_RXCIF_bm))
			;
		b++;
	}
}

/**
 * \brief Read a buffer over SPI SPI_MASTER. Blocks if using polled driver.
 *
 * Zeros are transmitted out of the SPI.
 *
 * \param[out] block Received data is written here.
 * \param[in] size The size of buffer to transfer
 *
 * \return Nothing.
 */
void SPI_MASTER_read_block(void *block, uint8_t size)
{
	uint8_t *b = (uint8_t *)block;
	while (size--) {
		SPI0.DATA = 0;
		while (!(SPI0.INTFLAGS & SPI_RXCIF_bm))
			;
		*b = SPI0.DATA;
		b++;
	}
}

void SPI_MASTER_initialization(void)
{

    // Set pin direction to input
    PA5_set_dir(PORT_DIR_IN);

    PA5_set_pull_mode(
    // <y> Pull configuration
    // <id> pad_pull_config
    // <PORT_PULL_OFF"> Off
    // <PORT_PULL_UP"> Pull-up
    PORT_PULL_OFF);

    // Set pin direction to output
    PA4_set_dir(PORT_DIR_OUT);

    PA4_set_level(
    // <y> Initial level
    // <id> pad_initial_level
    // <false"> Low
    // <true"> High
    false);

    // Set pin direction to output
    PA6_set_dir(PORT_DIR_OUT);

    PA6_set_level(
    // <y> Initial level
    // <id> pad_initial_level
    // <false"> Low
    // <true"> High
    false);

    SPI_MASTER_init();
}