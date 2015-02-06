function fetch_wikieducator_page(list){
	elem = jQuery(list).first();
	if(jQuery(elem).attr("checked")!=undefined){
		var data = {
			'url': jQuery(elem).val(),
			'base': jQuery(elem).attr("base"),
			'title': jQuery(elem).attr("title"),
			'parent': jQuery(elem).attr("parent"),
			'next': jQuery(elem).attr("next"),
			'prev': jQuery(elem).attr("prev"),
			'page_id': jQuery(elem).attr("page_id"),
			nonce : wikieducator.answerNonce,
			action : "wikieducator_harvest"
		};

		list = jQuery(list).splice(1,jQuery(list).length-1);
	
		jQuery.post(wikieducator.ajaxurl, data, function(response) {
			console.log(response);
			jQuery(elem).parent().css("background","#f00");
			fetch_wikieducator_page(list);
			if(jQuery(list).length==0){
				alert("Fetching Complete");
				build_wikieducator_menu();
			}
		});
		
	}

}

function build_wikieducator_menu(){

	var data = {
		nonce : wikieducator.answerNonce,
		action : "wikieducator_create_menu"
	};
	
	jQuery.post(wikieducator.ajaxurl, data, function(response) {
		console.log(response);
	});

}

jQuery(document).ready(
	function(){
		jQuery("#harvest_list")
			.click(
				function(){
					data_list = jQuery("#wikieducator_list p input:checked");	
					console.log(data_list);
					fetch_wikieducator_page(data_list);
				}
			);
			
		jQuery("#check_all_list")
			.click(
				function(){
					jQuery("#wikieducator_list p input")
						.each(
							function(index,value){
								jQuery(value)
									.attr('checked','checked');
							}
						);
				}
			);
			
		jQuery("#uncheck_all_list")
			.click(
				function(){
					jQuery("#wikieducator_list p input")
						.each(
							function(index,value){
								jQuery(value)
									.removeAttr('checked');
							}
						);
				}
			);
	}
);	