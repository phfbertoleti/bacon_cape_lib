/*
* Includes
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <MQTTClient.h>
#include <unistd.h>
#include "../bacon_cape_lib/led_ctrl.h"
#include "../bacon_cape_lib/slider_ctrl.h"
#include "../bacon_cape_lib/button_ctrl.h"

/*
* Defines
*/
#define MQTT_ADDRESS   "tcp://iot.eclipse.org"
/* Replace this ID by one of yours */
#define CLIENTID       "MQTTCClientID"

/* Replace these MQTT topics by those of your choice */
#define MQTT_PUBLISH_TOPIC_BUTTON       "MQTTBaconCapePubButton"
#define MQTT_PUBLISH_TOPIC_SLIDER       "MQTTBaconCapePubSlider"
#define MQTT_SUBSCRIBE_TOPIC_LED_GREEN  "MQTTBaconCapeSubGreenTopic"
#define MQTT_SUBSCRIBE_TOPIC_LED_BLUE   "MQTTBaconCapeSubBlueTopic"
#define MQTT_SUBSCRIBE_TOPIC_LED_RED    "MQTTBaconCapeSubRedTopic"
#define MQTT_SUBSCRIBE_TOPIC_DISPLAY    "MQTTBaconCapeSubDisplayTopic"

/*
*  Variables
*/
MQTTClient client;
TLEDCtrl LED_green;
TLEDCtrl LED_blue;
TLEDCtrl LED_red;
TSLIDERCtrl slider;
TBUTTONCtrl push_button;

/*
* Prototypes
*/
void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);

/*
* Implementations
*/
void publish(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 2;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);

}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;

    printf("Incoming message! \n\rTopic: %s Message: %s\n", topicName, payload);

    if (strcmp(MQTT_SUBSCRIBE_TOPIC_LED_GREEN, topicName) == 0)
    {
        if (payload[0] == '1')
            turn_on_led(&LED_green);

        if (payload[0] == '0')
            turn_off_led(&LED_green);
    }

    if (strcmp(MQTT_SUBSCRIBE_TOPIC_LED_BLUE, topicName) == 0)
    {
        if (payload[0] == '1')
            turn_on_led(&LED_blue);

        if (payload[0] == '0')
            turn_off_led(&LED_blue);
    }

    if (strcmp(MQTT_SUBSCRIBE_TOPIC_LED_RED, topicName) == 0)
    {
        if (payload[0] == '1')
            turn_on_led(&LED_red);

        if (payload[0] == '0')
            turn_off_led(&LED_red);
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

/* Main program */
int main(int argc, char *argv[])
{
   int rc;
   char push_button_state[2] = {0};
   char percentage_slider[5] = {0};
   MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

   LED_green.led_config = 0;
   LED_green.gpio_num = 50;

   LED_blue.led_config = 0;
   LED_blue.gpio_num = 51;

   LED_red.led_config = 0;
   LED_red.gpio_num = 7;

   slider.slider_config = 0;
   slider.slider_analog_input = 5;

   push_button.button_config = 0;
   push_button.gpio_num = 22;

   setup_led(&LED_green);
   setup_led(&LED_blue);
   setup_led(&LED_red);
   setup_slider(&slider);
   setup_button(&push_button);

   /* MQTT init & topics subscribe */
   MQTTClient_create(&client, MQTT_ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
   MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

   rc = MQTTClient_connect(client, &conn_opts);

   if (rc != MQTTCLIENT_SUCCESS)
   {
       printf("\n\rMQTT broker connection has failed. Error: %d\n", rc);
       exit(-1);
   }

   MQTTClient_subscribe(client, MQTT_SUBSCRIBE_TOPIC_LED_GREEN, 0);
   MQTTClient_subscribe(client, MQTT_SUBSCRIBE_TOPIC_LED_BLUE, 0);
   MQTTClient_subscribe(client, MQTT_SUBSCRIBE_TOPIC_LED_RED, 0);
   MQTTClient_subscribe(client, MQTT_SUBSCRIBE_TOPIC_DISPLAY, 0);

   while(1)
   {
        read_slider_percentage(&slider);
        read_button(&push_button);

        sprintf(push_button_state,"%d",push_button.button_state);
        sprintf(percentage_slider,"%d",slider.slider_percentage_value);

        publish(client, MQTT_PUBLISH_TOPIC_BUTTON, push_button_state);
        publish(client, MQTT_PUBLISH_TOPIC_SLIDER, percentage_slider);

        sleep(1);
   }
}