#pragma once
#include <string>
#include <iostream>

#include "utils.h"

class Mihoyosdk
{
public:
	std::string verify(const int uid, const std::string access_key);
	std::string getBHVer();
	std::string getOAServer();
	void scanCheck(const std::string& qrCode, const std::string& bhInfo);
	void scanConfirm(const std::string&, const std::string&);
	void setUserName(const std::string& name);
private:
	std::string makeSign(const std::string);
	std::string bh3Sign(std::string);
private:
	utils u;
	std::map<std::string, std::string> verifyData = { {"uid","1"},{"access_key","590"} };
	const std::string loginV2Url = "https://api-sdk.mihoyo.com/bh3_cn/combo/granter/login/v2/login";
	const std::string verifyBody = "{\"device\":\"0000000000000000\",\"app_id\":1,\"channel_id\":14,\"data\":\"\",\"sign\":\"\"}";
	const std::string scanCheckS = "{\"app_id\":\"1\",\"device\":\"0000000000000000\",\"ticket\":\"abab\",\"ts\":1637593776066,\"sign\":\"abab\"}";
	const std::string scanResult = "{\"device\":\"0000000000000000\",\"app_id\":1,\"ts\":1637593776681,\"ticket\":\"\",\"payload\":{},\"sign\":\"\"}";
	const std::string scanData = "{\"accountType\":\"2\",\"accountID\":\"\",\"accountToken\":\"\",\"dispatch\":{}}";
	std::string scanRawR = "{\"heartbeat\":false, \"open_id\":\"\", \"device_id\":\"0000000000000000\",\"app_id\":\"1\",\"channel_id\":\"14\",\"combo_token\":\"\",\"asterisk_name\":\"name\",\"combo_id\":\"\",\"account_type\":\"2\"}";
	const std::string scanExtR = "{\"data\":{}}";
	const std::string scanPayloadR = "{\"raw\":\"\",\"proto\":\"Combo\",\"ext\":\"\"}";
};