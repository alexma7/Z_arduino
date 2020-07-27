void read_serial()
{

	while (Serial.available() > 0)
	{
		//delay(1);
		serial_char = Serial.read();

	 	switch (serial_char) 
		{
			case 'i':
				serial_num = 1;
				continue;
			case 'c':
				// если id сообщенмя равен устройству, то записываем следующее значение
				if (id == arduino_id)
				{
					serial_num = 2;
				}	
				// иначе, мы обнуляем переменные и очищаем кэш сериала
				else
				{
					id = 0;
					while (Serial.available() > 0)
					{
						Serial.read();	
					}
				}	
				continue;
			case 't':
				serial_num = 3;
				continue;
			case 'n':
				serial_num = 4;
				continue;
			case 'v':
				serial_num = 5;
				continue;
		}


		switch (serial_num) 
		{
			case 1:
				serial_vars.id += serial_char;
				break;
			case 2:
				topic += serial_char;
				break;
			//default:
			//	return 0;
		}
	}
}