
  var logging_discarded;
  var logging_logs;
  var device;

  LOGGING_SERVICE_16UUID 	= 0x2301;
  LOGGING_SERVICE_128UUID	= "00000000-1111-2222-2222-333333333333"

  LOGGING_LOGS     			= "11111111-0000-0000-0000-111111111111";
  LOGGING_DISCARDED     	= "11111111-0000-0000-0000-222222222222";
  LOGGING_SEVERITY			= "11111111-0000-0000-0000-333333333333";
  
  function log(text, placeholder)
  {
    // Dump log data to text field
    var textarea = document.getElementById(placeholder);
    textarea.value += "\n" + text;
    textarea.scrollTop = textarea.scrollHeight;
  }
  
  function ab2str(buf) {
	return String.fromCharCode.apply(null, new Uint8Array(buf));
}
  
  async function incomingData(event){
    // Data was received from BLE peer
	log(ab2str(event.target.value.buffer), 'log');
	// Write to console for debugging
	// console.log(event.target.value);
  }

  async function onDisconnected(){
    // Peer has disconnected or connection was lost
    log("Peer has disconnected.", 'debug_txt');
	document.getElementById("disconnect_card").style.display = "none";
	document.getElementById("connect_card").style.display = "inline";
  }
 
  async function disconnect_from_peer() {
	  log("Central issued a disconnection.", 'debug_txt');
	  await device.gatt.disconnect();
  }
  
  async function clear_logs() {
	  var textarea = document.getElementById('debug_txt');
	  textarea.value = "";
	  textarea.scrollTop = textarea.scrollHeight;
	  textarea = document.getElementById('log');
	  textarea.value = "";
      textarea.scrollTop = textarea.scrollHeight;
  }
 
  async function set_severity() {
	  var x = parseInt(document.getElementById("severity_select").value);
	  try{
		  await logging_severity.writeValue(Uint8Array.of(x));
		  log("Logging severity is set to " + translate_severity(x) + ".", 'debug_txt');
	  } catch(error) {
      log('Failed: ' + error, 'debug_txt');
    }
  }
 
  function translate_severity(severity) {
	  
	  var text;
	  
	  switch(severity){
		case 0x00:
			text = "LOG_DEBUG";
			break;
		case 0x01:
			text = "LOG_NOTICE"
			break;
		case 0x02:
			text = "LOG_WARNING"
			break;
		case 0x03:
			text = "LOG_ERROR"
			break;
		case 0x04:
			text = "LOG_CRITICAL"
			break;
		default:
			text = "INVALID INPUT"
	  }
	  
	  return text;
  }
 
  async function connect_to_peer() {

    var name = "";

    // Update GUI to match current activity
    document.getElementById("connect_card").style.display = "none";
    var textarea = document.getElementById('debug');
    textarea.value = "";

    // Set BLE scan filters
    let options = {
        filters: [
          {services: [LOGGING_SERVICE_16UUID]}
        ],
        optionalServices: [LOGGING_SERVICE_128UUID]
      }
    
    // Try to connect to a BLE device
    try {
      log('Requesting Bluetooth Device...', 'debug_txt');

      device  = await navigator.bluetooth.requestDevice(options);
      device.addEventListener('gattserverdisconnected', onDisconnected);
     
      name = device.name;
      log("Connected to ["+name+"]", 'debug_txt');

      log('Connecting to GATT Server...', 'debug_txt');
      server = await device.gatt.connect();
      
      log('Mapping Logging Service...', 'debug_txt');
	  const logging_service = await server.getPrimaryService(LOGGING_SERVICE_128UUID);

	  // Read the discarded messages characteristic
	  log('Getting LOGGING_DISCARDED Characteristic...', 'debug_txt');
      logging_discarded = await logging_service.getCharacteristic(LOGGING_DISCARDED);

	  log(' Read the messages discarded before a connection is established...', 'debug_txt');
	  discarded_value = await logging_discarded.readValue();
	  log(discarded_value.getUint32(0, true) + ' messages have been discarded before connection....', 'debug_txt');

	  // Read the severity characteristic
	  log('Getting LOGGING_SEVERITY Characteristic...', 'debug_txt');
	  logging_severity = await logging_service.getCharacteristic(LOGGING_SEVERITY);
	  
	  log(' Read the current logging severity...', 'debug_txt');
	  severity_value = await logging_severity.readValue();
	  log(' The current messages logged have minimum ' + translate_severity(severity_value.getUint8(0)) + ' severity.', 'debug_txt');

	  // Getting the logging characteristic
      log('Getting LOGGING_LOGS Characteristic...', 'debug_txt');
      logging_logs = await logging_service.getCharacteristic(LOGGING_LOGS);
    
	  // Subscribe to the logging characteristic
      log(' Subscribing to LOGGING_LOGS notifications...', 'debug_txt');
      await logging_logs.startNotifications();
      logging_logs.addEventListener('characteristicvaluechanged', incomingData);
      
      log('Ready to communicate.', 'debug_txt');

      // Update GUI to show that we are ready to receive a file
      document.getElementById("disconnect_card").style.display = "inline";
	  document.getElementById("severity_select").getElementsByTagName('option')[severity_value.getUint8(0)].selected = 'selected';
      
    } catch(error) {
      log('Failed: ' + error, 'debug_txt');
    }
  }

