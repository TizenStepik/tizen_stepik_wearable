/* global tau*/


var notes = [];
var list;

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
	
	//Проверка заполненности полей текстом
	if (inputNoteTitle.value && inputNoteText.value) {
		
		/*Добавление сообщения в список*/    
		addNote(inputNoteTitle.value, inputNoteText.value);
		
		//Возвращение на предыдущую страницу со списком уведомлений
		window.history.back();
		
		//Очистка форм для ввода данных 
		inputNoteTitle.value = "";
		inputNoteText.value = "";
	}
}

function addNote(note_title, note_text){
	
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
