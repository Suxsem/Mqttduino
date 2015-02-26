_PROMPT = ""
_PROMPT2 = ""

mqttStatus = false
mqttIp = "0.0.0.0"
mqttPort = 0
mqttRetry = 0
mqttId = ""

local srv = net.createServer(net.TCP)
srv:listen(80, function(conn) 
	conn:on("receive",function(conn,payload)
		conn:send("MQTTDUINO - Node ID: " .. node.chipid() ..
					" - Node MAC: " .. wifi.sta.getmac() ..
					" - MQTT status: " .. (mqttStatus == true and mqttId or 'not connected') ..
					" - Online since: " .. tmr.time() .. "s")
	end)
	conn:on("sent",function(conn) conn:close() end)
end)

mqttInit = function (id, ip, port, user, password, alive, retry)
	mqttId = id;
	mqttIp = ip
	mqttPort = port
	mqttRetry = retry * 1000
	
	m = mqtt.Client(mqttId, alive, user, password)
	m:lwt(mqttId .. "/status", "0", 2, 1)

	m:on("offline", function(conn)
		if mqttStatus then
			print("[(d")
		end
		mqttStatus = false
		tmr.alarm(0, mqttRetry, 1, function() 
			if mqttStatus then
				tmr.stop(0)
			else
				mqttConnect()
			end
		end)
	end)
	m:on("message", function(conn, topic, data)
		if data ~= nil then
			print("[(m" .. topic .. "|" .. data .. "|")
		end
	end)
	
	mqttConnect()
end

mqttConnect = function ()
	m:connect(mqttIp, mqttPort, 0, function(conn)
		m:publish(mqttId .. "/status", "1", 2, 1, function(conn)
				print("[(c")
				mqttStatus = true
			end)
	end)
end

mqttSubscribe = function (topic, qos)
	m:subscribe(topic, qos, function(conn) print("[(s") end)
end

mqttPublish = function (topic, message, qos, ret)
	m:publish(topic, message, qos, ret, function(conn) print("[(p") end)
end

connectAP = function (ssid, password)
	wifi.setmode(wifi.STATION)
	wifi.sta.config(ssid, password)
	tmr.alarm(0, 1000, 1, function()
		if wifi.sta.getip() ~= nil then
			tmr.stop(0)
			print("[(w");
		end
	end)	
end

startAlive = function (interval)
	tmr.alarm(1, interval * 1000, 1, function()
		print("[(a")
	end)
end

tmr.alarm(0, 10000, 1, function()
	tmr.stop(0)
	print("[(r")
end)