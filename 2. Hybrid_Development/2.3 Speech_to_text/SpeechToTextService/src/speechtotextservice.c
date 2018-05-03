#include <tizen.h>
#include <service_app.h>
#include "speechtotextservice.h"

#include <message_port.h>
#include <stt.h>

const char *REMOTE_WEB_APP_ID = "zj6ij6pcUW.SpeechToText";
const char *RECORD_STATE_MESSAGE_PORT = "RECORD_STATE_MESSAGE_PORT";
const char *RECORD_CONTROL_MESSAGE_PORT = "RECORD_CONTROL_MESSAGE_PORT";
const char *STT_RESULT_MESSAGE_PORT = "STT_RESULT_MESSAGE_PORT";

stt_h stt;
int port_id;

static bool error_handling(int error_code){

	/*проверка на наличие ошибок*/
	if (error_code == STT_ERROR_NONE){

		/*Вывод сообщения об отсутствии ошибок*/
		dlog_print(DLOG_DEBUG, "stt_app", "no errors");
		return true;
	} else {

		/*Вывод кода ошибки на экран*/
		dlog_print(DLOG_ERROR, "stt_app", "error %d", error_code);
		return false;
	}
}

static void stt_state_changed(stt_h stt, stt_state_e previous, stt_state_e current, void* user_data){

	bundle *bund = NULL;

	/*Создание переменной типа bundle*/
	bund = bundle_create();

	switch(current){
	case STT_STATE_READY:

		/*Добавление в bund состояния готовности*/
		bundle_add_str(bund, "state", "READY");
		break;

	case STT_STATE_RECORDING:

		/*Добавление в bund состояния записи*/
		bundle_add_str(bund, "state", "RECORDING");
		break;

	default:

		/*Добавление в bund пустого состояния*/
		bundle_add_str(bund, "state", "");
	}

	/*Отправка сообщения сервиса веб приложению*/
	message_port_send_message(REMOTE_WEB_APP_ID, RECORD_STATE_MESSAGE_PORT, bund);

	/*Освобождение памяти для bund*/
	bundle_free(bund);
}

static void recognition_result_cb(stt_h stt, stt_result_event_e event, const char** data, int data_count,
					  const char* msg, void *user_data){

	if((int)event == STT_RESULT_EVENT_FINAL_RESULT) {

		bundle *bund = NULL;

		/*Создание переменной типа bundle*/
		bund = bundle_create();
		bundle_add_str(bund, "RESULT_TEXT", data[0]);

		message_port_send_message(REMOTE_WEB_APP_ID, STT_RESULT_MESSAGE_PORT, bund);

		dlog_print(DLOG_DEBUG, "tag", data[0]);
	}
}

static void initialize_stt(){

	/*Инициализация переменной stt*/
	int error = stt_create(&stt);

	if(error_handling(error)) {

		/*добавление слушателя на изменение состояния tts*/
		error = stt_set_state_changed_cb(stt, stt_state_changed, NULL);
		error_handling(error);

		/*добавление слушателя на получение результата преобразования речи в текст*/
		error = stt_set_recognition_result_cb(stt, recognition_result_cb, NULL);
		error_handling(error);

		/*подготовка сервиса stt к работе*/
		error = stt_prepare(stt);
		error_handling(error);

	}
}

static void record_controll_message_port_cb(int local_port_id, const char *remote_app_id, const char *remote_port, bool trusted_remote_port, bundle *message, void *user_data)
{
	/*Строка для получения команды*/
	char* command;

	/*Получение команды от веб-сервиса*/
	bundle_get_str(message, "Command", &command);

	/*Сравнение полученнй команды с командой старта записи речи*/
	if (strncmp(command, "START_RECORD", 12) == 0){

		/*Получение языка, выбранного на устройсте*/
		char* language;
		int error = stt_get_default_language(stt, &language);
		error_handling(error);

		/*Запуск записи речи*/
		error = stt_start(stt, language, STT_RECOGNITION_TYPE_FREE);
		error_handling(error);

	} else if (strncmp(command, "STOP_RECORD", 11) == 0) {

		/*Остановка записи речи, запуск распознавания речи*/
		int error = stt_stop(stt);
		error_handling(error);
	}
}

bool service_app_create(void *data)
{
    // Todo: add your code here.
    return true;
}

void service_app_terminate(void *data)
{
    message_port_unregister_local_port(port_id);

    return;
}

void service_app_control(app_control_h app_control, void *data)
{
	/*Регистрация порта для сообщений с командами*/
	port_id = message_port_register_local_port(RECORD_CONTROL_MESSAGE_PORT, record_controll_message_port_cb, NULL);

	/*Инициализация сервиса stt*/
	initialize_stt();

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
