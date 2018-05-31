client = new Paho.MQTT.Client(location.hostname, Number(location.port), "IoTT_Web");

client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;

client.connect({onSuccess:onConnect});

function onConnect()
{
	console.log("onConnect");
	client.subscribe("#");
}

function onConnectionLost(responseObject)
{
	if(responseObject.errorCode !== 0)
	{
		console.log("onConnectionLost: "+responseObject.errorMessage);
	}
}

function onMessageArrived(message)
{
	console.log("onMessageArrived:\n\tFrom " + message.destinationName + "\n\tPayload " + message.payloadString);
	var $Item = findListItem(message.destinationName);
}

function findListItem(destination)
{
	var result = document.getElementById("rootItem");
	var destinations = destination.split("/");
	for(var i = 0; i < destinations.length; i++)
	{
		var subResultId = "rootItem";
		for(var j = 0; j <= i; j++)
			{
				subResultId += '/' + destinations[j];
			}
		
		var subResultItem = document.getElementById(subResultId);
		if(subResultItem)
			{
				result = subResultItem;
			}
		else
			{
				var newItem = $('<li id="' + subResultId + '>' + destinations[i] + '</li>');
				result = result.appendChild(newItem)
			}
	}
	return result;
}