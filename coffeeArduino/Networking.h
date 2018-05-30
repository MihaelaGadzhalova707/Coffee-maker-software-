#ifndef _NETWORKING__H_
#define _NETWORKING__H_

#include <ESP8266WiFi.h>

class Networking {
	private:
		WiFiClient _client;
		int _http_requests_count;
		int _debug_level;
		int _local_id;

	protected:
		String _wifi_ssid;
		String _wifi_password;
		String _host;
		int _port;
	public:
		void setDebugLevel(unsigned int level) { _debug_level = level; }
		void setLocalId(int id) { _local_id = id; }

		void setWifiSsid(String ssid) { _wifi_ssid = ssid; }
		void setWifiPassword(String password) { _wifi_password = password; }
		void setHost(String host) { _host = host; }
		void setPort(int port) { _port = port; }

		bool isWifiConnected();
		bool connectWifi();
		bool scanWifi(String & networks);
		bool sendHttpRequest(String url, String contents, String &out, uint16_t * counter = NULL, uint16_t * ref_counter = NULL);
		String URLEncode(const char* msg);

		bool stopWifiAp();
		bool startWifiAp();
		String getWifiMode();
		String getWifiMode(WiFiMode mode);
		String getWifiStatus();
		String getWifiStatus(unsigned int status);
		String getWifiEncryption(unsigned int encryption);
	public:
		enum error_t {
			errorNoError = 0,
			errorFileNotFound,
			errorFileNotOpen,
			errorBufferTooSmall,
			errorSeekError,
			errorSectionNotFound,
			errorKeyNotFound,
			errorEndOfFile,
			errorUnknownError,
		};
	public:
		Networking() { _http_requests_count = 0; _debug_level = 0; };
		~Networking() {};
};

#endif
