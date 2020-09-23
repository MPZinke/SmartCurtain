


#include <SPI.h>
#include <Ethernet.h>


#define _TESTING_

#define HOST 10.0.0.11
#define PAGE /test.php

// ———— TRANSMISSION & DECODING ————
// designed for {{1111111111111}}, {{<1> <3> <3> <3> <3>}}
// {{<1 modes>  <3 c_len> <3 c_pos> <3 e_id> <3 e_pos>}}
#define BUFFER_SIZE 16  // number of characters to copy into buffer for response
#define VALID_RESPONSE_STR "HTTP/1.1 200 OK"  // initial string for valid response from device

#define SINGLE_SHIFT 7  // number of bits to shift byte over by (BASE = 2^X) EG (128 = 2^7)
#define DOUBLE_SHIFT 14  // SINGLE_SHIFT doubled

#define MODES_INDEX 1
#define AUTO_CALIBRATE_MASK 0
#define AUTO_CORRECT_MASK 1
#define DIRECTION_MASK 2
#define C_LENGTH_INDEX 2
#define C_POSITION_INDEX 5
#define E_ID_INDEX 8
#define E_POSITION_INDEX 11
#define END_ENCODING 14

// ———— CONCAT STRING TO CURTAIN_NUMBER ———— 
#define DEF(x) #x
#define DEF_SPC_STR(x, y) DEF(x) " " #y
#define STR_DEF(x, y) #x DEF(y)
#define STR_SPC_DEF(x, y) #x " " DEF(y)
#define STR_SPC_DEF_SPC_STR(x, y, z) #x " " DEF(y) " " #z
#define STR_DEF_STR(x, y, z) #x DEF(y) #z


byte MAC[] = {0xDE, 0x43, 0x52, 0x54, 0x4E, 0x31};  // {'Z', 'C', 'R', 'T', 'N', '1'}
IPAddress SERVER(10,0,0,11);  // HARDCODED: connection endpoint from where response is saught
EthernetClient CLIENT;  // the magician


typedef struct CURTAIN_DETAILS
{
	bool auto_calibrate;  // if the curtain has opportunity to move full span, count steps & return value
	bool auto_correct;  // if position is unexpected, go to expected position
	bool curtain_direction;
	long curtain_length;
	long curtain_position;
} CURTAIN_DETAILS;



void setup() 
{
	Ethernet.init();  // defaults to 10 (Teensy 3.2, etc)
	while(!Ethernet.begin(MAC)) delay(1000);  // wait while not connected to LAN
	while(!CLIENT.connect(SERVER, 80)) delay(1000);  // wait while not connected to device

	delay(1000);
}


void loop() 
{
	byte buffer[BUFFER_SIZE];

	ensure_connection();

	String data = "value=11";
	post_data(data);

	if(!read_data(buffer))
	{
		Serial.write(buffer, BUFFER_SIZE);
		Serial.println();
		Serial.println(DEF_SPC_STR(__LINE__, Invalid response));
		while(CLIENT.available()) CLIENT.read();	
		delay(500);
	}
	else
	{
		Serial.write(buffer, BUFFER_SIZE);  // TESTING
		Serial.println();

		uint32_t c_length = value_for_next_three_bytes(buffer, C_LENGTH_INDEX);
		uint32_t c_position = value_for_next_three_bytes(buffer, C_POSITION_INDEX);
		uint32_t e_length = value_for_next_three_bytes(buffer, E_ID_INDEX);
		uint32_t e_position = value_for_next_three_bytes(buffer, E_POSITION_INDEX);

		Serial.print("c_length: ");
		Serial.println(c_length);
		Serial.print("c_position: ");
		Serial.println(c_position);
		Serial.print("e_length: ");
		Serial.println(e_length);
		Serial.print("e_position: ");
		Serial.println(e_position);
	}
	// while (CLIENT.available())
	// {
	// 	char c = CLIENT.read();
	// 	Serial.write(c);
	// 	Serial.flush();
	// }


	delay(1500);
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


// Sends data using POST method to HOST.
// Takes char array of data to post.  Prints data to client.
void post_data(String data)
{
	CLIENT.println(STR_SPC_DEF_SPC_STR(POST, PAGE, HTTP/1.1));
	CLIENT.println(STR_SPC_DEF(Host:, HOST));
	CLIENT.println("Content-Type: application/x-www-form-urlencoded");
	CLIENT.print("Content-Length: ");
	CLIENT.println(data.length());
	CLIENT.println();
	CLIENT.print(data);
	CLIENT.println();
}


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


// Compare first line of response with known response to see if successful.
// Uses defined string to compare each character of response for accuracty.
// Returns if they do not match.
bool first_line_is_invalid()
{
	char first_line[] = VALID_RESPONSE_STR;
	for(uint8_t x = 0; x < sizeof(first_line)-1; x++) if(first_line[x] != CLIENT.read()) return true;
	return false;
}


// Converts next 3 bytes into single 32 bit int.
// Takes buffer array of scraped data, starting offset of value.
// Takes each byte sequentially & masks it into its corresponding 7 bit (2^7=128) section.
// Returns 32 bit int mapped value.
uint32_t value_for_next_three_bytes(byte buffer[], uint8_t start)
{
	return ((buffer[start]-1) << 14) | ((buffer[start+1]-1) << 7) | (buffer[start+2]-1);
}