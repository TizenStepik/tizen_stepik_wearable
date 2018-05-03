#include <tizen.h>
#include <service_app.h>
#include "notesservice.h"

#include <message_port.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>

const char *ADD_MESSAGE_PORT = "ADD_NOTE_MESSAGE_PORT";
const char *GET_NOTES_MESSAGE_PORT = "GET_NOTES_MESSAGE_PORT";
const char *REMOTE_WEB_APP_ID = "5zfZeMzxoW.Notes";

sqlite3 *db;

int port_id;

static bool error_handling(int error_code){

	/*проверка на наличие ошибок*/
	if (error_code == SQLITE_OK){

		/*Вывод сообщения об отсутствии ошибок*/
		dlog_print(DLOG_DEBUG, "notes_service", "no errors");
		return true;
	} else {

		/*Вывод кода ошибки на экран*/
		dlog_print(DLOG_ERROR, "notes_service", "error %d", error_code);
		return false;
	}
}

static void create_note_table()
{
	/*Объявление строки-запроса для создания таблицы*/
	char *sqlRequest = "CREATE TABLE IF NOT EXISTS Notes(TITLE TEXT NOT NULL, "
			"TEXT TEXT NOT NULL);";

	/*Строка для записи сообщений об ошибке при выполнении запроса*/
	char *err_msg;

	/*Выполнение запроса на создание таблицы*/
	int error = sqlite3_exec(db, sqlRequest, NULL, 0, &err_msg);
	/*Проверка кода выполнения запроса, вывод соответствующего сообщения в консоль*/
	error_handling(error);

}

static void open_db(){

	/*настройка конфигураций базы данных*/
	sqlite3_shutdown(); // закрытие базы данных
	sqlite3_config(SQLITE_CONFIG_URI, 1); // настройка пути к базе данных через URI
	sqlite3_initialize(); // инициализация базы данных

	/*создание пути до файла с базой данных*/
	char * resource = app_get_data_path(); // получение пути до ресурсов
	int size = strlen(resource) + 10; // задание размера строки для копирования
	char * path = malloc(sizeof(char)*size); // создание строки для хранения пути к бд
	strncat(path, resource, size); //копирование значения пути до бд
	strncat(path, "notes.db", size); // опирование названия бд

	/*открытие базы данных*/
	sqlite3_open(path, &db);
}

static void insert_record_to_note_table(char *note_title, char *note_text)
{
	/*Объявление переменной-строки для формирования запроса*/
	char sqlRequest[256];
	/*Строка для записи сообщений об ошибке при выполнении запроса*/
	char *err_msg;

	/*Формирование запроса*/
	snprintf(sqlRequest, 256, "INSERT INTO Notes VALUES('%s', '%s');", note_title, note_text);
	/*Выполнение запроса*/
	int error = sqlite3_exec(db, sqlRequest, NULL, 0, &err_msg);
	/*Проверка кода выполнения запроса, вывод соответствующего сообщения в консоль*/
	error_handling(error);
}

static int db_read_cb(void *data, int argc, char **argv, char **azColName)
{
	/*Получение данных*/
	char *note_title = argv[0];
	char *note_text = argv[1];

	/*Создание переменной для отправления в веб-приложение*/
	bundle *bund = NULL;
	bund = bundle_create();
	bundle_add_str(bund, "note_title", note_title);
	bundle_add_str(bund, "note_text", note_text);

	/*Отправка сообщения в веб-приложение*/
	message_port_send_message(REMOTE_WEB_APP_ID, GET_NOTES_MESSAGE_PORT, bund);

	/*Освобождение ресурсов*/
	bundle_free(bund);

	return 0;
}

static void get_all_notes()
{
	/*Формирование запроса*/
	char *sqlRequest = "SELECT * FROM Notes;";
	/*Строка для записи сообщений об ошибке при выполнении запроса*/
	char *err_msg;

	/*Выполнение запроса*/
	int error = sqlite3_exec(db, sqlRequest, db_read_cb, 0, &err_msg);
	/*Проверка кода выполнения запроса, вывод соответствующего сообщения в консоль*/
	error_handling(error);
}

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

	insert_record_to_note_table(note_title, note_text);
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

	/*Освобождение ресурсов базы данных*/
	sqlite3_close(db);
    return;
}

void service_app_control(app_control_h app_control, void *data)
{
	port_id = message_port_register_local_port("ADD_NOTE_MESSAGE_PORT", message_port_cb, NULL);

	dlog_print(DLOG_DEBUG, "notes_service", "service_app_control %s", ADD_MESSAGE_PORT);

//	/*Открытие базы данных*/
	open_db();

	/*Создание таблицы*/
	create_note_table();

	get_all_notes();

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
