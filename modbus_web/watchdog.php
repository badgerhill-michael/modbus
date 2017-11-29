<?php
	include( "lib.php" );

	function html_header() {
		$html = <<<EOT
		<html>
		<head>
<!--		<link rel="stylesheet" type="text/css" media="all" href="http://www.badgerhillbrewing.com/wp-content/themes/badger_hill/style.css" />-->
		<link href='https://fonts.googleapis.com/css?family=Lato' rel='stylesheet'>
		<style>
		body {
				font-family: 'Lato';
				font-size: 16px;
		}
		</style>
		<title>BH Sensor Summary</title>
		</head>
		<body>
EOT;
		return($html);
	}
	
	$time_interval = 15;
	
	if( isset($_REQUEST["mail"]) )
		$mail = $_REQUEST["mail"];

	$now = new DateTime( "now" );
	$now->setTimeZone(new DateTimeZone('America/Chicago'));
	$link = dbconnect();
	
	$msg_content = sprintf( "<h3>Now is %s<br/></h3>\n", $now->format( 'Y-m-d H:i:s' ) );

	$problems = checkGlycolChiller( $link );
	if( count($problems) > 0 )
		$msg_content .= "<h4>Trouble Reports</h4>\n";

	$problem_score = 0.0;
	foreach( array_keys( $problems ) as $system_id ) {
		$ts = array();
		$t  = selectSensorReportsByInterval( $link, 'tempc', $system_id, "T", $time_interval, "MINUTE" );
		$sp = selectSensorReportsByInterval( $link, 'tempc', $system_id, "SP", $time_interval, "MINUTE" );
		while ($r = mysqli_fetch_assoc( $t ) ) {
			$s = mysqli_fetch_assoc( $sp );
			$ts[]= array( "date_recorded" => $r["date_recorded"], "T" => $r["parameter_value"], "SP" => $s["parameter_value"] );
		}
		mysqli_free_result( $t );
		mysqli_free_result( $sp );
		
		$diff1 = floatVal($ts[0]["T"]) - floatVal($ts[0]["SP"]);
		$diff2 = floatVal($ts[count($ts)-1]["T"]) - floatVal($ts[count($ts)-1]["SP"]);
		$problem_score += $diff2**2;
		$rise = (floatVal($ts[count($ts)-1]["T"]) - floatVal($ts[count($ts)-1]["SP"])) - (floatVal($ts[0]["T"]) - floatVal($ts[0]["SP"]));
		$run = ($ts[count($ts)-1]["date_recorded"]-$ts[0]["date_recorded"])/3600;
		$slope = $rise/$run;
		
		$status = "";
		$color = "";
		if( $slope > 0 ) {
			$status = "NOT OK";
			$color = "#bb0000";
		}
		else {
			$status = "PROBABLY OK";
			$color = "#3DB90F";
		}

		$target_name = getTargetName( $link, 'tempc', $system_id, "T" );
		$msg_content .= sprintf( "<p><blockquote><font color='%s'>%s is %s</font><br/>", $color, $target_name, $status );
		$msg_content .= sprintf( "T=%4.1fF > Setpoint=%4.1fF<br/>Cooling is at %s%%<br/>dT=%0.2fF/hour</blockquote></p>\n", $problems[$system_id]["T"], $problems[$system_id]["SP"], $problems[$system_id]["OUT1"], $slope );
	}

	$msg_content .= checkRecency( $link );

	$msg = html_header();
	$msg .=	sprintf( "<p><blockquote><strong>Score=%0.1f problems=%d</strong><br/></blockquote></p>\n",
		sqrt($problem_score), count($problems) ) ;
	$msg .=	$msg_content;
		
		$msg .= <<<EOT
			</body>
			</html>
EOT;

		print($msg);

		if( (count($problems) > 0) || $mail ) {
			$headers[] = 'MIME-Version: 1.0';
			$headers[] = 'Content-type: text/html; charset=iso-8859-1';
			$headers[] = 'To: michael@badgerhillbrewing.com';
			$headers[] = 'From: webmaster@badgerhillbrewing.com';
			$headers[] = 'Reply-To: michael@badgerhillbrewing.com';
			$headers[] = 'X-Mailer: PHP/' . phpversion();
		
			mail( 'michael@badgerhillbrewing.com', 'Modbus Alert', $msg, implode("\n", $headers) );
		}
		mysqli_close($link);
?>
