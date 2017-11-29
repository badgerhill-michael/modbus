<?php

	include( "lib.php" );
	
	function getChartHTML( $title, $rows, $color ) {
		$html = <<<EOT

<canvas id="$title" width="1080" height="300"></canvas>
<script>
var ctx = document.getElementById('$title').getContext('2d');
var scatterChart = new Chart(ctx, {
		type: 'scatter',
		data: {
				datasets: [{
						label: '$title',
						backgroundColor: '$color',
						borderColor: 'rgba(0,0,0,1.0)',
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

	$link = dbconnect();
	$rows_hltp	 = selectSensorReports( $link, "SmartGateway", 2, "hltp", $hours );
	$rows_cltp	 = selectSensorReports( $link, "SmartGateway", 2, "cltp", $hours );

	$html = <<<EOT
<html>
<script src="https://cdn.jsdelivr.net/npm/moment@2.19.1/moment.min.js"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.4.0/Chart.min.js"></script>
<link rel="stylesheet" type="text/css" media="all" href="http://www.badgerhillbrewing.com/wp-content/themes/badger_hill/style.css" />
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
}
</style>
<body>
<div id="myDIV1"><strong>Liquor Tanks</strong><br/></div>
<div id="myDIV3">Displaying the last <form id="frm1" action="" style="display:inline">
<input type="text" name="hours" value="$hours" size=5>
<input type="hidden" name="accessKey" value="Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6">
<input type="hidden" name="bucketKey" value="KTYPDWRBG8S8">
</form> hours<br/></div>

EOT;

	$html .= getChartHTML( "HLT Level", $rows_hltp, "rgba(200,40,40, 0.5)" );
	$html .= getChartHTML( "CLT Level", $rows_cltp, "rgba(40,40,200, 0.5)" );

	$html .= <<<EOT
</body>
</html>
EOT;

	print($html);
	mysqli_free_result($rows_hltp);
	mysqli_free_result($rows_cltp);
	mysqli_close($link);
?>
