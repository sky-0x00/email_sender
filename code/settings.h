#pragma once

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

#define EMAIL_MESSAGE							LR"(<!DOCTYPE HTML><html><head><title>)" EMAIL_TOPIC LR"(</title><meta charset="utf-8"><link rel="stylesheet" href=")" EXTERNAL_WEB_STORAGE LR"(index.css" charset="utf-8"></head><body><div class="center-block"><a href="https://cryp.trade/agent/)" EMALE_MESSAGE__CRYP_TRADE__AGENT_ID LR"("><div class="top"><img src=")" EXTERNAL_WEB_STORAGE LR"(logo_full.png"/></div></a><div class="content"><div class="message"><p>�� ��� ������� ��� ������������ ��� ������ ������ ������������� � ���? ���������� ������� ��������? ��� ������ ����� �������������� �������� ������? ��������� �� ������ ���� � ������������� � ����� �������.</p><p>� ��������� ����� � ��� ������������ ��� ����������� ��������� � ����� �� �������� ��������. ������ �������� ���������� � ������ �������� �����! �������� ����� ����������, ������ ��� � ��� �, �������, ������ ���� �������.</p><p>��������� � ��� <a href="https://vk.com/)" EMALE_MESSAGE__VK_GROUP_ID LR"(">������ �-��������</a> � ��������� �������.</p></div><div class="contacts"><p>� ���������, <a href="https://vk.com/)" EMALE_MESSAGE__VK__ACCOUNT_ID LR"(">)" EMAIL_FROM_NAME LR"(</a></p><p>e-mail: <a href="mailto:)" EMAIL_ADDRESS_FROM LR"(?subject=)" EMAIL_TOPIC LR"(">)" EMAIL_ADDRESS_FROM LR"(</a></p></div></div><a href="https://cryp.trade/agent/)" EMALE_MESSAGE__CRYP_TRADE__AGENT_ID LR"("><div class="bottom"><div class="right"><p class="link">https://cryp.trade</p><p class="info">CRYP TRADE CAPITAL HOLDING</p></div><img src=")" EXTERNAL_WEB_STORAGE LR"(link.png"/></div></a></div></body></html>)"

#define DO_PAUSE_IN_PROCESS
#define PAUSE_SECS_MIN					1
#define PAUSE_SECS_MAX					10
