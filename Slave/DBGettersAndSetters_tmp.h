
/***********************************************************************************************************
*
*	created by: MPZinke
*	on ..
*
*	DESCRIPTION: As par https://techtutorialsx.com/2017/05/20/esp32-http-post-requests/
*	BUGS:
*	FUTURE:
*
***********************************************************************************************************/



#ifndef _ETHERNET_COMM_
#define _ETHERNET_COMM_


	// Reads the incoming transmission from the response.
	// Takes pointer to the buffer where the scraped data will be stored.
	// Checks that the data transmission is of the correct format.
	bool read_data(byte buffer[])
	{
		while(!CLIENT.available());  // wait for reponse
		if(first_line_is_invalid()) return false;

		while(CLIENT.available() && CLIENT.read() != '{');  // ignore 'Content-<Type>'s

		if(CLIENT.available() != BUFFER_SIZE) return false;  // EOF & no JSON found (<{>, <13 chars>, <}}>, <\n>)
		CLIENT.read(buffer, BUFFER_SIZE);  // if JSON is longer than BUFFER, then you need to change something

		while(CLIENT.available()) CLIENT.read();  // clear remaining characters

		return *buffer == '{' && buffer[BUFFER_SIZE-2] == '}' && buffer[BUFFER_SIZE-1] == '}';
	}

	// Checks whether server is connected.
	// If not, continually tries to connect to server.
	void ensure_connection()
	{
		while(!CLIENT.connected()) 
		{
			#ifdef _TESTING_
				Serial.println(DEF_SPC_STR(__LINE__, No connection));
			#endif
			delay(1000);
			CLIENT.connect(SERVER, 80);
		}
	}


	// ————————————————— GETTERS —————————————————

	// Sends data using POST method to HOST.
	// Takes char array of data to post.  Prints data to client.
	void post_data(String data)
	{
		ensure_connection();

		CLIENT.println(STR_SPC_DEF_SPC_STR(POST, PAGE, HTTP/1.1));
		CLIENT.println(STR_SPC_DEF(Host:, HOST));
		CLIENT.println("Content-Type: application/x-www-form-urlencoded");
		CLIENT.print("Content-Length: ");
		CLIENT.println(data.length());
		CLIENT.println();
		CLIENT.print(data);
		CLIENT.println();
	}


	// Sends data using POST method to HOST.
	// Takes char array of data to post.  Prints data to client.
	void post_data(char data[])
	{
		ensure_connection();

		CLIENT.println(STR_SPC_DEF_SPC_STR(POST, PAGE, HTTP/1.1));
		CLIENT.println(STR_SPC_DEF(Host:, HOST));
		CLIENT.println("Content-Type: application/x-www-form-urlencoded");
		CLIENT.print("Content-Length: ");
		CLIENT.println(data.length());
		CLIENT.println();
		CLIENT.print(data);
		CLIENT.println();
	}


	// Compare first line of response with known response to see if successful.
	// Uses defined string to compare each character of response for accuracty.
	// Returns if they do not match.
	bool first_line_is_invalid()
	{
		char first_line[] = VALID_RESPONSE_STR;
		for(uint8_t x = 0; x < sizeof(first_line)-1; x++) if(first_line[x] != CLIENT.read()) return true;
		return false;
	}


	bool set_curtain_length(uint32_t length)
	{
		char post_string[] = STR_STR_DEF(set_curtain_length=001, &curtain=, CURTAIN_NUMBER);
		post_string[19] = length >> 14;
		post_string[20] = (length >> 7) & 0xFF;
		post_string[21] = length & 0xFF;
		post_data(post_string);
	}


	bool set_curtain_activated()
	{
		post_data(STR_DEF_STR(curtain=, CURTAIN_NUMBER, &activation_state=1));
	}


	bool set_curtain_as_deactivated()
	{
		post_data(STR_DEF_STR(curtain=, CURTAIN_NUMBER, &activation_state=0));
	}

#endif