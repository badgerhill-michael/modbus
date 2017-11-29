<?php

#	date_default_timezone_set( "America/Chicago" );	

	function dbconnect() {
		$link = mysqli_connect("localhost", "badgerhi_mkmb", 'F52+@;~L9n$y', "badgerhi_modbus");
		if($link === false){
				die("ERROR: Could not connect. " . mysqli_connect_error() );
		}
		return( $link );
	}

	function checkAuth( $aKey, $bKey) {
		if( $aKey != "Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6" ) {
			die("Not found");
		}
		else if( $bKey != "KTYPDWRBG8S8" ) {
				die("Not found");
		}
	}

	function setTempSetPoint( $device, $system_id, $setpoint ) {
		$url = "https://api.particle.io/v1/devices/" . $device . "/setTempCSP";
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
	
	function selectSensorReportsByInterval( $link, $system_name, $system_id, $parameter_name, $interval, $units ) {
		$sql = "SELECT date_recorded, parameter_value, parameter_units FROM	sensor_reports WHERE system_name = \"$system_name\" AND system_id = \"$system_id\" AND parameter_name = \"$parameter_name\" AND parameter_value < 900000 AND date_received >= now() - INTERVAL $interval $units ORDER BY date_recorded ASC";

		$result = mysqli_query( $link, $sql );

		if( $result )
			return( $result );
		else{
			echo "ERROR: Could not execute $sql. " . mysqli_error($link);
			return(-1);
		}
	}

	function selectSensorReports( $link, $system_name, $system_id, $parameter_name, $hours ) {
		if( $hours < 1 )
			$hours = 24;
		$sql = "SELECT date_recorded, parameter_value, parameter_units FROM	sensor_reports WHERE system_name = \"$system_name\" AND system_id = \"$system_id\" AND parameter_name = \"$parameter_name\" AND parameter_value < 900000 AND date_received >= now() - INTERVAL $hours HOUR ORDER BY date_recorded ASC";

		$result = mysqli_query( $link, $sql );

		if( $result )
			return( $result );
		else{
			echo "ERROR: Could not execute $sql. " . mysqli_error($link);
			return(-1);
		}
	}

	function selectTemperatureData( $link ) {
		$sql = "SELECT date_recorded, system_id, parameter_name, parameter_value, parameter_units FROM	sensor_reports WHERE system_name = \"tempc\" ORDER BY date_recorded DESC LIMIT 33";

		$result = mysqli_query( $link, $sql );

		if( $result )
			return( $result );
		else{
			echo "ERROR: Could not execute $sql. " . mysqli_error($link);
			return(-1);
		}
	}

	function checkRecency( $link ) {
		$msg_content = "<h4>Latest Readings</h4>";
		// CO2 Flow Meter
		$sql = "SELECT system_name, system_id, parameter_name, parameter_value, parameter_units, date_received
			FROM `sensor_reports`
			WHERE system_name = 'co2_flow' AND system_id = 255 AND parameter_name = 'dv'
			ORDER BY date_received DESC LIMIT 1";
		$msg_content .= "<p><blockquote>";
		$msg_content .= formatRecency( $link, $sql );
		$msg_content .= "</blockquote></p>";


		// Smart Gateway
		$sql = "SELECT system_name, system_id, parameter_name, parameter_value, parameter_units, date_received
			FROM `sensor_reports`
			WHERE system_name = 'SmartGateway' AND system_id = 2
			ORDER BY date_received DESC LIMIT 1";
		$msg_content .= "<p><blockquote>";
		$msg_content .= formatRecency( $link, $sql );
		$msg_content .= "</blockquote></p>";

		// Temp Controllers
		$sql = "SELECT DISTINCT system_name, system_id FROM `sensor_reports` WHERE system_name = 'tempc' ORDER BY system_id";
		$result = mysqli_query( $link, $sql );

		if( ! $result ) {
			echo "ERROR: Could not execute $sql. " . mysqli_error($link);
			return(-1);
		}
		$msg_content .= "<p><blockquote>";
		while ($row = mysqli_fetch_assoc( $result ) ) {
			$sql = "SELECT system_name, system_id, parameter_name, parameter_value, parameter_units, date_received
				FROM `sensor_reports`
				WHERE system_name = '" . $row["system_name"] . "' AND system_id = " . $row["system_id"] . " AND parameter_name = 'T' 
				ORDER BY date_received DESC LIMIT 1";
			$msg_content .= formatRecency( $link, $sql );
		}
		$msg_content .= "</blockquote></p>";
		mysqli_free_result( $result );
		return( $msg_content );
	}
	
	function formatRecency( $link, $sql ) {
		$url = "http://www.badgerhillbrewing.com/modbus/view.php?accessKey=Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6&bucketKey=KTYPDWRBG8S8&system_name=%s&system_id=%s&parameter_name=%s";
		
		$msg_content = "";
		$result = mysqli_query( $link, $sql );

		if( ! $result ) {
			echo "ERROR: Could not execute $sql. " . mysqli_error($link);
			return(-1);
		}
		while ($row = mysqli_fetch_assoc( $result ) ) {
			$date_delta = formatDateDiff( new DateTime($row["date_received"] ), new DateTime( "now" ) );
			$target_name = getTargetName( $link, $row["system_name"], $row["system_id"], $row["parameter_name"] );
			$purl = sprintf( $url, $row["system_name"], $row["system_id"], $row["parameter_name"] );
			$msg_content .= sprintf( "<strong><a href=%s>%s</a></strong> &nbsp; %s = %4.1f %s &nbsp; %s<br/>\n", $purl, $target_name, $row["parameter_name"], $row["parameter_value"], $row["parameter_units"], $date_delta );
		}
		mysqli_free_result( $result );
		return( $msg_content );
	}
	
	function getTargetName( $link, $system_name, $system_id, $parameter_name ) {
			$sql = "SELECT target_name FROM `modbus_map` WHERE system_name = '" . $system_name . "' AND system_id = " . $system_id . " AND parameter_name = '" . $parameter_name . "'";
			$rows = mysqli_query( $link, $sql );
			if( ! $rows ) {
				echo "ERROR: Could not execute $sql. " . mysqli_error($link);
				return(-1);
			}
			$return_value = mysqli_fetch_row( $rows )[0];
			mysqli_free_result( $rows );
			return( $return_value );
	}
	
	function getLatestReading( $link, $system_name, $system_id, $parameter_name ) {
			$sql = "SELECT system_name, system_id, parameter_value, date_received FROM `sensor_reports` WHERE system_name = '" . $system_name . "' AND system_id = " . $system_id . " AND parameter_name = '" . $parameter_name . "' ORDER BY date_received DESC LIMIT 1";
			$rows = mysqli_query( $link, $sql );
			if( ! $rows ) {
				echo "ERROR: Could not execute $sql. " . mysqli_error($link);
				return(-1);
			}
			$one_row = mysqli_fetch_assoc( $rows );
			mysqli_free_result( $rows );
			return( $one_row );
	}
	
	function checkGlycolChiller( $link ) {
		$data			= array();	
		$problems = array();	

		$rows			= selectTemperatureData( $link );
		while ($row = mysqli_fetch_assoc( $rows	 ) )
			$data[$row["system_id"]][$row["date_recorded"]][$row["parameter_name"]]=$row["parameter_value"];
		mysqli_free_result( $rows );

		foreach( array_keys( $data ) as $system_id ){
			foreach( array_keys( $data[$system_id] ) as $date ){
				if( floatval($data[$system_id][$date]["T"]) > (floatval($data[$system_id][$date]["SP"]) + 0.3 ) ) {
					if( ! array_key_exists( $system_id, $problems ) ) {
						$problems[$system_id]=$data[$system_id][$date];
					}
				}
			}
		}
		return( $problems );
	}

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