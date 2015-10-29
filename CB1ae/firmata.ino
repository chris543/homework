byte command;

void printTest(String test) {
	Serial.print(test);
	index = 1;
	Serial.print(HSB);
}

void executive_firmata(int dataLength) {

	if ((firmataData[COMMAND] & 0xF0) == START_SYSEX) {

		command = firmataData[COMMAND];

		switch (command)
		{
		case SET_PIN_VALUE:
			if (BROAD_IO[firmataData[1]].mode == PWM )
			{
				// HAL_UART_Transmit(huart, (uint8_t *)"FB", 2, 100);
				/* code */

				switch (firmataData[1])
				{
				case 0:
					// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, (GPIO_PinState)firmataData[2]);
					// Serial.println("motor0 F/B");
					digitalWrite(SDc0, firmataData[2]);
					// digitalWrite(SDc0, HIGH);

					// delay(10);
					break;
				case 1:
					// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, (GPIO_PinState)firmataData[2]);
					// Serial.println("motor1 F/B");
					digitalWrite(SDc1, firmataData[2]);

					break;
				case 2:
					// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, (GPIO_PinState)firmataData[2]);
					// Serial.println("motor2 F/B");
					digitalWrite(SDc2, firmataData[2]);

					break;
				case 3:
					// HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, (GPIO_PinState)firmataData[2]);
					// Serial.println("motor3 F/B");
					digitalWrite(SDc3, firmataData[2]);

					break;
				}
			}

			break;

		case SET_PIN_MODE:
			// digitalWrite(led, HIGH);

			BROAD_IO[firmataData[1]].mode = firmataData[2];

			// Serial.println("mode");
			// Serial.println(BROAD_IO[firmataData[1]].mode);

			if (BROAD_IO[firmataData[1]].mode == OUTPUT )
			{
				// HAL_UART_Transmit(huart, (uint8_t *)"OUTPUT", 6, 100);
				/* code */
			}
			else if (BROAD_IO[firmataData[1]].mode == PWM )
			{
				Serial.println("set mode DC");

				// HAL_UART_Transmit(huart, (uint8_t *)"PWM", 3, 100);
				switch (firmataData[1])
				{
				case 0:
					Serial.println("detach 0");

					servo0.detach();
					break;
				case 1:
					Serial.println("detach 1");
					servo1.detach();

					break;
				case 2:
					Serial.println("detach 2");
					servo2.detach();
					// digitalWrite(led, LOW);

					break;
				case 3:
					Serial.println("detach 3");
					servo3.detach();

					break;
				}
			}
			else if (BROAD_IO[firmataData[1]].mode == SERVO )
			{
				Serial.println("set mode servo");

				// HAL_UART_Transmit(huart, (uint8_t *)"SERVO", 5, 100);
				switch (firmataData[1])
				{
				case 0:
					Serial.println("attach servo0");

					servo0.attach(SD0);
					break;
				case 1:
					Serial.println("attach servo1");

					servo1.attach(SD1);

					break;
				case 2:
					Serial.println("attach servo2");

					servo2.attach(SD2);
					// digitalWrite(led, HIGH);

					break;
				case 3:
					Serial.println("attach servo3");

					servo3.attach(SD3);

					break;
				}
			}
			break;
		case START_SYSEX:
			switch (firmataData[1])
			{
			case REPORT_FIRMWARE:

				// uint8_t version[8] = {0x55, 0x08, 0xF0, 0x79, 0x12, 0x12, 0xf7, 0xe1};

				// HAL_UART_Transmit(huart, (uint8_t *)version, 8, 100);


				break;

			}
			break;
		}


	} else {
		command = firmataData[COMMAND] & 0xF0;


		switch (command)
		{
		case ANALOG_MESSAGE:
			Serial.println(BROAD_IO[firmataData[COMMAND] & 0x0F].mode);

			if (BROAD_IO[firmataData[COMMAND] & 0x0F].mode == PWM )
			{
				// HAL_UART_Transmit(huart, (uint8_t *)"PWM", 3, 100);

				uint8_t value;
				value = firmataData[LSB] + (firmataData[HSB] << 7); //LSB +HSB
				BROAD_IO[firmataData[COMMAND] & 0x0F].value = value;
				switch (firmataData[COMMAND] & 0x0F) {
				case 0:

					// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
					analogWrite(SD0, BROAD_IO[firmataData[COMMAND] & 0x0F].value);

					break;
				case 1:
					// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
					analogWrite(SD1, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
					break;
				case 2:
					// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
					analogWrite(SD2, BROAD_IO[firmataData[COMMAND] & 0x0F].value);

					break;
				case 3:
					// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
					analogWrite(SD3, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
					break;
				}
			}

			if (BROAD_IO[firmataData[COMMAND] & 0x0F].mode == SERVO )
			{
				// HAL_UART_Transmit(huart, (uint8_t *)"SERVO", 5, 100);

				uint8_t value;
				value = firmataData[LSB] + (firmataData[HSB] << 7); //LSB +HSB
				BROAD_IO[firmataData[COMMAND] & 0x0F].value = value;

				//---set servo limit 0.8ms(+60 degree) to 2.2ms(-60 degree). 1.5ms is 0 degree
				if (value >= 23)
				{
					BROAD_IO[firmataData[COMMAND] & 0x0F].value = 22;
				}
				else if (value <= 7)
				{
					BROAD_IO[firmataData[COMMAND] & 0x0F].value = 8;

				}
				switch (firmataData[COMMAND] & 0x0F) {
				case 0:
					// Serial.println("servo0");
					// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
					servo0.writeMicroseconds(BROAD_IO[firmataData[COMMAND] & 0x0F].value * 100);
					break;
				case 1:
					// Serial.println("servo1");
					// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
					servo1.writeMicroseconds(BROAD_IO[firmataData[COMMAND] & 0x0F].value * 100);
					break;
				case 2:
					// Serial.println("servo2");
					// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
					servo2.writeMicroseconds(BROAD_IO[firmataData[COMMAND] & 0x0F].value * 100);
					// Serial.println(BROAD_IO[firmataData[COMMAND] & 0x0F].value * 100);

					break;
				case 3:
					// Serial.println("servo3");
					// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
					servo3.writeMicroseconds(BROAD_IO[firmataData[COMMAND] & 0x0F].value * 100);
					break;
				}
			}


			break;
		default:
			break;
		}

	}



//-----------------------old---------------



	// if ((firmataData[COMMAND] & 0xF0) == START_SYSEX) {

	// 	command = firmataData[COMMAND];
	// 	switch (command)
	// 	{

	// 	case SET_PIN_VALUE:
	// 		if (BROAD_IO[firmataData[1]].mode == PWM )
	// 		{
	// 			// HAL_UART_Transmit(huart, (uint8_t *)"FB", 2, 100);
	// 			/* code */

	// 			switch (firmataData[1])
	// 			{
	// 			case 0:
	// 				// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, (GPIO_PinState)firmataData[2]);
	// 				// Serial.println("motor0 F/B");
	// 				digitalWrite(SDc0, firmataData[2]);
	// 		// digitalWrite(SDc0, HIGH);

	// 				// delay(10);
	// 				break;
	// 			case 1:
	// 				// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, (GPIO_PinState)firmataData[2]);
	// 				// Serial.println("motor1 F/B");
	// 				digitalWrite(SDc1, firmataData[2]);

	// 				break;
	// 			case 2:
	// 				// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, (GPIO_PinState)firmataData[2]);
	// 				// Serial.println("motor2 F/B");
	// 				digitalWrite(SDc2, firmataData[2]);

	// 				break;
	// 			case 3:
	// 				// HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, (GPIO_PinState)firmataData[2]);
	// 				// Serial.println("motor3 F/B");
	// 				digitalWrite(SDc3, firmataData[2]);

	// 				break;
	// 			}
	// 		}

	// 		break;

	// 	case SET_PIN_MODE:

	// 		BROAD_IO[firmataData[1]].mode = firmataData[2];

	// 			Serial.println("mode");

	// 			Serial.println(BROAD_IO[IO_PIN_MODE].mode);

	// 		if (BROAD_IO[IO_PIN_MODE].mode == OUTPUT )
	// 		{
	// 			// HAL_UART_Transmit(huart, (uint8_t *)"OUTPUT", 6, 100);
	// 			/* code */
	// 		}
	// 		else if (BROAD_IO[IO_PIN_MODE].mode == PWM )
	// 		{
	// 			Serial.println("set mode DC");

	// 			// HAL_UART_Transmit(huart, (uint8_t *)"PWM", 3, 100);
	// 			switch (firmataData[1])
	// 			{
	// 			case 0:
	// 			Serial.println("detach 0");

	// 				servo0.detach();
	// 				break;
	// 			case 1:
	// 			Serial.println("detach 1");
	// 				servo1.detach();

	// 				break;
	// 			case 2:
	// 			Serial.println("detach 2");
	// 				servo2.detach();

	// 				break;
	// 			case 3:
	// 			Serial.println("detach 3");
	// 				servo3.detach();

	// 				break;
	// 			}
	// 		}
	// 		else if (BROAD_IO[IO_PIN_MODE].mode == SERVO )
	// 		{
	// 			Serial.println("set mode servo");

	// 			// HAL_UART_Transmit(huart, (uint8_t *)"SERVO", 5, 100);
	// 			switch (firmataData[1])
	// 			{
	// 			case 0:
	// 				Serial.println("attach servo0");

	// 				servo0.attach(SD0);
	// 				break;
	// 			case 1:
	// 				Serial.println("attach servo1");

	// 				servo1.attach(SD1);

	// 				break;
	// 			case 2:
	// 				Serial.println("attach servo2");

	// 				servo2.attach(SD2);
	// 				digitalWrite(led, HIGH);

	// 				break;
	// 			case 3:
	// 				Serial.println("attach servo3");

	// 				servo3.attach(SD3);

	// 				break;
	// 			}
	// 		}

	// 		break;

	// 	case START_SYSEX:
	// 		switch (firmataData[1])
	// 		{
	// 		case REPORT_FIRMWARE:

	// 			// uint8_t version[8] = {0x55, 0x08, 0xF0, 0x79, 0x12, 0x12, 0xf7, 0xe1};

	// 			// HAL_UART_Transmit(huart, (uint8_t *)version, 8, 100);


	// 			break;

	// 		}


	// 		break;
	// 	}

	// } else {
	// 	command = firmataData[COMMAND] & 0xF0;

	// 	switch (command)
	// 	{
	// 	case ANALOG_MESSAGE:

	// 		if (BROAD_IO[firmataData[COMMAND] & 0x0F].mode == PWM )
	// 		{
	// 			// HAL_UART_Transmit(huart, (uint8_t *)"PWM", 3, 100);

	// 			uint8_t value;
	// 			value = firmataData[LSB] + (firmataData[HSB] << 7); //LSB +HSB
	// 			BROAD_IO[firmataData[COMMAND] & 0x0F].value = value;
	// 			switch (firmataData[COMMAND] & 0x0F) {
	// 			case 0:

	// 				// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
	// 				analogWrite(SD0, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
	// 				// analogWrite(SD0, temp += 10);

	// 				// Serial.println("pwm m0");
	// 				// Serial.println(BROAD_IO[firmataData[COMMAND] & 0x0F].value);

	// 				// analogWrite(SD0, BROAD_IO[firmataData[COMMAND] & 0x0F].value);

	// 				break;
	// 			case 1:
	// 				// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
	// 				analogWrite(SD1, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
	// 				break;
	// 			case 2:
	// 				// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
	// 				analogWrite(SD2, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
	// 				break;
	// 			case 3:
	// 				// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
	// 				analogWrite(SD3, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
	// 				break;
	// 			}
	// 		}

	// 		if (BROAD_IO[firmataData[COMMAND] & 0x0F].mode == SERVO )
	// 		{
	// 			// HAL_UART_Transmit(huart, (uint8_t *)"SERVO", 5, 100);

	// 			uint8_t value;
	// 			value = firmataData[LSB] + (firmataData[HSB] << 7); //LSB +HSB
	// 			BROAD_IO[firmataData[COMMAND] & 0x0F].value = value;

	// 			//---set servo limit 0.8ms(+60 degree) to 2.2ms(-60 degree). 1.5ms is 0 degree
	// 			if (value >= 23)
	// 			{
	// 				BROAD_IO[firmataData[COMMAND] & 0x0F].value = 22;
	// 			}
	// 			else if (value <= 7)
	// 			{
	// 				BROAD_IO[firmataData[COMMAND] & 0x0F].value = 8;

	// 			}
	// 			switch (firmataData[COMMAND] & 0x0F) {
	// 			case 0:
	// 				Serial.println("servo0");
	// 				// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
	// 				servo0.writeMicroseconds(BROAD_IO[firmataData[COMMAND] & 0x0F].value * 100);
	// 				break;
	// 			case 1:
	// 				Serial.println("servo1");
	// 				// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
	// 				servo1.writeMicroseconds(BROAD_IO[firmataData[COMMAND] & 0x0F].value * 100);
	// 				break;
	// 			case 2:
	// 				Serial.println("servo2");
	// 				// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
	// 				servo2.writeMicroseconds(BROAD_IO[firmataData[COMMAND] & 0x0F].value * 100);
	// 				// Serial.println(BROAD_IO[firmataData[COMMAND] & 0x0F].value * 100);

	// 				break;
	// 			case 3:
	// 				Serial.println("servo3");
	// 				// __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, BROAD_IO[firmataData[COMMAND] & 0x0F].value);
	// 				servo3.writeMicroseconds(BROAD_IO[firmataData[COMMAND] & 0x0F].value * 100);
	// 				break;
	// 			}
	// 		}

	// 		break;
	// 	case DIGITAL_MESSAGE:

	// 		break;
	// 	}

	// }

}