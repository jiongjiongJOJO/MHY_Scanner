﻿#include "OfficialApi.h"
#include "CryptoKit.h"
#include "Json.h"
#include <random>
#include <sstream>
#include <iomanip>

std::string OfficialApi::getUid()
{
    return cookieMap["login_uid"];
}

std::string OfficialApi::generateUUID() 
{
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(0, 15);

    std::stringstream ss;
    for (int i = 0; i < 32; ++i) 
    {
        int randomDigit = distribution(generator);
        ss << std::hex << randomDigit;
    }

    std::string uuid = ss.str();

    // 格式化UUID，插入分隔符
    uuid.insert(8, "-");
    uuid.insert(13, "-");
    uuid.insert(18, "-");
    uuid.insert(23, "-");

    return uuid;
}

std::string OfficialApi::getDS()
{
    std::string time_now = std::to_string(getCurrentUnixTime());
    std::random_device rd;
    std::mt19937 gen(rd());
    // 定义范围
    int lower_bound = 100001;
    int upper_bound = 200000;
    std::uniform_int_distribution<int> dist(lower_bound, upper_bound);
    std::string rand = std::to_string(dist(gen));
    std::string m = "salt=" +salt +"&t=" + time_now +"&r=" + rand;
    CryptoKit::Md5(m);
    return time_now+","+rand +"," + m;
}

//扫码请求
int OfficialApi::scanRequest(std::string ticket,std::string uid,std::string token)
{
    std::string UUID = generateUUID();
    json::Json payload;
    payload["app_id"] = 8;
    payload["device"] = UUID;
    payload["ticket"] = ticket;
    std::string s;
    std::string hkrpgFirst = "https://api-sdk.mihoyo.com/hkrpg_cn/combo/panda/qrcode/scan";
    PostRequest(s, hkrpgFirst,payload.str());
    json::Json j;
    j.parse(s);
    if ((int)j["retcode"] != 0)
    {
        return -1;
    }
    if (confirmRequest(UUID,ticket,uid,token) != 0)
    {
        return -2;
    }
    return 0;
}

//扫码确认
int OfficialApi::confirmRequest(std::string UUID, std::string ticket, std::string uid, std::string token)
{
    std::string s;
    std::string getToken = "https://api-sdk.mihoyo.com/hkrpg_cn/combo/panda/qrcode/confirm";
    json::Json payload;
    payload["proto"] = "Account";
    payload["raw"] = "{\\\"uid\\\":\\\""+uid+"\\\",\\\"token\\\":\\\""+ token+"\\\"}";
    json::Json data;
    data["app_id"] = 8;
    data["device"] = UUID;
    data["payload"] = payload;
    data["ticket"] = ticket;
    std::string d = data.str();
    PostRequest(s, getToken, d);
    s = UTF8_To_string(s);
    json::Json j;
    j.parse(s);
    if ((int)j["retcode"] != 0)
    {
        return -1;
    }
    return 0;
}

//获取用户完整信息
std::string OfficialApi::getUserName(std::string uid)
{
    std::string url = "https://bbs-api.miyoushe.com/user/api/getUserFullInfo";
    std::string re;
    url += "?uid=" + uid;
    GetRequest(re, url);
    json::Json j;
    j.parse(re);
    re = j["data"]["user_info"]["nickname"];
    return re;

}

//获取账号上所有的角色
std::string OfficialApi::getRole()
{
    std::string url = "https://api-takumi.miyoushe.com/binding/api/getUserGameRolesByStoken";
    headers["DS"] = getDS();
    headers["Cookie"] = "stuid=" + cookieMap["login_uid"] + ";" + "stoken=" + data + ";" + "mid=" + "043co169fb_mhy";
    std::string re;
    GetRequest(re, url, headers);
    re = UTF8_To_string(re);
    string_To_UTF8(re);
    headers.erase("Cookie");
    return std::string();
}

void OfficialApi::cookieParser(const std::string& cookieString)
{
    // 切割 cookie 字符串
    size_t pos = 0;
    while (pos < cookieString.length()) 
    {
        // 查找键值对的结束位置
        size_t endPos = cookieString.find(';', pos);
        if (endPos == std::string::npos) 
        {
            endPos = cookieString.length();
        }

        // 提取键值对
        size_t equalPos = cookieString.find('=', pos);
        if (equalPos != std::string::npos && equalPos < endPos) 
        {
            std::string key = cookieString.substr(pos, equalPos - pos);
            key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
            std::string value = cookieString.substr(equalPos + 1, endPos - equalPos - 1);

            // 添加到字典中
            cookieMap[key] = value;
        }

        // 更新位置
        pos = endPos + 1;
    }
}

std::string OfficialApi::getMultiTokenByLoginTicket()
{
    std::string url = "https://api-takumi.mihoyo.com/auth/api/getMultiTokenByLoginTicket";
    std::map<std::string, std::string> params =
    {
        {"login_ticket",cookieMap["login_ticket"]},
        {"uid",cookieMap["login_uid"]},
        {"token_types","3"},
    };
    url = Url(url, params);
    std::string s;
    GetRequest(s, url);
    json::Json j;
    j.parse(s);
    data = j["data"]["list"][0]["token"];
    return data;
}

std::string OfficialApi::getGameToken()
{
    std::string url = "https://api-takumi.mihoyo.com/auth/api/getGameToken";
    std::map<std::string, std::string> params =
    {
        {"stoken",getMultiTokenByLoginTicket()},
        {"uid",cookieMap["login_uid"]},
    };
    url = Url(url, params);
    std::string s;
    GetRequest(s, url);
    json::Json j;
    j.parse(s);
    const std::string data = j["data"]["game_token"];
    return data;
}