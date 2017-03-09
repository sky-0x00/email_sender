#pragma once

#define GUID_NULL__STRING				L"00000000-0000-0000-0000-000000000000"
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

#define EMAIL_TOPIC								L"Альтернативные источники инвестиций - Cryp.Trade"

#define EMALE_MESSAGE__VK__ACCOUNT_ID			L"sky0x00"
#define EMALE_MESSAGE__VK_GROUP_ID				L"club136896048"
#define EMALE_MESSAGE__CRYP_TRADE__AGENT_ID		L"lek7mxoizggh"
#define EXTERNAL_WEB_STORAGE					L"https://sky-0x00.github.io/cryp-trade/"			// символ '/' обязателен на конце

#define STYLE_EXTERNAL 0
#define STYLE_INTERNAL 1
#define STYLE_INLINE   2

#define USE_STYLE STYLE_INLINE
#if USE_STYLE == STYLE_EXTERNAL
	#define EMAIL_MESSAGE					LR"(<!DOCTYPE HTML><html><head><title>)" EMAIL_TOPIC LR"(</title><meta charset="utf-8"><link rel="stylesheet" href=")" EXTERNAL_WEB_STORAGE LR"(index.css" charset="utf-8"></head><body><div class="center-block"><a href="https://cryp.trade/agent/)" EMALE_MESSAGE__CRYP_TRADE__AGENT_ID LR"("><div class="top"><img src=")" EXTERNAL_WEB_STORAGE LR"(logo_full.png"/></div></a><div class="content"><div class="message"><p>Вы уже слышали про криптовалюту или имеете четкое представление о ней? Впечатлены успехом биткоина? Или просто ищете дополнительный источник дохода? Предлагаю не пройти мимо и познакомиться с темой поближе.</p><p>В настоящее время я ищу соинвесторов для совместного заработка в одной из доходных компаний. Только реальные инвестиции и только реальный доход! Поделюсь своей стратегией, покажу что и как и, конечно, покажу свои выплаты.</p><p>Приходите в мою <a href="https://vk.com/)" EMALE_MESSAGE__VK_GROUP_ID LR"(">группу В-Контакте</a> и задавайте вопросы.</p></div><div class="contacts"><p>С уважением, <a href="https://vk.com/)" EMALE_MESSAGE__VK__ACCOUNT_ID LR"(">)" EMAIL_FROM_NAME LR"(</a></p><p>e-mail: <a href="mailto:)" EMAIL_ADDRESS_FROM LR"(?subject=)" EMAIL_TOPIC LR"(">)" EMAIL_ADDRESS_FROM LR"(</a></p></div><div class="unsibscribe"><p>Ваша персональная ссылка для подписки на данную рассылку: <span>)" GUID_NULL__STRING LR"(</span></p></div></div><a href="https://cryp.trade/agent/)" EMALE_MESSAGE__CRYP_TRADE__AGENT_ID LR"("><div class="bottom"><div class="right"><p class="link">https://cryp.trade</p><p class="info">CRYP TRADE CAPITAL HOLDING</p></div><img src=")" EXTERNAL_WEB_STORAGE LR"(link.png"/></div></a></div></body></html>)"
#elif USE_STYLE == STYLE_INTERNAL
	#define INTERNAL_STYLE_FILENAME			LR"(..\data\site\index.css)"
	#define EMAIL_MESSAGE					LR"(<!DOCTYPE HTML><html><head><title>)" EMAIL_TOPIC LR"(</title><meta charset="utf-8"><style type="text/css"></style></head><body><div class="center-block"><a href="https://cryp.trade/agent/)" EMALE_MESSAGE__CRYP_TRADE__AGENT_ID LR"STR("><div class="top"><img src=")STR" EXTERNAL_WEB_STORAGE LR"(logo_full.png"/></div></a><div class="content"><div class="message"><p>Вы уже слышали про криптовалюту или имеете четкое представление о ней? Впечатлены успехом биткоина? Или просто ищете дополнительный источник дохода? Предлагаю не пройти мимо и познакомиться с темой поближе.</p><p>В настоящее время я ищу соинвесторов для совместного заработка в одной из доходных компаний. Только реальные инвестиции и только реальный доход! Поделюсь своей стратегией, покажу что и как и, конечно, покажу свои выплаты.</p><p>Приходите в мою <a href="https://vk.com/)" EMALE_MESSAGE__VK_GROUP_ID LR"(">группу В-Контакте</a> и задавайте вопросы.</p></div><div class="contacts"><p>С уважением, <a href="https://vk.com/)" EMALE_MESSAGE__VK__ACCOUNT_ID LR"(">)" EMAIL_FROM_NAME LR"(</a></p><p>e-mail: <a href="mailto:)" EMAIL_ADDRESS_FROM LR"(?subject=)" EMAIL_TOPIC LR"(">)" EMAIL_ADDRESS_FROM LR"(</a></p></div><div class="unsibscribe"><p>Ваша персональная ссылка для подписки на данную рассылку: <span>)" GUID_NULL__STRING LR"(</span></p></div></div><a href="https://cryp.trade/agent/)" EMALE_MESSAGE__CRYP_TRADE__AGENT_ID LR"STR("><div class="bottom"><div class="right"><p class="link">https://cryp.trade</p><p class="info">CRYP TRADE CAPITAL HOLDING</p></div><img src=")STR" EXTERNAL_WEB_STORAGE LR"(link.png"/></div></a></div></body></html>)"
#else		// inline- стили
#define EMAIL_MESSAGE						LR"(<!DOCTYPE HTML><html><head><title>)" EMAIL_TOPIC LR"(</title><meta charset="utf-8"><style type="text/css">a:hover{color: #4d9ec3;}</style></head><body style="background-image: url(')" EXTERNAL_WEB_STORAGE LR"(strategy.png');"><div style="width: 820px; margin-left: auto;	margin-right: auto;"><a href="https://cryp.trade/agent/)" EMALE_MESSAGE__CRYP_TRADE__AGENT_ID LR"STR("><div style="border: 0px solid gray; text-align: center; background-color: rgba(162, 210, 252, 0.7); border-radius: 12px;"><img style="margin-top: 4px; margin-bottom: 4px; height: 64px;	border: 0px solid red;" src=")STR" EXTERNAL_WEB_STORAGE LR"(logo_full.png"/></div></a><div style="font-family: 'open sans', sans-serif; color: #302E2E; line-height: 1.0em; letter-spacing: 0.6px;"><div style="font-size: 16px;"><p style="margin-bottom: -12px;">Вы уже слышали про криптовалюту или имеете четкое представление о ней? Впечатлены успехом биткоина? Или просто ищете дополнительный источник дохода? Предлагаю не пройти мимо и познакомиться с темой поближе.</p><p style="margin-bottom: -12px;">В настоящее время я ищу соинвесторов для совместного заработка в одной из доходных компаний. Только реальные инвестиции и только реальный доход! Поделюсь своей стратегией, покажу что и как и, конечно, покажу свои выплаты.</p><p style="margin-bottom: -12px;">Приходите в мою <a style="color: #347da0; text-decoration: none; font-weight: bold; font-style: normal;" href="https://vk.com/)" EMALE_MESSAGE__VK_GROUP_ID LR"(">группу В-Контакте</a> и задавайте вопросы.</p></div><div style="border: 0px solid gray; margin-top: 40px; font-size: 14px; line-height: 1.1em;"><p style="margin-top: 0px; margin-bottom: 0px; border: 0px solid green;">С уважением, <a style="color: #347da0; text-decoration: none; font-weight: bold; font-style: normal;" href="https://vk.com/)" EMALE_MESSAGE__VK__ACCOUNT_ID LR"(">)" EMAIL_FROM_NAME LR"(</a></p><p style="margin-top: 0px; margin-bottom: 0px; border: 0px solid green;">e-mail: <a style="color: #347da0; text-decoration: none; font-weight: bold; font-style: normal;" href="mailto:)" EMAIL_ADDRESS_FROM LR"(?subject=)" EMAIL_TOPIC LR"(">)" EMAIL_ADDRESS_FROM LR"(</a></p></div><div style="font-size: 14px; display: none; opacity: 0; font-size: 0;"><p>Ваша персональная ссылка для подписки на данную рассылку: <span>)" GUID_NULL__STRING LR"(</span></p></div></div><a href="https://cryp.trade/agent/)" EMALE_MESSAGE__CRYP_TRADE__AGENT_ID LR"STR("><div style="margin-top: 10px; height: 56px; border: 0px solid gray; background-color: rgba(162, 210, 252, 0.7); border-radius: 12px; padding-left: 10px; padding-right: 10px; padding-top: 4px;"><div style="border: 0px solid gray; margin-right: 8px; margin-left: auto; margin-top: 4px; margin-bottom: auto; font-family: 'open sans', sans-serif; float: right;"><p style="margin-top: 0px; margin-bottom: 0px; color: #347da0; font-size: 24px;">https://cryp.trade</p><p style="margin-top: 0px; margin-bottom: 0px; color: #605E5E; font-size: 12px; text-transform: uppercase; font-weight: normal;">CRYP TRADE CAPITAL HOLDING</p></div><img style="border: 0px solid gray; margin-left: 0px; margin-right: auto; height: 52px; float: left;" src=")STR" EXTERNAL_WEB_STORAGE LR"(link.png"/></div></a></div></body></html>)"
#endif

#define DO_PAUSE_IN_PROCESS
#define PAUSE_SECS_MIN						 1
#define PAUSE_SECS_MAX						10

#define EMAIL_LOG__FILE_NAME				LR"(..\data\email.log)"
#define EMAIL_BASE__FILE_NAME				LR"(..\data\email.base)"