<?php

	function dbconnect() {
		$link = mysqli_connect("localhost", "badgerhi_mkmb", 'F52+@;~L9n$y', "badgerhi_modbus");
		if($link === false){
				die("ERROR: Could not connect. " . mysqli_connect_error() );
		}
		return( $link );
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
		$sql = "SELECT date_recorded, system_id, parameter_name, parameter_value, parameter_units FROM	sensor_reports WHERE system_name = \"tempc\" ORDER BY date_recorded DESC LIMIT 100";

		$result = mysqli_query( $link, $sql );

		if( $result )
			return( $result );
		else{
			echo "ERROR: Could not execute $sql. " . mysqli_error($link);
			return(-1);
		}
	}
