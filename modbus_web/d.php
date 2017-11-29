<?php

function formatDateDiff($start, $end=null) { 
    if(!($start instanceof DateTime)) { 
        $start = new DateTime($start); 
    } 
    
    if($end === null) { 
        $end = new DateTime(); 
    } 
    
    if(!($end instanceof DateTime)) { 
        $end = new DateTime($start); 
    } 
    
    $interval = $end->diff($start); 
    $doPlural = function($nb,$str){return $nb>1?$str.'s':$str;}; // adds plurals 
    
    $format = array(); 
    if($interval->y !== 0) { 
        $format[] = "%y ".$doPlural($interval->y, "year"); 
    } 
    if($interval->m !== 0) { 
        $format[] = "%m ".$doPlural($interval->m, "month"); 
    } 
    if($interval->d !== 0) { 
        $format[] = "%d ".$doPlural($interval->d, "day"); 
    } 
    if($interval->h !== 0) { 
        $format[] = "%h ".$doPlural($interval->h, "hour"); 
    } 
    if($interval->i !== 0) { 
        $format[] = "%i ".$doPlural($interval->i, "minute"); 
    } 
    if($interval->s !== 0) { 
        if(!count($format)) { 
            return "less than a minute ago"; 
        } else { 
            $format[] = "%s ".$doPlural($interval->s, "second"); 
        } 
    } 
    
    // We use the two biggest parts 
    if(count($format) > 1) { 
        $format = array_shift($format)." and ".array_shift($format); 
    } else { 
        $format = array_pop($format); 
    } 
    
    // Prepend 'since ' or whatever you like 
    return $interval->format($format . " ago"); 
}

function setTempSetPoint( $device, $system_id, $setpoint ) {
		$url = "https://api.particle.io/v1/devices/" . $device . "/setTempCSP";
		error_log($url);
		$data = array(
			'access_token'	=> '11f269af8c894898b5f297dec4b332b1940c4907',
			'command'		=> $system_id . ":" . $setpoint
		);
		$options = array(
				'http' => array(
				'header'  => "Content-type: application/x-www-form-urlencoded\r\n",
				'method'  => 'POST',
				'content' => http_build_query($data),
			)
		);
		$context  = stream_context_create($options);
		$result = json_decode( file_get_contents( $url, false, $context ) );
		return( $result->return_value );
	}
	
	function checkAuth( $aKey, $bKey) {
		if( $aKey != "Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6" ) {
			die("Not found");
		}
		else if( $bKey != "KTYPDWRBG8S8" ) {
				die("Not found");
		}
	}

	function selectSensorReports( $link, $system_name, $system_id, $parameter_name, $hours ) {
		if( $hours < 1 )
			$hours = 24;
		$sql = "SELECT date_recorded, parameter_value FROM	sensor_reports WHERE system_name = \"$system_name\" AND system_id = \"$system_id\" AND parameter_name = \"$parameter_name\" AND parameter_value < 900000 AND date_received >= now() - INTERVAL $hours HOUR ORDER BY date_recorded ASC";
		error_log( $sql );
		$result = mysqli_query( $link, $sql );
			

		if( $result )
			return( $result );
		else{
			echo "ERROR: Could not execute $sql. " . mysqli_error($link);
			return(-1);
		}
	}

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
		$html .= sprintf( "{ x: moment.unix(%0.0f), y: %f }\n", $row[0], $row[1] );
		while ($row = mysqli_fetch_row( $rows )) {
	$html .= sprintf( ",{ x: moment.unix(%0.0f), y: %f }\n", $row[0], $row[1] );
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

	date_default_timezone_set( "America/Chicago" );	
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
	
	$fv = $system_id - 9;
	
	$link = mysqli_connect("localhost", "badgerhi_mkmb", 'F52+@;~L9n$y', "badgerhi_modbus");
	if($link === false){
			die("ERROR: Could not connect. " . mysqli_connect_error());
	}
	
	$rows_T    = selectSensorReports( $link, $system_name, $system_id, "T", $hours );
	if( mysqli_num_rows($rows_T) > 0 ) {
		mysqli_data_seek( $rows_T, mysqli_num_rows($rows_T)-1 );
		$row = mysqli_fetch_row($rows_T);
		$current_temp = sprintf( "%4.1f", $row[1] );
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
		$rtn = setTempSetPoint( $device_id_dev, $system_id, $new_setpoint );
		if($rtn != 1 )
			error_log( "Setting of setpoint failed." );
		$old_setpoint = $new_setpoint;
		$msg = "Set point set to " . $old_setpoint;
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
    background-color: darkred;
    color: white;
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
    border: thin solid lightgrey;
    font-size: 18px;
}</style>
<body>
<div id="myDIV1"><strong>FV$fv Temperature</strong><br/></div>
<div id="myDIV3">Displaying the last <form id="frm1" action="" style="display:inline">
<input type="text" name="hours" value="$hours" size=5>
<input type="hidden" name="accessKey" value="Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6">
<input type="hidden" name="bucketKey" value="KTYPDWRBG8S8">
<input type="hidden" name="system_name" value="$system_name">
<input type="hidden" name="system_id" value="$system_id">
</form> hours<br/></div>
<p></p>
<div id="myDIV3"><strong>Latest Temperature:</strong>
	<div id="myDIV2">$current_temp ˚F</div>
	$date_delta<br/></div>
	<div id="myDIV3"><strong>Setpoint:</strong><form id="frm2" action="" style="display:inline">
	<input type="text" name="newSetPoint" value="$old_setpoint" size=5>
	<input type="hidden" name="accessKey" value="Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6">
	<input type="hidden" name="hours" value="$hours" size=5>
	<input type="hidden" name="cmd" value="changeSetPoint">
	<input type="hidden" name="bucketKey" value="KTYPDWRBG8S8">
	<input type="hidden" name="system_name" value="$system_name">
	<input type="hidden" name="system_id" value="$system_id">
	</form> ˚F
</div>
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
