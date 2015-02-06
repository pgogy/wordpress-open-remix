<?PHP

	class wikieducator{
	
		var $page_object;
		
		function ajax(){
		
			add_action( 'wp_ajax_wikieducator_harvest', array($this, 'wikieducator_harvest') );
			add_action( 'wp_ajax_wikieducator_create_menu', array($this, 'create_menu') );
		
		}
		
		function styles(){
		
			wp_register_style( 'wikieducator_admin_css', plugins_url( '/css/wikieducator_admin.css' , __FILE__ ), false, '1.0.0' );
			wp_enqueue_style( 'wikieducator_admin_css' );
		
		}
		
		function scripts(){
		
			wp_enqueue_script( 'wikieducator', plugins_url('/js/wikieducator.js', __FILE__), array('jquery'));	
			wp_localize_script( 'wikieducator', 'wikieducator', array( 'ajaxurl' => admin_url( 'admin-ajax.php' ), 'answerNonce' => wp_create_nonce( 'wikieducator_nonce' ) ) );
			
		}
	
		public function get_name(){
		
			return "WikiEducator";
		
		}
		
		public function get_value(){
		
			return "wikieducator";
		
		}
		
		public function create_menu(){
		
			if(wp_verify_nonce($_POST['nonce'], "wikieducator_nonce")){
			
				global $wpdb;
			
				$the_query = new WP_Query( 'wiki_this_harvest=1&posts_per_page=9999&post_type=page' );
				
				if ( $the_query->have_posts() ) {
				
					while ( $the_query->have_posts() ) {
						$the_query->the_post();
						$id = get_the_id();
						
						$parent = get_post_meta($id, "wiki_parent");
						
						echo "PARENT IS " . $parent[0] . " " . get_the_title() . "\n";
						
						if($parent[0]!=1){
						
							$querystr = "SELECT * from $wpdb->postmeta where meta_key = 'wiki_page_id' and meta_value=" . $parent[0] ;
							$page = $wpdb->get_results($querystr, OBJECT);
							
							$my_post = array(
								'ID'           => $id,
								'post_parent' => $page[0]->post_id
							);

							wp_update_post( $my_post );
							
							echo "parent set";
					
						}
						
						$prev = get_post_meta($id, "wiki_prev");
						
						$querystr = "SELECT * from $wpdb->postmeta where wiki_page_id=" . $prev[0] ;
						$page = $wpdb->get_results($querystr, OBJECT);
						
						update_post_meta($id, "wiki_prev", $page[0]);
						
						$next = get_post_meta($id, "wiki_next");
						
						$querystr = "SELECT * from $wpdb->postmeta where wiki_page_id=" . $next[0] ;
						$page = $wpdb->get_results($querystr, OBJECT);
						
						update_post_meta($id, "wiki_next", $page[0]);
						
					}
				}
				
				$wpdb->delete($wpdb->postmeta, array("meta_key" => "wiki_last_harvest"));	
				$wpdb->delete($wpdb->postmeta, array("meta_key" => "wiki_page_id"));	
			
			}
			
		}			
		
		public function wikieducator_harvest(){
		
			if(wp_verify_nonce($_POST['nonce'], "wikieducator_nonce")){
		
				$url_parts = explode("/", $_POST['base']);
				array_pop($url_parts);
				array_pop($url_parts);
				
				require_once(dirname(__FILE__) . "/../../lib/simple_html_dom.php");
				
				echo $_POST['url'] . " ---- \n";
				
				$url = implode("/", $url_parts) . $_POST['url'];
				
				$html = file_get_html($url);
				
				$this->page_data = "";
				
				foreach($html->find('div[id=content]') as $child){
					$this->page_data = $child;
				}
				
				file_put_contents(dirname(__FILE__) . "/temp/data.txt", $this->page_data);
				
				$this->page_data = file_get_contents(dirname(__FILE__) . "/temp/data.txt");
				
				$this->replacement_rules();
				
				$this->create_post();
				
			}else{
			
				echo "ERROR";
			
			}
			
			wp_die();
		
		}
		
		public function replacement_rules(){
		
			$dom = new DOMDocument('1.0', 'utf-8');
			@$dom->loadHTML($this->page_data);

			$xpath = new DOMXPath($dom);
			$remove_class = array(
								"printfooter",
								"catlinks",
								"firstHeading",
								"magnify",
								"magnify",
								"printfooter",
								"weMenubar",
								"weCourseTitle",
								"navigation"
							);         

			//#footer (after copying bits of it into the new footer)
			
			foreach($remove_class as $remove){
				$content = $xpath->evaluate("//*[contains(@class, '" . $remove . "')]");
				foreach($content as $elem){
					$elem->parentNode->removeChild($elem);
				}
			}
			
			$remove_id = array(
								"siteSub",
								"contentSub",
								"jump-to-nav",
								"toc",
								"mw-head",
								"mw-page-base",
								"mw-head-base",
								"globalWrapper",
								"firstHeading",
								"mw-navigation",
								"mw-js-message",
								"mw-panel",
								"siteNotice",
								"contentSub",
								"catlinks",
								"footer-places-disclaimer",
								"footer-places-about",
								"footer-info-lastmod",
								"footer-info-viewcount",
								"footer-poweredbyico",
								"top"
							);
							
			foreach($remove_id as $remove){
				$content = $xpath->evaluate("//*[contains(@id, '" . $remove . "')]");
				foreach($content as $elem){
					$elem->parentNode->removeChild($elem);
				}
			}
			
			$content = $xpath->evaluate("//script");
			foreach($content as $elem){
				$elem->parentNode->removeChild($elem);
			}
		
			
			$this->page_data = $dom->saveHTML();
			
			$this->page_data = str_replace('src="/images','src="http://wikieducator.org/images', $this->page_data);
			
			$this->page_data = str_replace('<div id="bodyContent">','<div class="row"><div class="col-md-12"><div class="panel"><div class="panel-body"><div id="bodyContent">', $this->page_data);
			
			$this->page_data = str_replace('<div id="content" class="mw-body" role="main">', '<div id="content" class="mw-body container" role="main">', $this->page_data);
			
			$this->page_data = str_replace('<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "http://www.w3.org/TR/REC-html40/loose.dtd">', '', $this->page_data);
			$this->page_data = str_replace('<html><body>', '', $this->page_data);

			$this->page_data = str_replace('$(', 'jQuery(', $this->page_data);
			
			$this->page_data .= "</div></div></div></div>";
			
			$this->page_data = preg_replace ('/style\="[a-zA-z0-9\-\:\;]*"/', "" , $this->page_data); 
					
		}
		
		public function create_post(){
			
			$post = array(
				'post_content'   => $this->page_data,
				'post_name'      => $_POST['title'],
				'post_title'     => $_POST['title'],
				'post_status'    => 'publish',
				'post_type'      => 'page'
			);
			
			$post_id = wp_insert_post($post);
			
			add_post_meta($post_id, "wiki_this_harvest", 1); 
			add_post_meta($post_id, "wiki_parent", $_POST['parent']); 
			add_post_meta($post_id, "wiki_prev", $_POST['prev']); 
			add_post_meta($post_id, "wiki_next", $_POST['next']); 
			add_post_meta($post_id, "wiki_page_id", $_POST['page_id']);
		
		}
		
		public function get_data(){
		
			if(isset($_POST['fetch_url'])){
			
				$this->get_content();
				$this->url = $_POST['fetch_url'];
				$this->page_list = array();
				?>
				<h1>Get Wikieducator Content</h1>
				<p>Pages found at <?PHP echo $_POST['fetch_url']; ?></p>
				<button id="harvest_list">Harvest Selected</button>
				<button id="check_all_list">Check All</button>
				<button id="uncheck_all_list">Uncheck All</button>
				<div id='wikieducator_list'>
				<?PHP
				$this->render_menu($this->page_object, 0, 1, 0);
				echo $this->output;
				?></div><?PHP
				
			}else{
		
				?><h2>Get WikiEducator Content</h2>
				<form action="" method="POST">
				<input name="fetch_url" value="Enter URL here" size="100" />
				<input name="site_type" type="hidden" value="wikieducator" />
				<input type="submit" value="Get content" />
				</form><?PHP
			
			}
		
		}
		
		public function make_page_object($data){
			
			$children = explode("<li>", $data);
			
			array_shift($children);
			
			$level = 0;
			$first_level = TRUE;
			$this->page_object = new StdClass();
			$pointer = $this->page_object;
			$this->counter = 1;
			
			foreach($children as $child){
			
				$data = new StdClass();
			
				if(strpos($child, "<ul>")!==FALSE){
					$href = explode('href="', $child);
					if(count($href)!==1){
						$href = explode('"', $href[1]);
						$href = $href[0];
						$content = explode(">", $child);
						$content = explode("<", $content[1]);
						$content = $content[0];
					}else{
						$href = "";
						$content = explode("<ul>", $child);
						$content = trim($content[0]);
					}
					$data->href = $href;
					$data->content = $content;
					$old_pointer = $pointer;
					$pointer->{$content} = $data;
					$pointer = $pointer->{$content};
					$pointer->children = new StdClass();
					$pointer = $pointer->children;
					$pointer->parent = $old_pointer;
					$level++;
				
				}else if(strpos($child, "</li> </ul> </li>") !== FALSE){
					$href = explode('href="', $child);
					$href = explode('"', $href[1]);
					$href = $href[0];
					$content = explode(">", $child);
					$content = explode("<", $content[1]);
					$content = $content[0];
					
					$data->href = $href;
					$data->content = $content;
					$data->children = new StdClass();
					$pointer->{$content} = $data;
					
					$check = explode("</ul>",$child);
					
					for($x=0;$x!=(count($check)-1);$x++){
						$pointer = $pointer->parent;
					}
					
				}else{
					$href = explode('href="', $child);
					$href = explode('"', $href[1]);
					$href = $href[0];
					$content = explode(">", $child);
					$content = explode("<", $content[1]);
					$content = $content[0];
					
					$data->href = $href;
					$data->content = $content;
					$data->children = new StdClass();
					if(trim($content)!=""){
						$pointer->{trim($content)} = $data;
					}
					
				}
			}
			
		}
		
		public function get_content(){
		
			if(isset($_POST['fetch_url'])){
				
				require_once(dirname(__FILE__) . "/../../lib/simple_html_dom.php");
				
				$html = file_get_html($_POST['fetch_url']);
				
				$content = $html->find('div[id=mw-content-text]');
				
				foreach($content as $node){
				
					$data = $node->innertext;
				
				}
				
				$this->make_page_object($data);
			
			}
			
		}
		
		public function render_menu($content, $depth, $parent){
		
			foreach($content as $key){
				if($key->content!=""){
					$this->page_list[$counter] = array($key);
					
					$this->output .= "<p><input type='checkbox' parent='" . $parent . "' title='" . addslashes($key->content) . "' page_id='" . $this->counter . "' prev='" . ($this->counter - 1) . "' next='" . ($this->counter + 1) . "' base='" . $this->url . "' checked value='" . $key->href . "' />";
					for($x=0; $x!=$depth; $x++){
						$this->output .= " ---- ";
					}
					$this->output .= $key->content . "<br />";
					$this->output .= "</p>";
					$new_parent = $this->counter;
					$this->counter+=1;
					if(isset($key->children)){
						$this->render_menu($key->children, $depth+1, $new_parent);
					}
				}
			}
		
		}

		public function form(){
	
			?>
			<div class="wrap">
				<?php screen_icon(); ?>
				<h2>Get WikiEducator Content</h2>           
				<form method="post" action="">
				</form>
			</div>
			<?php
			
		}
	
	}