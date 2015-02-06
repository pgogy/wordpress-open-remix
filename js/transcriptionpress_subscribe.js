function transcription_subscribe(){ 

	jQuery(document).ready(function($) {
	
		var data = {
			action: "transcriptionpress_subscribe",
			transcription:document.getElementById("subscriber_text").value,
			nonce: transcriptionpress_subscribe.answerNonce
		};
		
		jQuery.post(transcriptionpress_subscribe.ajaxurl, data, function(response) {
			document.getElementById("transcription_subscribe").innerHTML = "Thanks for your subscription";	
		});

	});
	
}