#pragma once

#define GUID_NULL__STRING				L"00000000-0000-0000-0000-000000000000"
#define LOCALE_NAME						"Russian_Russia.1251"

#define EMAIL_ADDRESS_FROM				"t.nemzinskiy@inbox.ru"
#define EMAIL_ADDRESS_FROM_PASSWORD		"X46rNMY5LRFrCXrwRaxh"
#define EMAIL_FROM_NAME					L"����� ����������"

#define SMTP_SERVER_NAME				"smtp.mail.ru"
#define SMTP_SERVER_PORT				465						// SSL

#define EMAIL_ADDRESS_LIST_TO			{	\
	"granada2003@inbox.ru",					\
	"granada2004@inbox.ru",					\
}

#define EMAIL_TOPIC								L"�������������� ��������� ���������� - Cryp.Trade"

#define EMALE_MESSAGE__VK__ACCOUNT_ID			L"sky0x00"
#define EMALE_MESSAGE__VK_GROUP_ID				L"club136896048"
#define EMALE_MESSAGE__CRYP_TRADE__AGENT_ID		L"lek7mxoizggh"
#define EXTERNAL_WEB_STORAGE					L"https://sky-0x00.github.io/cryp-trade/"

//#define USE_EXRERNAL_STYLE
#ifdef USE_EXRERNAL_STYLE
	#define EMAIL_MESSAGE						LR"(<!DOCTYPE HTML><html><head><title>)" EMAIL_TOPIC LR"(</title><meta charset="utf-8"><link rel="stylesheet" href=")" EXTERNAL_WEB_STORAGE LR"(index.css" charset="utf-8"></head><body><div class="center-block"><a href="https://cryp.trade/agent/)" EMALE_MESSAGE__CRYP_TRADE__AGENT_ID LR"("><div class="top"><img src=")" EXTERNAL_WEB_STORAGE LR"(logo_full.png"/></div></a><div class="content"><div class="message"><p>�� ��� ������� ��� ������������ ��� ������ ������ ������������� � ���? ���������� ������� ��������? ��� ������ ����� �������������� �������� ������? ��������� �� ������ ���� � ������������� � ����� �������.</p><p>� ��������� ����� � ��� ������������ ��� ����������� ��������� � ����� �� �������� ��������. ������ �������� ���������� � ������ �������� �����! �������� ����� ����������, ������ ��� � ��� �, �������, ������ ���� �������.</p><p>��������� � ��� <a href="https://vk.com/)" EMALE_MESSAGE__VK_GROUP_ID LR"(">������ �-��������</a> � ��������� �������.</p></div><div class="contacts"><p>� ���������, <a href="https://vk.com/)" EMALE_MESSAGE__VK__ACCOUNT_ID LR"(">)" EMAIL_FROM_NAME LR"(</a></p><p>e-mail: <a href="mailto:)" EMAIL_ADDRESS_FROM LR"(?subject=)" EMAIL_TOPIC LR"(">)" EMAIL_ADDRESS_FROM LR"(</a></p></div><div class="unsibscribe"><p>���� ������������ ������ ��� �������� �� ������ ��������: <span>)" GUID_NULL__STRING LR"(</span></p></div></div><a href="https://cryp.trade/agent/)" EMALE_MESSAGE__CRYP_TRADE__AGENT_ID LR"("><div class="bottom"><div class="right"><p class="link">https://cryp.trade</p><p class="info">CRYP TRADE CAPITAL HOLDING</p></div><img src=")" EXTERNAL_WEB_STORAGE LR"(link.png"/></div></a></div></body></html>)"
#else
	#define INTERNAL_STYLE_FILENAME				L"..\\data\\site\\index.css"
	#define EMAIL_MESSAGE						LR"(<!DOCTYPE HTML><html><head><title>)" EMAIL_TOPIC LR"(</title><meta charset="utf-8"><style type="text/css"></style></head><body><div class="center-block"><a href="https://cryp.trade/agent/)" EMALE_MESSAGE__CRYP_TRADE__AGENT_ID LR"STR("><div class="top" style="background-color: rgba(162, 210, 252, 0.7)"><img src=")STR" EXTERNAL_WEB_STORAGE LR"(logo_full.png"/></div></a><div class="content"><div class="message"><p>�� ��� ������� ��� ������������ ��� ������ ������ ������������� � ���? ���������� ������� ��������? ��� ������ ����� �������������� �������� ������? ��������� �� ������ ���� � ������������� � ����� �������.</p><p>� ��������� ����� � ��� ������������ ��� ����������� ��������� � ����� �� �������� ��������. ������ �������� ���������� � ������ �������� �����! �������� ����� ����������, ������ ��� � ��� �, �������, ������ ���� �������.</p><p>��������� � ��� <a href="https://vk.com/)" EMALE_MESSAGE__VK_GROUP_ID LR"(">������ �-��������</a> � ��������� �������.</p></div><div class="contacts"><p>� ���������, <a href="https://vk.com/)" EMALE_MESSAGE__VK__ACCOUNT_ID LR"(">)" EMAIL_FROM_NAME LR"(</a></p><p>e-mail: <a href="mailto:)" EMAIL_ADDRESS_FROM LR"(?subject=)" EMAIL_TOPIC LR"(">)" EMAIL_ADDRESS_FROM LR"(</a></p></div><div class="unsibscribe"><p>���� ������������ ������ ��� �������� �� ������ ��������: <span>)" GUID_NULL__STRING LR"(</span></p></div></div><a href="https://cryp.trade/agent/)" EMALE_MESSAGE__CRYP_TRADE__AGENT_ID LR"STR("><div class="bottom" style="background-color: rgba(162, 210, 252, 0.7)"><div class="right"><p class="link">https://cryp.trade</p><p class="info">CRYP TRADE CAPITAL HOLDING</p></div><img style="height: 64px;" src=")STR" EXTERNAL_WEB_STORAGE LR"(link.png"/></div></a></div></body></html>)"
#endif

#define DO_PAUSE_IN_PROCESS
#define PAUSE_SECS_MIN					1
#define PAUSE_SECS_MAX					10
