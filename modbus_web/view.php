<?php
	include( "lib.php" );
	function getChartHTML( $system_name, $parameter_name, $rows ) {
		$html = <<<EOT
<canvas id="$system_name$parameter_name" width="900" height="600"></canvas>
<script>
var ctx = document.getElementById('$system_name$parameter_name').getContext('2d');
var scatterChart = new Chart(ctx, {
		type: 'scatter',
		data: {
				datasets: [{
						backgroundColor: 'rgba( 255, 140, 0, 0.5 )',
						borderColor: 'rgba( 255, 69, 0, 1.0 )',
						label: '$system_name : $parameter_name',
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
		responsive: false,
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
	$hours = 72;

	if( isset($_REQUEST["hours"]) )
		$hours = $_REQUEST["hours"];

	$system_name		= $_REQUEST["system_name"];
	$system_id			= $_REQUEST["system_id"];
	$parameter_name = $_REQUEST["parameter_name"];
	
	$link = dbconnect();
	
	$fv = getTargetName( $link, $system_name, $system_id, $parameter_name );
	$rows = selectSensorReports( $link, $system_name, $system_id, $parameter_name, $hours );
	if( mysqli_num_rows($rows) > 0 ) {
		mysqli_data_seek( $rows, mysqli_num_rows($rows)-1 );
		$row = mysqli_fetch_row($rows);
		$current_reading = sprintf( "%4.1f", $row[1] );
		$current_reading_units = $row[2];
		$interval = date_diff( date_create_from_format('U', $row[0] ), date_create("now") );
		$date_delta = formatDateDiff( date_create_from_format('U', $row[0] ), date_create("now") );
		$rows->data_seek( 1 );
	}

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
<title>$fv $parameter_name</title>
<body>
<div id="myDIV1"><strong>$fv ($parameter_name)</strong><br/></div>
<div id="myDIV3">Displaying the last <form id="frm1" action="" style="display:inline">
<input type="text" name="hours" value="$hours" size=5>
<input type="hidden" name="accessKey" value="Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6">
<input type="hidden" name="bucketKey" value="KTYPDWRBG8S8">
<input type="hidden" name="system_name" value="$system_name">
<input type="hidden" name="system_id" value="$system_id">
<input type="hidden" name="parameter_name" value="$parameter_name">
</form> hours<br/></div>
<p></p>
<div id="myDIV3"><strong>Latest:</strong></div>
	<div id="myDIV2">$current_reading $current_reading_units</div>
	($date_delta)<br/>

EOT;

	$html .= getChartHTML( $system_name, $parameter_name, $rows );

	$html .= <<<EOT
</body>
</html>
EOT;

	print($html);
	mysqli_free_result($rows);
	mysqli_close($link);
?>
