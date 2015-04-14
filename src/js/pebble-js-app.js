var fetched;

function iconFromWeatherId(weatherId) {
  if (weatherId > 199 && weatherId < 233) {
    return 0; //Thunderstorm
  } 
	else if (weatherId > 299 && weatherId < 322) {
    return 1; //Drizzle
  } 
	else if (weatherId > 499 && weatherId < 523) {
    return 2; //Rain
  } 
	else if (weatherId > 599 && weatherId < 622) {
    return 3; //Snow
  } 
	else if (weatherId > 700 && weatherId < 742) {
	return 4; //Haze or fog
  }
	else if (weatherId == 800){
	return 5; //Clear
  }
	else if (weatherId == 801){
	return 6; //Few clouds
  }
	else if (weatherId > 801){
	return 7; //Scattered, broken, or overcast clouds
  }
}

function fetchWeather(toattach) {
  var response;
  var req = new XMLHttpRequest();
  var address = "http://api.openweathermap.org/data/2.5/weather?" + toattach;
  req.open('GET', address, true);
  console.log("Getting new weather data from: " + address);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
      	console.log("Response success. Result: \n" + req.responseText);
        response = JSON.parse(req.responseText);
		var temperature, condition;
			var weatherResultList = response.weather[0];
			temperature = response.main.temp;
			condition = iconFromWeatherId(weatherResultList.id);

          Pebble.sendAppMessage({
            "condition":condition,
            "temperature":temperature,
		  });
      } else {
			console.log("Error: could not connect! (is api.openweathermap.com down?)");
      }
    }
  };
  req.send(null);
}

function formatLatAndLong(lat, lon){
	var toReturn = "lat=" + lat + "&lon=" + lon;
	return toReturn;
}

function formatCustomLoc(customLoc){
	var toReturn = "q=" + customLoc;
	return toReturn;
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
	if(fetched === 0){
		fetchWeather(formatLatAndLong(coordinates.latitude, coordinates.longitude));
		fetched = 1;
	}
	else if(fetched == 1){
		return;
	}
}

function locationError(err) {
  console.warn('Location error (' + err.code + '): ' + err.message);
	var temperatureError = parseInt(err.code) + 400;
	var workingLatitude = localStorage.getItem("latitude1");
	var workingLongitude = localStorage.getItem("longitude1");

	fetchWeather(formatLatAndLong(workingLatitude, workingLongitude));
	console.log("Fetching previous working temperature from latitude: " + workingLatitude + " and longitude: " + workingLongitude);
	console.log("Because of error: " + temperatureError);
}

function getWeather(){
	var customLoc = localStorage.getItem("customLoc");
	if(customLoc !== "noLOC" && customLoc){
		fetchWeather(formatCustomLoc(customLoc));
	}
	else{
  		locationWatcher = window.navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);
  	}
}

var locationOptions = { "timeout": 15000, "maximumAge": 60000 }; 

Pebble.addEventListener("ready", 
						function(e) {
							getWeather();
                        });

Pebble.addEventListener("showConfiguration", function(e){
	var url = "https://edwinfinch.github.com/config-timedock";
	console.log("Opening: " + url);
	Pebble.openURL(url);
});

Pebble.addEventListener("webviewclosed", function(e){
	if(e.response){
		console.log(e.response);
		var values = JSON.parse(decodeURIComponent(e.response));

		if(values.customLoc){
			localStorage.setItem("customLoc", values.customLoc.replace(" ", ""));
			console.log("Set customLoc to: " + values.customLoc.replace(" ", ""));
		}
		if(values.customLoc === ""){
			localStorage.setItem("customLoc", "noLOC");
			console.log("Reset customLoc");
		}

		Pebble.sendAppMessage(values);

		getWeather();
	}
});

Pebble.addEventListener("appmessage", function(e){
	console.log("Got appmessage. Requesting new weather data.");
	getWeather();
});