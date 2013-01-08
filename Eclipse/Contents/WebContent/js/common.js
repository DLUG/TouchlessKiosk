//11.15 add taehwan
$(document).keydown(function(event) {
	if(event.which == 72) {
		location.href="./";
	} else if (event.which == 77){
		location.href="./main.html";
	};
});


var intervalId;
$(document).ready(function(){
	settingTimeout();
});
function settingTimeout() {
	intervalId = setTimeout(reset, 1000 * 60 * 10);
}
function reset() {
	location.href = "./ads.html";
}
function resetTimeout() {
	clearTimeout(intervalId - 1);
	clearTimeout(intervalId);
}
function backButton() {
	resetTimeout();
	settingTimeout();
	location.href = "./main.html";
}