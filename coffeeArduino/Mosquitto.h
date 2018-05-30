#ifndef _MOSQUITTO__H_
#define _MOSQUITTO__H_

#define MQTT_MAX_PACKET_SIZE 300

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class Mosquitto {
	private:
		WiFiClient _client;
		PubSubClient _mqtt_client;

		int _mqtt_reconnect_count;
		int _debug_level;

	protected:
		String _mqtt_host;
		String _mqtt_id;
		String _mqtt_user;
		String _mqtt_pass;
		int _mqtt_port;
		String _mqtt_topic_in;
		String _mqtt_topic_out;

	public:
		void setup();
		void setup(String host, String id, String user, String pass, int port);
		void setup(String host, String id, String user, String pass, int port, String topic_in, String topic_out);

		bool reconnect();
		bool read();
		bool publish(String message);

		void setDebugLevel(unsigned int level) { _debug_level = level; }

		void setMqttHost(String host) { _mqtt_host = host; }
		void setMqttId(String id) { _mqtt_id = id; }
		void setMqttUser(String user) { _mqtt_user = user; }
		void setMqttPassword(String password) { _mqtt_pass = password; }
		void setMqttPort(int port) { _mqtt_port = port; }
		void setMqttTopicIn(String topic) { _mqtt_topic_in = topic; }
		void setMqttTopicOut(String topic) { _mqtt_topic_out = topic; }
		void setMqttReconnectCount(unsigned int count) { _mqtt_reconnect_count = count; }
		void setCallback(void (*callback)(char*, uint8_t*, unsigned int));
	public:
		Mosquitto() { _mqtt_client.setClient(_client); _mqtt_reconnect_count = 0; _debug_level = 0; };
		~Mosquitto() {};
};

#endif
