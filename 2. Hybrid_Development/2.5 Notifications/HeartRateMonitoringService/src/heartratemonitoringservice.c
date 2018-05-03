#include <tizen.h>
#include <service_app.h>
#include "heartratemonitoringservice.h"
#include <message_port.h>
#include <sensor.h>
#include <ecore.h>
#include <notification.h>

const char *REMOTE_WEB_APP_ID = "WdKRoYqwHo.HeartRateMonitoring";
const char *SERVICE_STOP_MESSAGE_PORT = "SERVICE_STOP_MESSAGE_PORT";
const char *HRM_MESSAGE_PORT = "HEART_RATE_MONITORING_MESSAGE_PORT";

/*Тип сенсора*/
sensor_type_e type = SENSOR_HRM;
/*Сенсор*/
sensor_h sensor;
/*Слушатель сенсора*/
sensor_listener_h listener;

/*Интервал через который вызывается функция слушателя сенсора*/
const int MIN_INTERVAL = 10000;
/*Таймер*/
Ecore_Timer *timer;

int port_id;

static bool handle_sensor_error(int error_code){
	/*проверка на наличие ошибок*/
	if (error_code == SENSOR_ERROR_NONE){

		/*Вывод сообщения об отсутствии ошибок*/
		dlog_print(DLOG_DEBUG, "hrm_service", "no errors");
		return true;
	} else {

		/*Вывод кода ошибки на экран*/
		dlog_print(DLOG_ERROR, "hrm_service", "error %d", error_code);
		return false;
	}
}

static bool handle_notification_error(int error_code){
	/*проверка на наличие ошибок*/
	if (error_code == NOTIFICATION_ERROR_NONE){

		/*Вывод сообщения об отсутствии ошибок*/
		dlog_print(DLOG_DEBUG, "hrm_service", "no errors");
		return true;
	} else {

		/*Вывод кода ошибки на экран*/
		dlog_print(DLOG_ERROR, "hrm_service", "error %d", error_code);
		return false;
	}
}

notification_h create_notification(){

	/*Объявление переменной типа уведомление*/
	static notification_h notification = NULL;

	/*Путь до картинки*/
	char image_path[255];

	/*Получение пути до папки с ресурсами*/
	char *shared_path = app_get_shared_resource_path();

	/*Копирование пути до картинки в переменную image_path*/
	snprintf(image_path, 255, "%stutorial_native_api_application.png", shared_path);

	/*Создание уведомления*/
	notification = notification_create(NOTIFICATION_TYPE_NOTI);
	if (notification != NULL) {

		/*Добавление заголовка уведомления*/
		int error = notification_set_text(notification, NOTIFICATION_TEXT_TYPE_TITLE, "Heart Rate is too high",
		                            NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
		handle_notification_error(error);

		/*Добавление картинки уведомления*/
		error = notification_set_image(notification, NOTIFICATION_IMAGE_TYPE_ICON, image_path);
		handle_notification_error(error);

	}
	return notification;
}

void send_hrm_value_to_web(float hrm_value){
	char hrm_value_str[3];

	/*Приведение значения пульса к строковому типу*/
	sprintf(hrm_value_str,"%d", (int)hrm_value);

	/*Подготовка данных для отправки*/
	bundle *bund = NULL;
	bund = bundle_create();
	bundle_add_str(bund, "hrm_value", hrm_value_str);

	/*Отправка пульса в веб-приложение*/
	message_port_send_message(REMOTE_WEB_APP_ID, HRM_MESSAGE_PORT, bund);
}

static Eina_Bool timer_cb(void *data EINA_UNUSED)
{
	/* Запуск сенсора*/
	int error = sensor_listener_start(listener);
	handle_sensor_error(error);

	return ECORE_CALLBACK_RENEW;
}

void start_timer(){
	timer = ecore_timer_add(30.0, timer_cb, NULL);
}

void on_sensor_event(sensor_h sensor, sensor_event_s *event, void *user_data){

	/*Отправка данных о пульсе веб-приложению*/
	send_hrm_value_to_web(event->values[0]);

	/*Остановка слушателя пульса*/
	int error = sensor_listener_stop(listener);
	handle_sensor_error(error);

	/*Проверка превышения заданного знчения пульса*/
	if(event->values[0] >= 100.0){

		/*Создание уведомления*/
		notification_h notification = create_notification();

		/*Отображение ведомления*/
		error = notification_post(notification);
		handle_notification_error(error);
	}
}

static void start_hrm_sensor(){
	bool supported;

	/*Проверка поддержки сенсора устройством*/
	int error = sensor_is_supported(type, &supported);
	handle_sensor_error(error);

	if(supported){

		/*Получение сенсора*/
		error = sensor_get_default_sensor(type, &sensor);
		handle_sensor_error(error);

		/*Создание слушателя сенсора*/
	    error = sensor_create_listener(sensor, &listener) ;
	    handle_sensor_error(error);

	    /*Задание функции-отклика для получения значений сенсора*/
	    error = sensor_listener_set_event_cb(listener, MIN_INTERVAL, on_sensor_event, NULL);
	    handle_sensor_error(error);

	    /*Запуск сенсора*/
		error = sensor_listener_start(listener);
		handle_sensor_error(error);

	}
}

static void message_port_cb(int local_port_id, const char *remote_app_id, const char *remote_port, bool trusted_remote_port, bundle *message, void *user_data)
{
	/*Инициализация строки для получения команды*/
	char* command;

	/*Подучение команды*/
	bundle_get_str(message, "Command", &command);

	/*Сравнение значения с полученной командой*/
	if (strncmp(command, "STOP SERVICE", 12)){

		/*Остановка приложения*/
		service_app_exit();
	}
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

	/*Удаление таймера*/
	ecore_timer_del(timer);

	notification_delete(notification);

    return;
}

void service_app_control(app_control_h app_control, void *data)
{

	port_id = message_port_register_local_port(SERVICE_STOP_MESSAGE_PORT, message_port_cb, NULL);

	/*Запуск сенсора для мониторинга пульса*/
	start_hrm_sensor();

	/*Запуск таймера*/
	start_timer();
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
