<?php
	/***************************************************
	*
	*	-Included with every web-portal doc as redundant
	*	 import of (menu) script
	*
	***************************************************/

	{ ?>
			</body>
		</html>

		<script> 
			// Open menu
			function openNav()
			{
				document.getElementById("sideMenu").style.width = "85%";
			}

			function closeNav()
			{
				document.getElementById("sideMenu").style.width = "0";
			}

			function redirect_to_curtain(element)
			{
				var curtain_id = element.value;
				var page = window.location.href.substr(0, window.location.href.indexOf('?'));
				window.location = `${page}?curtain=${curtain_id}`;
			}

			function collapse_expand_div(button, div, button_text)
			{
				if(button.innerHTML == button_text)
				{
					button.innerHTML = "Hide";
					$(div).show();
				}
				else
				{
					button.innerHTML = button_text;
					$(div).hide();
				}
			}

			// ————————————— CURRENT STATUS —————————————

			function refresh_activation_and_percent_position()
			{
				var curtain = <?php echo $selected_curtain(); ?>;

				if(!curtain) return;
				$.ajax({
					url: "./connections/API.php",
					type: "POST",
					dataType: "json",
					data: {"get_activation_and_width" : curtain},
					success: function(response) {
						if("success" in response) return;

						var current_light = document.getElementById("current_light");
						if(response["curtain_activated"])
						{
							current_light.style["background-color"] = "#33FF33";
							current_light.style.width = '100%';
						}
						else
						{
							var width = response["curtain_percent_position"];
							current_light.style.width = `${width}%`;
							current_light.style["background-color"] = `#FFFF${Math.floor(width * 255 / 100).toString(16)}`;
						}
					}
				});
			}

			// thread that checks every second
			setInterval(function(){
				refresh_activation_and_percent_position();
			}, 1000);

		</script>

	<?php 
	}
	/* created by: MPZinke on 01.10.19 */
?>