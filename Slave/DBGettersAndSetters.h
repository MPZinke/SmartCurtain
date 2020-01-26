
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


#ifndef DB_COMMUNICATION_H
#define DB_COMMUNICATION_H

	#include<HTTPClient.h>


	long read_next_number_from_json(const char json[], bool reset=0)
	{
		static char json_pos = 0;  // max string size as 255 chars
		if(reset) json_pos = 0;  // reset the json_pos if json parsing is finished for current main loop

		register char json_iter = json_pos;
		char number[18] = {};
		while(json[json_iter] && json[json_iter] != ':') json_iter++;
		json_iter += 1;
		register char num_iter = 0;
		while(json[json_iter] != ',' && json[json_iter] != '}') number[num_iter++] = json[json_iter++];
		json_pos = json_iter;
		return atol(number);
	}


	// ————————————————— GETTERS —————————————————


	bool auto_calibration_option()
	{
		HTTPClient client;
		client.begin(REQUEST_ADDRESS);
		client.addHeader("Content-Type", "application/x-www-form-urlencoded");
		char response = client.POST(STR(auto_calibration_option=, CURTAIN_NUMBER));

		if(response == (char)200)
		{
			const char* json_c_str = client.getString().c_str();
			client.end();
			return (bool)read_next_number_from_json(json_c_str, true);
		}
		client.end();
		return false;
	}


	bool auto_correct_option()
	{
		HTTPClient client;
		client.begin(REQUEST_ADDRESS);
		client.addHeader("Content-Type", "application/x-www-form-urlencoded");
		char response = client.POST(STR(auto_correct_option=, CURTAIN_NUMBER));

		if(response == (char)200)
		{
			const char* json_c_str = client.getString().c_str();
			client.end();
			return (bool)read_next_number_from_json(json_c_str, true);
		}
		client.end();
		return false;
	}


	void get_curtain_details(CURTAIN_DETAILS* details)
	{
		HTTPClient client;
		client.begin(REQUEST_ADDRESS);
		client.addHeader("Content-Type", "application/x-www-form-urlencoded");
		char response = client.POST(STR(get_min_curtain_details=, CURTAIN_NUMBER));

		if(response == (char)200)
		{
			const char* json_c_str = client.getString().c_str();
			details->curtain_direction = (bool)read_next_number_from_json(json_c_str, true);
			details->curtain_length = read_next_number_from_json(json_c_str);
			details->curtain_position = read_next_number_from_json(json_c_str);
		}
		else
		{
			details->curtain_direction = 0;
			details->curtain_length = 0;
			details->curtain_position = 0;
		}
		client.end();
	}


	void get_most_recent_event(EVENT* event)
	{
		HTTPClient client;
		client.begin(REQUEST_ADDRESS);
		client.addHeader("Content-Type", "application/x-www-form-urlencoded");
		char response = client.POST(STR(get_most_recent_event=, CURTAIN_NUMBER));

		if(response == (char)200)
		{
			const char* json_c_str = client.getString().c_str();
			event->event_id = read_next_number_from_json(json_c_str, true);
			event->event_position = read_next_number_from_json(json_c_str);
		}
		else
		{
			event->event_id = 0;
			event->event_position = 0;
		}
		client.end();
	}


	// ————————————————— SETTERS —————————————————

	bool reset_curtain_length(long length)
	{
		HTTPClient client;
		client.begin(REQUEST_ADDRESS);
		client.addHeader("Content-Type", "application/x-www-form-urlencoded");
		char response = client.POST(STR_LNG(	reset_curtain_length=, 
													CURTAIN_NUMBER,
													&new_length=) + String(length));

		if(response == (char)200)
		{
			const char* json_c_str = client.getString().c_str();
			client.end();
			return (bool)read_next_number_from_json(json_c_str, true);
		}
		client.end();
		return false;
	}


	bool set_curtain_as_activated()
	{
		HTTPClient client;
		client.begin(REQUEST_ADDRESS);
		client.addHeader("Content-Type", "application/x-www-form-urlencoded");
		int response = client.POST(STR_LNG(	set_curtain_activated_state=, 
													CURTAIN_NUMBER,
													&activation_state=1));

		if(response == (char)200)
		{
			const char* json_c_str = client.getString().c_str();
			client.end();
			return (bool)read_next_number_from_json(json_c_str, true);
		}
		client.end();
		return false;
	}


	bool set_curtain_as_deactivated()
	{
		HTTPClient client;
		client.begin(REQUEST_ADDRESS);
		client.addHeader("Content-Type", "application/x-www-form-urlencoded");
		int response = client.POST(STR_LNG(	set_curtain_activated_state=, 
													CURTAIN_NUMBER,
													&activation_state=0));

		if(response == (char)200)
		{
			const char* json_c_str = client.getString().c_str();
			client.end();
			return (bool)read_next_number_from_json(json_c_str, true);
		}
		client.end();
		return false;
	}


	bool set_curtain_as_closed()
	{
		HTTPClient client;
		client.begin(REQUEST_ADDRESS);
		client.addHeader("Content-Type", "application/x-www-form-urlencoded");
		int response = client.POST(STR(set_curtain_as_closed=, CURTAIN_NUMBER));

		if(response == (char)200)
		{
			const char* json_c_str = client.getString().c_str();
			client.end();
			return (bool)read_next_number_from_json(json_c_str, true);
		}
		client.end();
		return false;
	}


	bool set_curtain_as_open()
	{
		HTTPClient client;
		client.begin(REQUEST_ADDRESS);
		client.addHeader("Content-Type", "application/x-www-form-urlencoded");
		char response = client.POST(STR(set_curtain_as_open=, CURTAIN_NUMBER));

		if(response == (char)200)
		{
			const char* json_c_str = client.getString().c_str();
			client.end();
			return (bool)read_next_number_from_json(json_c_str, true);
		}
		client.end();
		return false;
	}


	bool set_curtain_as_partially_open()
	{
		HTTPClient client;
		client.begin(REQUEST_ADDRESS);
		client.addHeader("Content-Type", "application/x-www-form-urlencoded");
		char response = client.POST(STR(set_curtain_as_partially_open=, CURTAIN_NUMBER));

		if(response == (char)200)
		{
			const char* json_c_str = client.getString().c_str();
			client.end();
			return (bool)read_next_number_from_json(json_c_str, true);
		}
		client.end();
		return false;
	}


	bool set_event_as_activated(int event_number)
	{
		HTTPClient client;
		client.begin(REQUEST_ADDRESS);
		client.addHeader("Content-Type", "application/x-www-form-urlencoded");
		int response = client.POST(String("set_event_as_activated=") + event_number);

		if(response == (char)200)
		{
			const char* json_c_str = client.getString().c_str();
			client.end();
			return (bool)read_next_number_from_json(json_c_str, true);
		}
		client.end();
		return false;
	}

#endif
