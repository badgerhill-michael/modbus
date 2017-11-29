<?php

	function checkAuth( $aKey, $bKey) {
		if( $aKey != "Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6" ) {
			die("Not found");
		}
		else if( $bKey != "KTYPDWRBG8S8" ) {
				die("Not found");
		}
	}

	function report_parameter( $link, $date_recorded, $system_name, $system_id, $p, $units ) {
		$parameter_name = $p;
		$parameter_value = $_GET[$parameter_name];
		if( $parameter_value < 900000 )
			insertSensorReport( $link, $date_recorded, $system_name, $system_id, $parameter_name, $parameter_value, $units, "modbus_post" );
	}

	function insertSensorReport( $link, $date_recorded, $system_name, $system_id, $parameter_name, $parameter_value, $parameter_units, $reported_by ) {
			$sql = "INSERT INTO sensor_reports
				( date_recorded, system_name, system_id, parameter_name, parameter_value, parameter_units, reported_by)
				VALUES ( $date_recorded, \"$system_name\", \"$system_id\", \"$parameter_name\", \"$parameter_value\", \"$parameter_units\", \"$reported_by\" )";
			if(mysqli_query($link, $sql)){
				echo "Records inserted successfully.";
			} else{
				echo "ERROR: Could not execute $sql. " . mysqli_error($link);
			}
	}

	checkAuth( $_REQUEST["accessKey"], $_REQUEST["bucketKey"] );
	
	$link = mysqli_connect("localhost", "badgerhi_mkmb", 'F52+@;~L9n$y', "badgerhi_modbus");
	if($link === false){
			die("ERROR: Could not connect. " . mysqli_connect_error());
	}

	$date_recorded	= $_REQUEST["date_recorded"];
	$system_name		= $_REQUEST["system_name"];
	$system_id			= $_REQUEST["system_id"];

	if( $system_name == "tempc" ) {
		report_parameter( $link, $date_recorded, $system_name, $system_id, "T", "degreesF" );
		report_parameter( $link, $date_recorded, $system_name, $system_id, "SP", "degreesF" );
		report_parameter( $link, $date_recorded, $system_name, $system_id, "OUT1", "Percent" );
//		report_parameter( $link, $date_recorded, $system_name, $system_id, "Alarm1", "Type" );
	}
	elseif( $system_name == "co2_flow") {
//		$system_id = 255;
		report_parameter( $link, $date_recorded, $system_name, $system_id, "v", "m^3" );
		report_parameter( $link, $date_recorded, $system_name, $system_id, "dv", "LPM" );
	}
	elseif( $system_name == "SmartGateway" ) {
//		$system_id = 2;
		report_parameter( $link, $date_recorded, $system_name, $system_id, "cltp", "InchesH2O" );
		report_parameter( $link, $date_recorded, $system_name, $system_id, "cltt", "degreesC" );
		report_parameter( $link, $date_recorded, $system_name, $system_id, "hltp", "InchesH2O" );
		report_parameter( $link, $date_recorded, $system_name, $system_id, "hltt", "degreesC" );
		report_parameter( $link, $date_recorded, $system_name, $system_id, "fvp", "InchesH2O" );
		report_parameter( $link, $date_recorded, $system_name, $system_id, "fvt", "degreesC" );
		report_parameter( $link, $date_recorded, $system_name, $system_id, "mflowv", "gal" );
		report_parameter( $link, $date_recorded, $system_name, $system_id, "mflowdv", "GPM" );
	}
	else
		die("Not found");
	mysqli_close($link);
?>

