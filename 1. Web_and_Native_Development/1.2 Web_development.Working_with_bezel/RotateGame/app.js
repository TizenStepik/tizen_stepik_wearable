/*global tau */

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

function init(){
	/* Инициализация кнопки запуск игры*/
	var btnStart = document.getElementById("btn-start-game");
	
	/*Добавление слушателя на нажатие кнопки*/
	btnStart.addEventListener('click', onBtnStartClicked);
}

function onBtnStartClicked() {
	/*Запуск игры*/
	startGame();
	
	/*Переход на страницу игры*/
	tau.changePage("#page-game");
}

function startGame(){
	var labelCommand = document.getElementById("label-command");
	var labelScore = document.getElementById("label-score");
	var score = 0;
	
	/*Вывод на экран первой команды*/
	labelCommand.innerText = generateCommand();
	
	/*Добавление слушателя на поворот безели*/
	document.addEventListener('rotarydetent', function(event){
		 if (event.detail.direction === "CW" && labelCommand.innerText === "Right" || 
				 event.detail.direction === "CCW" && labelCommand.innerText === "Left") { 
			 
			 		/*Добавление очков*/
					score++;
					
					/*Обновление значения набранных очков на экране*/
					labelScore.innerText = score;
					
					/*Вывод на экран следующей команды*/
					labelCommand.innerText = generateCommand();
		} else {
			/*Вывод на экран сообщения о проигрыше*/
			alert("Game Over");
			
			/*Инициализация  страницы с результатами*/
			initResultPage(score);
			
			/*Запуск страницы с результатами*/
			tau.changePage("#page-score");
		 }
	});
}

function generateCommand(){
	/*Генерация случайного числа от 0 до 1*/
	var num = Math.random();
	
	if (num > 0.5){
		return "Right";
	} else {
		return "Left";
	}
} 

function initResultPage(score){
	/*Вывод заработанных очков на экран*/
	var labelResultScore = document.getElementById("label-result-score");
	labelResultScore.innerText = "Your score is " + score;
	
	/*Инициализация кнопки выхода из приложения*/
	var btnOK = document.getElementById("btn-ok");
	btnOK.addEventListener('click', onOKBtnClicked);
}

function onOKBtnClicked(){
	tizen.application.getCurrentApplication().exit();
}