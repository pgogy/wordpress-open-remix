<?PHP

	class openstart_settings{

		public function __construct()
		{
			add_action( 'admin_menu', array( $this, 'add_plugin_page' ) );
			$dir = opendir(dirname(__FILE__) . "/types/");
			while($type = readdir($dir)){
				if($type != "." && $type !=".."){
					require_once(dirname(__FILE__) . "/types/" . $type . "/" . $type . ".inc");
					$option = new $type();
					$option->ajax();
					add_action('admin_enqueue_scripts', array($option, 'styles') );
					add_action('admin_enqueue_scripts', array($option, 'scripts') );
				}
			}
		}
		
		public function add_plugin_page(){
		
			add_menu_page('Open Start', 'Open Start', 'manage_options', 'openstart-admin', array($this,'options'));
			add_submenu_page( 'openstart-admin', 'Harvest', 'Harvest', 'manage_options', 'openstart-get-content', array($this, 'get_content'));
			
		}
		
		function options(){
		
			?>
				<h2>TranscribePress : Welcome!</h2>
				<p>TranscribePress is a tool designed to help people make image based texts available for transcription to people.</p>
				<p>There are two pages for settings</p>
				<ol>
					<li>Settings - this configures the tool itself.</li>
					<li>Subscribers - this allows you to manage subscribers to the email list.</li>
					<li>Email Subscribers - send an email to subscribers.</li>
					<li>Social Media - set up social media elements.</li>
					<li>Random Transcription - set up the random transcription page</li>
					<li>Statistics - show details of user's approvals of submissions</li>
				</ol>
			<?PHP
		
		}
		
		public function get_content(){
		
			?>
			<div class="wrap">
			<?php screen_icon(); 
			
			if(isset($_POST['site_type'])){
			
				if(file_exists(dirname(__FILE__) . "/types/" . $_POST['site_type'] . "/" . $_POST['site_type'] . ".inc")){				
					require_once(dirname(__FILE__) . "/types/" . $_POST['site_type'] . "/" . $_POST['site_type'] . ".inc");
					$option = new $_POST['site_type']();
					$option->get_data();
				}
			
			}else{
			
				?><h2>Get Open Content</h2> 
				<form method="post" action="">
				<?PHP
				
					$dir = opendir(dirname(__FILE__) . "/types/");
					while($type = readdir($dir)){
						if($type != "." && $type !=".."){
							require_once(dirname(__FILE__) . "/types/" . $type . "/" . $type . ".inc");
							$option = new $type();
							?><input type="submit" name="site_type" value="<?PHP echo $option->get_value(); ?>" label="Get <?PHP echo $option->get_name(); ?> content" /><?PHP
						}
					}
				
				?>
				</form>
			</div>
			<?php
			
			}
			
		}
		
	}

	
	$openstart_settings = new openstart_settings;
	