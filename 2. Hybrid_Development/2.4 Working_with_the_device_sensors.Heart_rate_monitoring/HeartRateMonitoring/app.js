var btnHRM; 
const HRM_STATE_KEY = "hrm_state"; 
const REMOTE_SERVICE_APP_ID = "WdKRoYqwHo.heartratemonitoringservice";
const SERVICE_STOP_MESSAGE_PORT = "SERVICE_STOP_MESSAGE_PORT";
const HRM_MESSAGE_PORT = "HEART_RATE_MONITORING_MESSAGE_PORT";
var localMsgReceiver;
var watchId;

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
	
	/*Инициализация кнопки*/
	btnHRM = document.getElementById("hrm-control-btn");
	btnHRM.addEventListener('click', onBtnHRMClicked);
	
	/*Получение состояния сервиса*/
	var hrmState = localStorage.getItem(HRM_STATE_KEY);
	/*Инициализация текста кнопки*/
	if (hrmState === "STARTED") {
		btnHRM.innerText = "STOP";
	} else {
		btnHRM.innerText = "START";
	}
} 

function onBtnHRMClicked(){
	/*Получение состояния сервиса*/
	var hrmState = localStorage.getItem(HRM_STATE_KEY);
	
	/*Проверка полученного значения*/
	if (hrmState === "STARTED") {
		/*Сервис работал, пользователь нажал на кнопку остановить*/
		
		/*Изменение названия кнопки*/
		btnHRM.innerText = "START";
		
		/*Сохранение нового состояния сервиса*/
		localStorage.setItem(HRM_STATE_KEY, "STOPPED");
		
		/*Отправка сообщения об остановке сервису*/
		sendStopServiceMessage();
		
		/*Удаление слушателя сообщений от сервиса*/
		unregisterMessagePort();
	} else {
		/*Сервис не работал, пользователь нажал кнопку запустить*/
		
		/*Изменение названия кнопки*/
		btnHRM.innerText = "STOP";
		
		/*Сохранение нового состояния сервиса*/
		localStorage.setItem(HRM_STATE_KEY, "STARTED");
		
		/*Запуск сервиса*/
		startHRMServiceMonitoring();
		
		/*Добавление слушателя сообщений от сервисного приложения */
		registerMessagePort();
	}
}

function startHRMServiceMonitoring() {
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

/*Регистрация слушателя сообщений*/
function registerMessagePort(){
	localMsgReceiver = tizen.messageport.requestLocalMessagePort(HRM_MESSAGE_PORT);
	watchId = localMsgReceiver.addMessagePortListener(onReceived);
}

/* Отображение значения пульса*/
function onReceived(data) {
	var hrmValueLabel = document.getElementById("hrm-value-label");
	hrmValueLabel.innerText = data[0].value;
}

/* Отправка сообщения с командой остановки*/
function sendStopServiceMessage(){
	var requestRemotePort = tizen.messageport.requestRemoteMessagePort(REMOTE_SERVICE_APP_ID, SERVICE_STOP_MESSAGE_PORT);
	requestRemotePort.sendMessage([{key: 'Command', value: "STOP_SERVICE"}]);
}

/*Удаление слушателя на прием сообщений*/
function unregisterMessagePort(){
	localMsgReceiver.removeMessagePortListener(watchId);
} 
