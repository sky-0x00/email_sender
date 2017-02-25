#pragma once

#define LOCALE_NAME						"Russian_Russia.1251"

#define EMAIL_ADDRESS_FROM				"t.nemzinskiy@inbox.ru"
#define EMAIL_ADDRESS_FROM_PASSWORD		"X46rNMY5LRFrCXrwRaxh"
#define EMAIL_FROM_NAME					L"Тихон Немзинский"

#define SMTP_SERVER_NAME				"smtp.mail.ru"
#define SMTP_SERVER_PORT				465						// SSL

#define EMAIL_ADDRESS_LIST_TO			{	\
	"granada2003@inbox.ru",					\
	"granada2004@inbox.ru",					\
}

#define EMAIL_MESSAGE_TOPIC				L"Альтернативные источники инвестиций - Cryp.Trade"
#define EMAIL_MESSAGE_CONTENT			L"<html><head><title>Заголовок письма</title></head><body><p><span style='color: #00FF00'>Тело</span> письма</p></body></html>"

#define DO_PAUSE_IN_PROCESS
#define PAUSE_SECS_MIN					1
#define PAUSE_SECS_MAX					10
