#include "rw_util.h"

#define NSP_MODULE_PING 0x00
#define NSP_MODULE_INIT 0x01
#define NSP_MODULE_PEEK 0x02
#define NSP_MODULE_POKE 0x03
#define NSP_MODULE_TELEMETRY 0x04
// No 0x05?
#define NSP_MODULE_CRC 0x06
#define NSP_MODULE_APP_TELEMETRY 0x07
#define NSP_MODULE_APP_COMMAND 0x08

#define NSP_MCF_POLL_MASK 0x80

// Should functions build messages each time? Seems like the safest
// Do I need to worry about mutual exclusion?
struct NSP_message {
	uint8 dest_addr;
	uint8 src_addr;
	uint8 mcf; // message control field
	uint8* data;
	uint8 data_len;
	uint16 crc; 
};

// TODO: Consider scope for all methods

// Adds a byte to data array, encodes in SLIP if neccessary
// returns the new count.
// Special characters in SLIP (Serial Line Internet Protocol)
#define SLIP_FEND  0xC0 // Marks end of message
#define SLIP_FESC  0xDB 
#define SLIP_TFEND 0xDC // Natural FENDs are escaped with FESC TFEND
#define SLIP_TFESC 0xDD // Natural FESCs are escaped with FESC TFESC
int add_slip_byte(int count, uint8 data_array[], uint8 byte) {
	if (byte == SLIP_FEND) {
		data_array[count] = SLIP_FESC;
		count++;
		data_array[count] = SLIP_TFEND;
		count++;
	} else if (byte == SLIP_FESC) {
		data_array[count] = SLIP_FESC;
		count++;
		data_array[count] = SLIP_TFESC;
		count++;
	} else {
		// no SLIP needed.
		data_array[count] = byte;
		count++;
	}

	return count;
}

int read_slip_byte(uint8 array[], uint8* byte) {
	if (array[0] == SLIP_FESC) {
		// FESC, followed either by a TFEND or a TFESC
		if (array[1] == SLIP_TFEND) {
			*byte = SLIP_FEND;
			return 2;
		} else if (array[1] == SLIP_TFESC) {
			*byte = SLIP_FESC;
			return 2;
		} else {
			// TODO: Handle Error
			printf("Error in read slip byte, FESC not followed by TFEND or TFEST\n");
			*byte = array[1];
			return 2;
		}
	} else {
		*byte = array[0];
		return 1;
	}
}

// frame's data is set to the maximum count
// TODO: Make sure the max isn't exceeded (I2C_MTU)
int send_nsp_message(struct NSP_message nsp_mesg) {
	int retval = 0, count = 0, index = 0;
	i2c_frame_t *frame = (i2c_frame_t *)csp_buffer_get(sizeof(i2c_frame_t));
	if (frame == NULL) {
		// out of memory
		// TODO: What to do now?
		return -42;
	}

	frame->retries = 0;
	frame->dest = nsp_mesg.dest_addr;
	count = add_slip_byte(count, frame->data, nsp_mesg.src_addr);
	count = add_slip_byte(count, frame->data, nsp_mesg.mcf);

	for (index = 0; index < nsp_mesg.data_len; index++) {
		count = add_slip_byte(count, frame->data, nsp_mesg.data[index]);
	}
	
	// Send CRC, 2 bytes
	count = add_slip_byte(count, frame->data, ((uint8 *)(&nsp_mesg.crc))[0]);
	count = add_slip_byte(count, frame->data, ((uint8 *)(&nsp_mesg.crc))[1]);
	
	frame->data[count] = SLIP_FEND;
	count++;

	frame->len = count;
	frame->len_rx = 0;

	retval = i2c_send(0, frame, 1000); // E_NO_ERR == -1
	if (retval != E_NO_ERR) {
		printf("RW: Send Failed\n");
		csp_buffer_free(frame); // Buffer not freed if failed
		return retval; // send failed
	}
	return retval;
}

// For messages that require a response
// Buffer should already be allocated.
// ret_nsp_mesg should already be fully allocated
// TODO: CAN ONLY READ 144 CHARACTERS (maybe less with SLIP)
int send_recv_nsp_message(struct NSP_message nsp_mesg, struct NSP_message *ret_nsp_mesg) {
	int retval = 0, count = 0, index = 0;

	// The recv_frame is allocated by i2c_receive
	i2c_frame_t *recv_frame = NULL;
	i2c_frame_t *send_frame = (i2c_frame_t *)csp_buffer_get(sizeof(i2c_frame_t));

	send_frame->retries = 0;
	send_frame->dest = nsp_mesg.dest_addr;
	count = add_slip_byte(count, send_frame->data, nsp_mesg.src_addr);
	count = add_slip_byte(count, send_frame->data, nsp_mesg.mcf);

	for (index = 0; index < nsp_mesg.data_len; index++) {
		count = add_slip_byte(count, send_frame->data, nsp_mesg.data[index]);
	}
	
	// Send CRC, 2 bytes
	count = add_slip_byte(count, send_frame->data, ((uint8 *)(&nsp_mesg.crc))[0]);
	count = add_slip_byte(count, send_frame->data, ((uint8 *)(&nsp_mesg.crc))[1]);
	
	send_frame->data[count] = SLIP_FEND;
	count++;

	send_frame->len = count;
	// This is ugly, but have to account for SLIP encoding
	send_frame->len_rx = ret_nsp_mesg->data_len * 2; 

	// Frees the buffer
	retval = i2c_send(0, send_frame, 1000);
	if (retval != E_NO_ERR) {
		printf("RW: Send Failed\n");
		csp_buffer_free(send_frame); // Buffer not freed if failed
		return retval; // send failed
	}

	// Allocates a buffer (you must free)
	retval = i2c_receive(0, &recv_frame, 1000);
	if (retval != E_NO_ERR) {
		printf("RW: Receive Failed\n");
		return retval; // receive failed
	}

	ret_nsp_mesg->dest_addr = nsp_mesg.src_addr;
	ret_nsp_mesg->src_addr = nsp_mesg.dest_addr;

	int frame_index = 0; // needed to un-SLIP.
	uint8 next_byte;
	frame_index += read_slip_byte(&(recv_frame->data[frame_index]), &next_byte);
	ret_nsp_mesg->mcf = next_byte;

	// Copy from recv_frame to the buffer
	index = 0;
	while (frame_index < recv_frame->len - 3) {
		if (index >= ret_nsp_mesg->data_len) {
			break;
		} else {
			frame_index += read_slip_byte(&(recv_frame->data[frame_index]), &next_byte);
			ret_nsp_mesg->data[index] = next_byte;
			index++;
		}
	}

	frame_index += read_slip_byte(&(recv_frame->data[frame_index]), &next_byte);
	((uint8 *)&(ret_nsp_mesg->crc))[0] = next_byte;
	frame_index += read_slip_byte(&(recv_frame->data[frame_index]), &next_byte);
	((uint8 *)&(ret_nsp_mesg->crc))[1] = next_byte;

	// free the buffer!
	csp_buffer_free(recv_frame);

	return retval;
}

// Message length is the header length plus the data length
#define NSP_HEADER_BYTES 3 // One for each destination address, source address, and message control field
#define POLY 0x8408 // x^16 + x^12 + x^5 + 1, reversed for LSB-first
uint16 generateCRC(int message_length, uint8* header, uint8* data) {
	uint16 crc = 0xFFFF;
	int index, jndex;
	
	// TODO: assert for message length
	for (index = 0; index < NSP_HEADER_BYTES; index++) {
		uint8 current_char = header[index];
    	for (jndex = 0; jndex < 8; jndex++) {
    		crc = (crc >> 1) ^ (((current_char ^ crc) & 0x01) ? POLY : 0);
    		current_char >>= 1;
		}
	}
	
	if (message_length > NSP_HEADER_BYTES) {
		for (index = 0; index < message_length - NSP_HEADER_BYTES; index++) {
			uint8 current_char = data[index];
			for (jndex = 0; jndex < 8; jndex++) {
				crc = (crc >> 1) ^ (((current_char ^ crc) & 0x01) ? POLY : 0);
				current_char >>= 1;
			}
		}
	}

	return crc;
}

int verifyCRC(struct NSP_message nsp_mesg) {
	uint16 actual_crc;
	actual_crc = generateCRC(3 + nsp_mesg.data_len, &(nsp_mesg.dest_addr), nsp_mesg.data);

	if ((((uint8 *)(&actual_crc))[0] == 
		 ((uint8 *)(&nsp_mesg.crc))[0]) &&
		(((uint8 *)(&actual_crc))[1] == 
		 ((uint8 *)(&nsp_mesg.crc))[1])) {
		return 0;
	} else {
		printf("RW: CRC of message: %x\n", nsp_mesg.crc);
		printf("RW: CRC calculated: %x\n", actual_crc);
		return -1;
	}
}

// turn on wheel
int rwlib_turnon(uint8 dest_addr) {
	int retval;
	// TODO: Well, anyone else using i2c? Likely should init elsewhere
	// handle, mode, address, speed, trans queue len, rec queue len, receive callback
	static int i2c_init_count = 0; // should only init once // TODO: Fix this. Inited right now by the EPS code.
	if (i2c_init_count < 1) {
		retval = i2c_init(0, I2C_MASTER, 0x11, 100, 10, 10, NULL); // response function?
		if (retval != E_NO_ERR) {
			// Bail if init fails
			return retval;
		}

		i2c_init_count++;
	}

	// Data - 0x00100000 for starting the wheel
	uint8 init_start_data[4];
	init_start_data[0] = 0x00;
	init_start_data[1] = 0x10;
	init_start_data[2] = 0x00;
	init_start_data[3] = 0x00;

	struct NSP_message nsp_mesg;
	nsp_mesg.dest_addr = dest_addr;
	nsp_mesg.src_addr = 0x11;
	nsp_mesg.mcf = NSP_MODULE_INIT;
	nsp_mesg.data_len = 4;
	nsp_mesg.data = init_start_data;
	nsp_mesg.crc = generateCRC(3 + 4, &(nsp_mesg.dest_addr), nsp_mesg.data);

	retval = send_nsp_message(nsp_mesg);

	return retval;
}

// turn off wheel
int rwlib_turnoff(uint8 dest_addr) {
	int retval;

	struct NSP_message nsp_mesg;
	nsp_mesg.dest_addr = dest_addr;
	nsp_mesg.src_addr = 0x11;
	nsp_mesg.mcf = NSP_MODULE_INIT;
	nsp_mesg.data_len = 0;
	nsp_mesg.crc = generateCRC(3, &(nsp_mesg.dest_addr), NULL);

	retval = send_nsp_message(nsp_mesg);

	return retval;
}

int rwlib_receiveModeTelem(uint8 dest_addr, uint8 *mode_type, float *mode_value) {
	int retval = 0;
	uint8 command = 0;

	struct NSP_message nsp_mesg;
	nsp_mesg.dest_addr = dest_addr;
	nsp_mesg.src_addr = 0x11;
	nsp_mesg.mcf = NSP_MCF_POLL_MASK | NSP_MODULE_APP_TELEMETRY;
	nsp_mesg.data_len = 1;
	nsp_mesg.data = &command;
	nsp_mesg.crc = generateCRC(3 + 1, &(nsp_mesg.dest_addr), nsp_mesg.data);

    uint8 recv_bytes[6];
	struct NSP_message nsp_recv_mesg;
	nsp_recv_mesg.data = recv_bytes;
	nsp_recv_mesg.data_len = 6;

	retval = send_recv_nsp_message(nsp_mesg, &nsp_recv_mesg);

	if (retval != E_NO_ERR) {
		return retval; // Something went wrong.
	}

	if (verifyCRC(nsp_recv_mesg) < 0) {
		printf("RW: CRC check failed.\n");
	}

	*mode_type = nsp_recv_mesg.data[1];
	uint8 *floating_address = (uint8 *) mode_value;
	floating_address[0] =  nsp_recv_mesg.data[2];
	floating_address[1] =  nsp_recv_mesg.data[3];
	floating_address[2] =  nsp_recv_mesg.data[4];
	floating_address[3] =  nsp_recv_mesg.data[5];

	return retval;
}

int rwlib_receiveParamTelem(uint8 dest_addr, uint8 *param_number, float *param_value) {
	int retval = 0;

	struct NSP_message nsp_mesg;
	nsp_mesg.dest_addr = dest_addr;
	nsp_mesg.src_addr = 0x11; // TODO: define
	nsp_mesg.mcf = NSP_MCF_POLL_MASK | NSP_MODULE_APP_TELEMETRY;
	nsp_mesg.data_len = 1;
	nsp_mesg.data = param_number;
	nsp_mesg.crc = generateCRC(3 + 1, &(nsp_mesg.dest_addr), nsp_mesg.data);

	uint8 recv_bytes[5];
	struct NSP_message nsp_recv_mesg;
	nsp_recv_mesg.data = recv_bytes;
	nsp_recv_mesg.data_len  = 5;

	retval = send_recv_nsp_message(nsp_mesg, &nsp_recv_mesg);

	*param_number = nsp_recv_mesg.data[0];
	uint8 *floating_address = (uint8 *) param_value;
	floating_address[0] =  nsp_recv_mesg.data[1];
	floating_address[1] =  nsp_recv_mesg.data[2];
	floating_address[2] =  nsp_recv_mesg.data[3];
	floating_address[3] =  nsp_recv_mesg.data[4];

	return retval;
}

int rwlib_sendModeCommand(uint8 dest_addr, uint8 mode_number, float mode_value) {
	int retval = 0;

	struct NSP_message nsp_mesg;
	nsp_mesg.dest_addr = dest_addr;
	nsp_mesg.src_addr = 0x11;
	nsp_mesg.mcf = NSP_MODULE_APP_COMMAND;
	
	uint8 data_buffer[6];
	data_buffer[0] = 0; // indicates a mode command
	data_buffer[1] = mode_number;
	uint8* floating_address = (uint8 *) &mode_value;
	data_buffer[2] = floating_address[0];
	data_buffer[3] = floating_address[1];
	data_buffer[4] = floating_address[2];
	data_buffer[5] = floating_address[3];

	nsp_mesg.data_len = 6;
	nsp_mesg.data = data_buffer;

	nsp_mesg.crc = generateCRC(3 + 6, &(nsp_mesg.dest_addr), nsp_mesg.data);

	retval = send_nsp_message(nsp_mesg);

	return retval;
}

int rwlib_sendParamCommand(uint8 dest_addr, uint8 param_number, float param_value) {
	int retval = 0;

	struct NSP_message nsp_mesg;
	nsp_mesg.dest_addr = dest_addr;
	nsp_mesg.src_addr = 0x11;
	nsp_mesg.mcf = NSP_MODULE_APP_COMMAND;

	uint8 data_buffer[5];
	data_buffer[0] = param_number;
	uint8* floating_address = (uint8 *) &param_value;
	data_buffer[1] = floating_address[0];
	data_buffer[2] = floating_address[1];
	data_buffer[3] = floating_address[2];
	data_buffer[4] = floating_address[3];

	nsp_mesg.data_len = 5;
	nsp_mesg.data = data_buffer;

	nsp_mesg.crc = generateCRC(3 + 5, &(nsp_mesg.dest_addr), nsp_mesg.data);

	retval = send_nsp_message(nsp_mesg);

	return retval;
}

// Ping
// PING DOES NOT WORK: Only seem to be able to read 144 characters.
int rwlib_ping(uint8 dest_addr) {
	int retval;

	struct NSP_message nsp_mesg;
	nsp_mesg.dest_addr = dest_addr;
	nsp_mesg.src_addr = 0x11; // Should be a define somewhere
	nsp_mesg.mcf = NSP_MCF_POLL_MASK | NSP_MODULE_PING;
	nsp_mesg.data_len = 0;
	// TODO: remove the magic 3
	nsp_mesg.crc = generateCRC(3, &(nsp_mesg.dest_addr), NULL);

	uint8 recv_bytes[200 ];
	struct NSP_message nsp_recv_mesg;
	nsp_recv_mesg.data = recv_bytes;
	nsp_recv_mesg.data_len = 200;

	retval = send_recv_nsp_message(nsp_mesg, &nsp_recv_mesg);

	if (retval != E_NO_ERR) {
		return retval; // Something went wrong.
	}

	if (verifyCRC(nsp_recv_mesg) < 0) {
		printf("RW: CRC check failed.\n");
	}

	printf("RW: Ping? %s\n", nsp_recv_mesg.data);
	printf("RW: Ping? %s\n", &(nsp_recv_mesg.data[100]));

	return retval;	
}
