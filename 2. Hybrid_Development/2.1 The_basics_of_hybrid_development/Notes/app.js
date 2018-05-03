/* global tau*/

var notes = [];
var list;
var REMOTE_SERVICE_APP_ID = "5zfZeMzxoW.notesservice";
var ADD_NOTE_MESSAGE_PORT = "ADD_NOTE_MESSAGE_PORT";

(function () {
	window.addEventListener("tizenhwkey", function (ev) {
		var activePopup = null,
			page = null,
			pageid = "";

		if (ev.keyName === "back") {
			activePopup = document.querySelector(".ui-popup-active");
			page = document.getElementsByClassName("ui-page-active")[0];
			pageid = page ? page.id : "";

			if (pageid === "main" && !activePopup) {
				try {
					
					tizen.application.getCurrentApplication().exit();
				} catch (ignore) {
				}
			} else {
				window.history.back();
			}
		}
	});
	
	window.onload = init();
}());

function init() {
	
	/*Инициализация кнопки добавления заметки*/
	var addNoteButton = document.getElementById("add-note-button");
	
	/*Добавление слушателя на нажатие на кнопку*/
	addNoteButton.addEventListener("click", onAddNoteButtonClicked);
	
	/*Инициализация списка*/
	list = document.getElementById("notes-list");
	
	/*Запуск сервиса*/
	startService();
	
}

function onAddNoteButtonClicked() {
	/*Инициализация страницы добавления заметки*/
	initAddNotePage();
	
	/*Переход на экран создания заметки*/
	tau.changePage("#create-note");
}

function initAddNotePage(){
	/*Инициализация кнопки создания заметки*/
	var createNoteText = document.getElementById("create-note-button");
	
	/*Добавление слушателя на нажатие на кнопку*/
	createNoteText.addEventListener("click", onCreateNoteBtnClicked);
}

function onCreateNoteBtnClicked(){
	
	/*Получение элементов интерфейса для ввода данных*/
	var inputNoteTitle = document.getElementById("input-note-title");
	var inputNoteText = document.getElementById("input-note-text");
	
	var note_title = inputNoteTitle.value;
	var note_text = inputNoteText.value;
	
	//Проверка заполненности полей текстом
	if (note_title && note_text) {
		//Добавляем сообщение в список
		addNote(note_title, note_text);
		//Возвращение на предыдущую страницу со списком уведомлений
		window.history.back();
		
		//Очистка форм для ввода данных 
		inputNoteTitle.value = "";
		inputNoteText.value = "";
		
		/*Создание порта для связи с нативным приложением*/
		var requestRemotePort = tizen.messageport.requestRemoteMessagePort(REMOTE_SERVICE_APP_ID, ADD_NOTE_MESSAGE_PORT);
		
		/*Отправка сообщения нативному приложению*/
		requestRemotePort.sendMessage([{key: 'note_title', value: note_title }, {key: 'note_text', value: note_text}]);
	}
}

function addNote(note_title, note_text) {
	//Создание объекта уведомления
	var note = {
			"title": note_title,
			"text": note_text
	};
	
	//Добавление уведомления в конец списка
	notes.push(note);
	//Создание элемента li для отображения пункта списка  на экране
	var item = document.createElement("li");
	item.innerText = note.title;
	
	/*Добавление обработчика нажатия на элемент списка*/    
	item.addEventListener('click', function() {
		
		/*Инициализация страницы детального отображения заметки*/
		initNoteDetailPage(note.title, note.text);
		
		/*Переход на детальное отображение заметки*/
		tau.changePage("#note-detail");
	});
	
	//Добавление элемента в список на экране
	list.appendChild(item);
}

function initNoteDetailPage(noteTitle, noteText){
	/*Инициализация заголовка заметки*/
	var noteDetailTitle = document.getElementById("note-detail-title");
	noteDetailTitle.innerText = noteTitle;
	
	/*Инициализация текста заметки*/
	var noteTextLabel = document.getElementById("note-detail-label");
	noteTextLabel.innerText = noteText;
} 


function startService() {
	try {
		
		/*Создание  объекта appControl для  управления сервисом*/
		var appControl = new tizen.ApplicationControl( "http://tizen.org/appcontrol/operation/service", null, null, null, null);
		
		/*Запуск сервисного приложения*/
		tizen.application.launchAppControl(appControl, REMOTE_SERVICE_APP_ID, serviceAppSuccessStartedCallback, serviceAppErrorStartedCallback, null);
	
	} catch (exc) {
		console.error('Exception while launching HybridServiceApp: ' + exc.message);
	}
}

/*Выполняется при успешном запуске сервиса*/
function serviceAppSuccessStartedCallback() {
	console.log("Service started");
}

/*Выполняется при ошибке запуска сервиса*/
function serviceAppErrorStartedCallback(e){
	console.log("Service not started : " + e.message);
}
