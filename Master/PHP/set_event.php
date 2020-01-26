<?php
	/***************************************************
	*
	*	-Main page of local portal to curtains
	*	-Input upcoming events to db
	*
	***************************************************/

	include_once ($_SERVER['DOCUMENT_ROOT'].'/dependencies/header.php');

	if($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['set_event_btn'])) {
		$date = filter_input(INPUT_POST, 'date_input');
		$time = filter_input(INPUT_POST, 'time_input');
		$action = filter_input(INPUT_POST, 'action');
		$event_time = date('Y-m-d H:i:s', strtotime($date.$time));

		if(set_future_event($action, $event_time)) {
			echo '<script>'.
					'alert("Successfully created event");'.
					'window.location.href = "upcoming.php";'.
				'</script>';
		}
		else {
			echo '<script>'.
					'alert("Could not set event");'.
					'window.location.href = "upcoming.php";'.
				'</script>';
		}
	}

	{ ?>

	<form method='POST'>
		<div id='set_event_title' class='title'> 
			<h2>Create Event</h2>
		</div>
		<div id='date' class='v_space-20'>
			<input type='date' name='date_input' class='input' id='date_input' onchange='enable_button()' value="<?php echo date('Y-m-d', strtotime("+1 day")); ?>" min="<?php echo date('Y-m-d'); ?>" max="<?php echo date('Y-m-d', strtotime("+1 week")); ?>" />
		</div>
		<div id='time' class='v_space-20'>
			<input type='time' class='input' name='time_input' id='time_input' onchange='enable_button()'/>
		</div>
		<div class='v_space-20'>
			<select name='action' id='action' class='input' onchange='enable_button()'>
				<option selected='selected' value="" disabled>State</option>
				<option value='O'>Open</option>
				<option value='C'>Close</option>
			</select>
		</div>
		<div id='set_btn' class='v_space-20'>
			<button name='set_event_btn' id='set_event_btn' class='button' disabled>Set Event</button>
		</div>
	</form>

	<script>
		function enable_button() {
			var date = document.getElementById('date_input').value;
			var time = document.getElementById('time_input').value;
			var select = document.getElementById('action');
			var action = select.options[select.selectedIndex].value;

			if(date !== '' && time !== '' && action !== '') {
				document.getElementById('set_event_btn').disabled = false;
			}
		}
	</script>

<?php }
	include_once ($_SERVER['DOCUMENT_ROOT'].'/dependencies/footer.php');
	/* created by: MPZinke on 01.10.19 */
?>
