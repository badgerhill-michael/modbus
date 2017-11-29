<?php

	include( "lib.php" );

	function getChartHTML( $title, $rows, $color ) {
		if( ! isset( $color ) )
			$color = 'rgba( 200, 200, 200, 0.5 )';
			
		$html = <<<EOT

<canvas id="$title" width="1080" height="150"></canvas>
<script>
var ctx = document.getElementById('$title').getContext('2d');
var scatterChart = new Chart(ctx, {
		type: 'scatter',
		data: {
				datasets: [{
						label: '$title',
						backgroundColor: '$color',
						borderColor: 'rgba( 0, 0, 0, 1.0 )',
						pointRadius: 0,
						lineTension: 0,
						data: [
EOT;

		$row = mysqli_fetch_row( $rows );
		$html .= sprintf( "{ x: moment.unix(%0.0f).local(), y: %f }\n", $row[0], $row[1] );
		while ($row = mysqli_fetch_row( $rows )) {
	$html .= sprintf( ",{ x: moment.unix(%0.0f).local(), y: %f }\n", $row[0], $row[1] );
		}

		$html .= <<<EOT
]
				}]
		},
		options: {
		responsive: true,
				scales: {
						xAxes: [{
								type: 'time',
								position: 'bottom'
						}]
				}
		}
});
</script>

EOT;
		return( $html );
}

	checkAuth( $_REQUEST["accessKey"], $_REQUEST["bucketKey"] );

	$hours				= 72;
	$system_name	= "tempc12";
	$system_id		= 0;
	$cmd					= "";
	$msg					= "";
	$device_id		= "31003e001247353136383631";
	$device_id_dev	= "280022000851353531343431";
		
	if( isset($_REQUEST["hours"]) )
		$hours = $_REQUEST["hours"];
	if( isset($_REQUEST["system_name"]) )
		$system_name = $_REQUEST["system_name"];
	if( isset($_REQUEST["system_id"]) )
		$system_id = $_REQUEST["system_id"];
	if( isset($_REQUEST["cmd"]) )
		$cmd = $_REQUEST["cmd"];
		

	$link = dbconnect();
	$fv = getTargetName( $link, 'tempc', $system_id, "T" );
	
	$rows_T    = selectSensorReports( $link, $system_name, $system_id, "T", $hours );
	if( mysqli_num_rows($rows_T) > 0 ) {
		mysqli_data_seek( $rows_T, mysqli_num_rows($rows_T)-1 );
		$row = mysqli_fetch_row($rows_T);
		$current_temp = sprintf( "%4.1f", $row[1] );
		$current_temp_units = $row[2];
		$interval = date_diff( date_create_from_format('U', $row[0] ), date_create("now") );
		$date_delta = formatDateDiff( date_create_from_format('U', $row[0] ), date_create("now") );
		$rows_T->data_seek( 1 );
	}

	$rows_SP	 = selectSensorReports( $link, $system_name, $system_id, "SP", $hours );
	mysqli_data_seek( $rows_SP, mysqli_num_rows($rows_SP)-1 );
	$row = mysqli_fetch_row($rows_SP);
	$old_setpoint = sprintf( "%4.1f", $row[1] );
	$rows_SP->data_seek( 1 );
	
	if( $cmd == "changeSetPoint" ) {
		$new_setpoint = $_REQUEST["newSetPoint"];
		$rtn = setTempSetPoint( $device_id, $system_id, $new_setpoint );
		if($rtn != 1 )
			error_log( "Setting of setpoint failed." );
		$old_setpoint = $new_setpoint;
		$msg = "Set point set to " . $old_setpoint . ". NOTE: It may take 1-2 minutes to display the new setpoint.";
	}

	$rows_OUT1   = selectSensorReports( $link, $system_name, $system_id, "OUT1", $hours );

	$html = <<<EOT
<html>
<script src="https://cdn.jsdelivr.net/npm/moment@2.19.1/moment.min.js"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.4.0/Chart.min.js"></script>
<link href='https://fonts.googleapis.com/css?family=Lato' rel='stylesheet'>
<style>
body {
    font-family: 'Lato';
    font-size: 16px;
}
#myDIV1 {
    width: 200px;
    height: 100px;
    color: darkred;
    display:inline;
    border: thin solid lightgrey;
    font-size: 36px;
}
#myDIV2 {
    width: 200px;;
    height: 100px;
    color: green;
    display:inline;
    font-size: 24px;
}
#myDIV3 {
    width: 200px;;
    height: 100px;
    color: black;
    display:inline;
    border: thin dashed lightgrey;
    font-size: 18px;
}</style>
<title>$fv Temperature</title>
<body>
<div id="myDIV1"><strong>$fv Temperature</strong><br/></div>
<div id="myDIV3">Displaying the last <form id="frm1" action="" style="display:inline">
<input type="text" name="hours" value="$hours" size=5>
<input type="hidden" name="accessKey" value="Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6">
<input type="hidden" name="bucketKey" value="KTYPDWRBG8S8">
<input type="hidden" name="system_name" value="$system_name">
<input type="hidden" name="system_id" value="$system_id">
</form> hours<br/></div>
<p></p>
<div id="myDIV3"><strong>Latest Temperature:</strong></div>
	<div id="myDIV2">$current_temp $current_temp_units</div>
	($date_delta)<br/>
	<div id="myDIV3"><strong>Setpoint:</strong></div><form id="frm2" action="" style="display:inline">
	<input type="text" name="newSetPoint" value="$old_setpoint" size=5>
	<input type="hidden" name="accessKey" value="Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6">
	<input type="hidden" name="hours" value="$hours" size=5>
	<input type="hidden" name="cmd" value="changeSetPoint">
	<input type="hidden" name="bucketKey" value="KTYPDWRBG8S8">
	<input type="hidden" name="system_name" value="$system_name">
	<input type="hidden" name="system_id" value="$system_id">
	</form> $current_temp_units
<p>$msg</p>
EOT;

	$html .= getChartHTML( "Temperature", $rows_T, 'rgba( 200, 100, 100, 0.5 )' );
	$html .= getChartHTML( "Setpoint", $rows_SP, 'rgba( 100, 200, 100, 0.5 )' );
	$html .= getChartHTML( "Cooling",	$rows_OUT1, 'rgba( 100, 100, 200, 0.5 )' );

	$html .= <<<EOT
</body>
</html>
EOT;

	print($html);
	mysqli_free_result($rows_T);
	mysqli_free_result($rows_SP);
	mysqli_free_result($rows_OUT1);
	mysqli_close($link);
?>
