#include "nativehelloapp.h"
#include <tts.h>

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
	Evas_Object *btn; // кнопка
	Evas_Object *box; // контейнер
	Evas_Object *entry; // поле для ввода
} appdata_s;

tts_h tts;

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}


static bool error_handling(int error_code){

	/*проверка на наличие ошибок*/
	if (error_code == TTS_ERROR_NONE){

		/*Вывод сообщения об отсутствии ошибок*/
		dlog_print(DLOG_DEBUG, "native_hello_app", "no errors");
		return true;
	} else {

		/*Вывод кода ошибки на экран*/
		dlog_print(DLOG_ERROR, "native_hello_app", "error %d", error_code);
		return false;
	}
}

static int get_state(tts_h* tts)
{
	 /*Инициализация переменной для получения состояния tts*/
	 tts_state_e current_state;
	 /*Получение состояния tts*/
	 int error_code = tts_get_state(*tts, &current_state);

	 /*Обработка ошибки*/
	 if (error_handling(error_code))
	 {
		 return (int) current_state;
	 } else {
		 return -1;
	 }
 }


static void
state_changed_cb(tts_h tts, tts_state_e previous, tts_state_e current, void* user_data)
{
	 appdata_s *ad = user_data;

	 switch (current)
	 {
	 case TTS_STATE_READY:

		 /*Изменение текста кнопки*/
		 elm_object_text_set(ad->btn, "Play");
		 break;
	 case TTS_STATE_PLAYING:

		 /*Изменение текста кнопки*/
		 elm_object_text_set(ad->btn, "Stop");
		 break;
	 default:
	 break;
	 }
}

static void add_text(char* text) {
	/*Задание языка*/
	const char* language = "en_US";

	/*Задание параметров голоса*/
	int voice_type = TTS_VOICE_TYPE_FEMALE;

	/*Задние скорости воспроизведения*/
	int speed = TTS_SPEED_AUTO;

	/*Идентификатор для воспроизводимого текста*/
	 int utt_id;

	 /*Добавление текста для воспроизведения*/
	 int error = tts_add_text(tts, text, language, voice_type, speed, &utt_id);
	 error_handling(error);
}

static void play_text(char *text){

	/*Получение состояния функции воспроизведения речи*/
	int state = get_state(&tts);

	/*Проверка значения состояния*/
	if ((tts_state_e) state == TTS_STATE_READY) {

		/*Добавление текста, который будет воспроизводиться*/
		add_text(text);

		/*Воспроизведение текста*/
		int error = tts_play(tts);
		error_handling(error);

	} else if ((tts_state_e) state == TTS_STATE_PLAYING) {

		/*Остановка воспроизведения текста*/
		int error = tts_stop(tts);
		error_handling(error);
	}
}

static void
btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	/*получение структуры*/
	appdata_s* ad = data;
	/*извлечение введенного имени*/
	const char* text = elm_entry_entry_get(ad->entry);
	/*строка для приветственного сообщения*/
	char hello_text[255];

	/*создание приветственного сообщения*/
	sprintf(hello_text, "Hello, %s!", text);
	/*вывод сообщения на экран*/
	elm_object_text_set(ad->label, hello_text);

	/*Воспроизведение текста*/
	play_text(hello_text);
}

static void
create_base_gui(appdata_s *ad)
{
	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/*создание контейнера*/
	ad->box = elm_box_add(ad->conform);
	/*размещение контейнера на экране*/
	evas_object_size_hint_weight_set(ad->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	/*добавление контейнера в родительский контейнер*/
	elm_object_content_set(ad->conform, ad->box);
	/*вывод контейнера на экран*/
	evas_object_show(ad->box);

	/* Label */
	/* Create an actual view of the base gui.
	   Modify this part to change the view. */
	/*создание текстового поля*/
	ad->label = elm_label_add(ad->box);
	/*добавление текста*/
	elm_object_text_set(ad->label, "Hello!");
	/*азмещение текстового полян на экране*/
	evas_object_size_hint_weight_set(ad->label, EVAS_HINT_FILL, 0.5);
	/*добавление текстового поля в контейнер*/
	elm_box_pack_end(ad->box, ad->label);
	/*вывод текстового поля на  экран*/
	evas_object_show(ad->label);

	/*создание поля для ввода*/
	ad->entry = elm_entry_add(ad->box);
	/*настройка возможности писать текст только в одну строку*/
	elm_entry_single_line_set(ad->entry, EINA_TRUE);
	/*добавление текста*/
	elm_entry_entry_insert(ad->entry, "Enter your name");
	/*размещение поля для ввода на экране*/
	evas_object_size_hint_weight_set(ad->entry, EVAS_HINT_EXPAND, 0.5);
	/*добавление поля для ввода в контейнер*/
	elm_box_pack_end(ad->box, ad->entry);
	/*вывод поля для ввода на экран*/
	evas_object_show(ad->entry);

	/*создание кнопки*/
	ad->btn = elm_button_add(ad->box);
	/*добавление текста кнопки*/
	elm_object_text_set(ad->btn, "Press");
	/*размещение кнопки на экране*/
	evas_object_size_hint_align_set(ad->btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	/*добавление обработчика на нажатие кнопки*/
	evas_object_smart_callback_add(ad->btn, "clicked", btn_clicked_cb, ad);
	/*добавлнеие кнопки в контейнер*/
	elm_box_pack_end(ad->box, ad->btn);
	/*вывод кнопки на экран*/
	evas_object_show(ad->btn);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static void init_tts(appdata_s *ad){

	/*Инициализация переменной tts*/
	int error = tts_create(&tts);

	dlog_print(DLOG_INFO, "native_hello_app", "init_tts");
	/*проверка на наличие ошибок*/
	if(error_handling(error)){

		/*добавление слушателя на изменение состояния tts*/
		error = tts_set_state_changed_cb(tts, state_changed_cb, ad);
		error_handling(error);

		/*добавление слушателя на изменение состояния tts*/
		error = tts_prepare(tts);
		error_handling(error);
	}
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */

	appdata_s *ad = data;

	create_base_gui(ad);

	dlog_print(DLOG_INFO, "tag", "init_tts");
	init_tts(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
