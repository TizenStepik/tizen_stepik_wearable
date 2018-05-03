var REMOTE_SERVICE_APP_ID = "zj6ij6pcUW.speechtotextservice";
var RECORD_STATE_MESSAGE_PORT = "RECORD_STATE_MESSAGE_PORT";
var RECORD_CONTROL_MESSAGE_PORT = "RECORD_CONTROL_MESSAGE_PORT";
var STT_RESULT_MESSAGE_PORT = "STT_RESULT_MESSAGE_PORT";

var localStateMsgReceiver;
var localResultMsgReceiver;
var stateWatchId;
var resultWatchId;
var btnRecordControl;
var requestRemotePort;

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
					
					/*Удаление слушателей портов*/
					localStateMsgReceiver.removeMessagePortListener(stateWatchId);
					localResultMsgReceiver.removeMessagePortListener(resultWatchId);
					
					tizen.application.getCurrentApplication().exit();
				} catch (ignore) {
				}
			} else {
				window.history.back();
			}
		}
	});
	
	window.onload = init();
	console.log("init called");
}());

function init(){
	
	console.log("init called");
	/*запуск сервиса*/
	startSpeechToTextService();
	
	/*регистрация порта для получения состояний функций tts*/
	registerStateMessagePort();
	
	/*регистрация порта для получения результата распознавания речи*/
	registerResultMessagePort();
	
	/*инициализация кнопки управления функциями tts*/
	btnRecordControl = document.getElementById("record-state-button");
	btnRecordControl.addEventListener('click', function(){
		if (btnRecordControl.innerText === "START") {
			
			/*Отправка сообщения о старте записи речи*/
			sendStartRecordMessage();
			
		} else if(btnRecordControl.innerText === "STOP") {
			
			/*Отправка сообщения об остановке записи речи*/
			sendStopRecordMessage();
			
		}
	});
	
} 

function startSpeechToTextService() {
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
	
	requestRemotePort = tizen.messageport.requestRemoteMessagePort(REMOTE_SERVICE_APP_ID, RECORD_CONTROL_MESSAGE_PORT);
}

/*Выполняется при ошибке запуска сервиса*/
function serviceAppErrorStartedCallback(e){
	console.log("Service not started : " + e.message);
} 

function registerStateMessagePort(){
	
	localStateMsgReceiver = tizen.messageport.requestLocalMessagePort(RECORD_STATE_MESSAGE_PORT);
	stateWatchId = localStateMsgReceiver.addMessagePortListener(onReceivedState);
	
}

function onReceivedState(data){
	
	/* Получаем сообщение о состоянии сервиса */
	var state = data[0].value;
	
	
	if (state === "READY") { // Сервис готов записывать речь
		btnRecordControl.innerText = "START";
	} else if (state === "RECORDING"){ // Сервис записывает речь
		btnRecordControl.innerText = "STOP";
	}
}

function registerResultMessagePort(){
	localResultMsgReceiver = tizen.messageport.requestLocalMessagePort(STT_RESULT_MESSAGE_PORT);
	resultWatchId = localResultMsgReceiver.addMessagePortListener(onReceivedResult);
}

function onReceivedResult(data){
	
	/*Получение результата*/
	var textLabel = document.getElementById("text-label");
	
	/*Вывод результата на экран*/
	textLabel.innerText = data[0].value;
}

function sendStartRecordMessage(){
	
	/*Отправка сообщения о старте записи речи*/
	requestRemotePort.sendMessage([{key: 'Command', value: "START_RECORD"}]);
}

function sendStopRecordMessage(){
	
	/*Отправка сообщения об остановке записи речи*/
	requestRemotePort.sendMessage([{key: 'Command', value: "STOP_RECORD"}]);
}