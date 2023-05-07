﻿#pragma once
#include <QThread>
#include <QMutex>
#include "ScreenScan.h"
#include "QRScanner.h"
#include "Mihoyosdk.h"
class ThreadGetScreen
	: public QThread
{

	Q_OBJECT
public:
	ThreadGetScreen(QObject* parent);
	~ThreadGetScreen();
	bool isExit = true;
	void biliInitScreen(int uid, std::string access_key, std::string uname);
	void run();
signals:
	void loginResults(const bool& b);
private:
	QMutex m_mux;
	bool signed_in = true;
	std::string LoginData;
	Mihoyosdk m;
};