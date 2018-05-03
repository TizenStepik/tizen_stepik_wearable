#include "nativehelloapp.h"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
	Evas_Object *btn; // кнопка
	Evas_Object *box; // контейнер
	Evas_Object *entry; // поле для ввода
} appdata_s;

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

/* обработка нажатия на кнопку*/
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

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad);

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
