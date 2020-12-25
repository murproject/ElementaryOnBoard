#include <Arduino.h>
#include "Motors.h"

#define MOTOR_LEFT_TH_PIN 2
#define MOTOR_VERT_TH_PIN 3
#define MOTOR_RIGHT_TH_PIN 4
#define MOTOR_VERT_TH_2_PIN 7

#define MOTOR_L_PIN 5
#define MOTOR_R_PIN 6

BrushlessMotor motorLeft(MOTOR_LEFT_TH_PIN);
BrushlessMotor motorVert(MOTOR_VERT_TH_PIN);
BrushlessMotor motorRight(MOTOR_RIGHT_TH_PIN);
BrushlessMotor motorVert2(MOTOR_VERT_TH_2_PIN);

BrushMotor motorBrush(MOTOR_L_PIN, MOTOR_R_PIN);

unsigned char Crc8(uint8_t *pcBlock, unsigned int len) {
    unsigned char crc = 0xFF;
    unsigned int i;

    while (len--)
    {
        crc ^= *pcBlock++;

        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }

    return crc;
}

void setup() {
	Serial.begin(1200);

	motorLeft.init();
	motorVert.init();
	motorRight.init();
	motorVert2.init();

	delay(2000);
}

#define START_BYTE 0xAA
#define END_BYTE 0xEF

void loop() {
	if (Serial.available()) {
		uint8_t first_received_byte = 0;
		Serial.readBytes(&first_received_byte, 1);
		if (first_received_byte != START_BYTE)
			return;

		uint8_t buffer[8] = {};
		buffer[0] = first_received_byte;

		Serial.readBytes(buffer + 1, 8);

		if (buffer[7] == Crc8(buffer + 2, 5) &&
			buffer[8] == END_BYTE) {

			int8_t left_th, vert_th_1, right_th, vert_th_2, add;
			left_th 	= (int8_t)buffer[2];
			vert_th_1 	= (int8_t)buffer[3];
			right_th 	= (int8_t)buffer[4];
			vert_th_2 	= (int8_t)buffer[5];
			add 		= (int8_t)buffer[6];

			motorLeft.set_power(left_th);
			motorVert.set_power(vert_th_1);
			motorRight.set_power(right_th);
			motorVert2.set_power(vert_th_2);
			motorBrush.set_power(add);
		}
        else {
			delay(10);
		}
	}
}
