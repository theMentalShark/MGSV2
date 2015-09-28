var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // Construct URL
  var url = "http://api.openweathermap.org/data/2.5/weather?lat=" +
      pos.coords.latitude + "&lon=" + pos.coords.longitude;

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      // Temperature in Kelvin requires adjustment
      var temperature_c = Math.round(json.main.temp - 273.15);
      console.log("Temperature c is " + temperature_c);

      // Temperature in Kelvin requires adjustment to f
      var temperature_f = Math.round(json.main.temp * 9/5 - 459.67);
      console.log("Temperature f is " + temperature_f);

      // Conditions
      var conditions = json.weather[0].main;      
      console.log("Conditions are " + conditions);
      
      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_CONDITIONS": conditions,
        "KEY_TEMPERATURE_C": temperature_c,
        "KEY_TEMPERATURE_F": temperature_f,
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
    }      
  );
}

function locationError(err) {
  console.log("Error requesting location!");
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

    // Get the initial weather
    getWeather();
  }
);


// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getWeather();
  }                     
);


Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://www.sfu.ca/~zya31/mgsv_watchface_config/';
  console.log('Showing configuration page: ' + url);

  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));
  console.log('Configuration page returned: ' + JSON.stringify(configData));

  var temperatureFormat = configData.temperatur_format;
  var powerSavingMode = configData.power_saving;
  
  console.log('temp format:' + temperatureFormat);
  console.log('power saving:' + powerSavingMode);

//   // Assemble dictionary using our keys
//   var dictionary = {
//     "KEY_POWERSAVING": powerSavingMode,
//     "KEY_IMPERIAL": temperatureFormat,
//   };
  
  // Assemble dictionary using our keys
  var dictionary = {
    "KEY_POWERSAVING": powerSavingMode,
    "KEY_IMPERIAL": temperatureFormat,
  };
  // Send to watchapp
  Pebble.sendAppMessage(dictionary, function() {
  
      console.log('Send successful!');
//   if (configData.temperatur_format) {
//     Pebble.sendAppMessage({
//       temperatureFormat: configData.temperatur_format,
//       powerSavingMode: configData.power_saving
    }, function() {
    console.log('Send failed!');
    });
  
});
