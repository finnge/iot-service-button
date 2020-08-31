---
layout: default
---

## Automation of ordering processes in the industry 4.0

![Header](https://github.com/finnge/iot-service-button/blob/master/paper/figures/microcontroller.jpeg?raw=true)

The digitalization process is as prominent in modern industry4.0 as in our day-to-day life. Our approach to such transfor-mation is a service button that can help employees in variousdifferent contexts to reorder products or building blocks andis also setup with a authentication abilities. This product livesin the context of the internet of things and can be connectedwith different suppliers and other working processes.  Thisterm paper will focus on the conception and development ofa prototype for a service button. It will highlight the contextof use by examine the problem space and taking a closer lookat the interaction with the system especially how an extra au-thentication step can be included in the process. We will alsodiscuss the choices of used sensors, actuators and authenti-cation procedures and the paradigms of system architecture.We will conclude by discussing various ethics and securityparameters and by taking a look at possible transformations toa ready-to-use product.

## Prototype in Action

<iframe style="width: 100%; margin-bottom: 20px" height="315" src="https://www.youtube-nocookie.com/embed/YwqazpQT0mQ" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

## Assembly instructions

To rebuild the prototype, the following components are required:

- 1 x ESP-32
- 1 x RFID-Reader (MFRC-522)
- 1 x Button (Grove-Button) 
- 1 x Buzzer (Grove-Buzzer)
- 1 x LCD-Display (Grove - LCD RGB Backlight)
- 4 x Grove - 4 pin Cable
- 11 x Jumper Cable

First the buzzer and the button are connected using two *4-pin cables*. These are connected to the grove-connectors for pin 26 for the buzzer and 33 for the button. Next the display is connected be connected. This cannot be done easily since the display needs a 5V power connection. Therefore, two *4-pin cables* must be connected to four *jumper cables*. One end of the cable can then be connected to the display and the other end of the cable to the grove-connector for pin 22. Note that one of the 4 jumper cables must not be connected to a *4-pin cable* again, but to the 5V connector of ESP-32.
Next the RFID reader is connected using seven\textit{jumper cables} which are plugged in the direct pins of the ESP-32. 
The assignment of these cables and the cables for the other components can be seen in the full [term paper](https://github.com/finnge/iot-service-button/blob/master/paper/paper.pdf).
Additionally, the exact arrangement of the components can be seen in the [demo video](#prototype-in-action).

After completing the connection of all parts of the micro-controller, the software can be flashed to the ESP-32. Before the system can function completely the backend using *Amazon Web Services (AWS)* must be setup: First, an object for ESP-32 must be created in the configuration interface of the *AWS IOT Core*. Thereby two certificates are created. These must be inserted into the `secrets.h` (Note the `secrets.default.h`). Another important step is the definition of the AWS endpoint. This can be done from the online interface of AWS and must also be inserted into the file. Furthermore, a unique name of the service button must be defined. The local Wi-Fi authentication-data must also be inserted into the file. This is necessary so that the ESP can connect to the network and has access to AWS. Once these steps have been carried out, the `secret.h` is fully configured. Now all files can be flashed to the ESP. In the test environment of AWS you can now subscribe to the individual MQTT-topics and send messages to the individual topics. Furthermore AWS offers many possibilities to extend the functionality of the Service Button. Dashboards can be created and lambda functions can be triggered by MQTT messages as descriped in the [term paper](https://github.com/finnge/iot-service-button/blob/master/paper/paper.pdf).
