#include <tizen.h>
#include <service_app.h>
#include "notesservice.h"

#include <message_port.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>

const char *ADD_MESSAGE_PORT = "ADD_NOTE_MESSAGE_PORT";

int port_id;

static void message_port_cb(int local_port_id, const char *remote_app_id, const char *remote_port, bool trusted_remote_port, bundle *message, void *user_data)
{
	/* Объявление переменных-строк*/
	char *note_title;
	char *note_text;

	/* Извлечение полученных заголовка и текста в строковые переменные*/
	bundle_get_str(message, "note_title", &note_title);
	bundle_get_str(message, "note_text", &note_text);

	/*Вывод полученных значений в логи приложения*/
	dlog_print(DLOG_DEBUG, "notes_service", note_title);
	dlog_print(DLOG_DEBUG, "notes_service", note_text);

}

bool service_app_create(void *data)
{
    // Todo: add your code here.
    return true;
}

void service_app_terminate(void *data)
{
	/*Освобождение ресурсов порта*/
	message_port_unregister_local_port(port_id);

    return;
}

void service_app_control(app_control_h app_control, void *data)
{
	port_id = message_port_register_local_port("ADD_NOTE_MESSAGE_PORT", message_port_cb, NULL);

	dlog_print(DLOG_DEBUG, "notes_service", "service_app_control %s", ADD_MESSAGE_PORT);

    return;
}

static void
service_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	return;
}

static void
service_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
service_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
service_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int main(int argc, char* argv[])
{
    char ad[50] = {0,};
	service_app_lifecycle_callback_s event_callback;
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = service_app_create;
	event_callback.terminate = service_app_terminate;
	event_callback.app_control = service_app_control;

	service_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, service_app_low_battery, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, service_app_low_memory, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, service_app_lang_changed, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, service_app_region_changed, &ad);

	return service_app_main(argc, argv, &event_callback, ad);
}
