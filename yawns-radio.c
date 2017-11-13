
#include "radio.h"

#include <stdio.h>

#include "owns/owns.h"

static struct ons_s onsx;
static struct ons_config_s config = ONS_CONFIG_DEFAULT;
static struct ons_radio_s radiox;

char server_address[256];
char local_address[256];
char band[256];

uint32_t channel = 0;

extern int contiki_argc;
extern char **contiki_argv;

/*---------------------------------------------------------------------------*/
static int yawns_init(void)
{
    int res;

    if (contiki_argc != 4) {
        printf("Error invalid arg count\n");
        return -1;
    }

    strncpy(server_address, contiki_argv[1], sizeof(server_address) - 1);
    strncpy(local_address, contiki_argv[2], sizeof(local_address) - 1);
    strncpy(band, contiki_argv[3], sizeof(band) - 1);

    printf("YAWNS connecting to server: %s with address: %s and band: %s\n",
            server_address, local_address, band);

    res = ONS_init(&onsx, server_address, local_address, &config);
    if (res < 0) {
        printf("Error %d creating ONS connector\n", res);
        return -2;
    }

    res = ONS_radio_init(&onsx, &radiox, band);
    if (res < 0) {
        printf("Error %d creating ONS virtual radio\n", res);
        return -3;
    }

    return 0;
}
/*---------------------------------------------------------------------------*/
static int yawns_prepare(const void *payload, unsigned short payload_len)
{
    return 1;
}
/*---------------------------------------------------------------------------*/
static int yawns_transmit(unsigned short transmit_len)
{
    return RADIO_TX_OK;
}
/*---------------------------------------------------------------------------*/
static int yawns_send(const void *payload, unsigned short payload_len)
{
    return ONS_radio_send(&radiox, channel, (uint8_t *) payload, payload_len);
}
/*---------------------------------------------------------------------------*/
static int yawns_radio_read(void *buf, unsigned short buf_len)
{
    int res;
    uint16_t count;
    res = ONS_radio_get_received(&radiox, buf_len, (uint8_t *) buf, &count);
    if (res < 0) {
        return res;
    }
    return count;
}
/*---------------------------------------------------------------------------*/
static int yawns_channel_clear(void)
{
    return 1;
}
/*---------------------------------------------------------------------------*/
static int yawns_receiving_packet(void)
{
    int res;
    uint32_t state;
    res = ONS_radio_get_state(&radiox, &state);
    if (state == ONS_RADIO_STATE_RECEIVING) {
        return 1;
    }
    return res;
}
/*---------------------------------------------------------------------------*/
static int yawns_pending_packet(void)
{
    return ONS_radio_check_receive(&radiox);
}
/*---------------------------------------------------------------------------*/
static int yawns_on(void)
{
    return ONS_radio_start_receive(&radiox, channel);
}
/*---------------------------------------------------------------------------*/
static int yawns_off(void)
{
    return ONS_radio_stop_receive(&radiox);
}
/*---------------------------------------------------------------------------*/
static radio_result_t yawns_get_value(radio_param_t param, radio_value_t *value)
{
    return RADIO_RESULT_NOT_SUPPORTED;
}
/*---------------------------------------------------------------------------*/
static radio_result_t yawns_set_value(radio_param_t param, radio_value_t value)
{
    return RADIO_RESULT_NOT_SUPPORTED;
}
/*---------------------------------------------------------------------------*/
static radio_result_t yawns_get_object(radio_param_t param, void *dest, size_t size)
{
    return RADIO_RESULT_NOT_SUPPORTED;
}
/*---------------------------------------------------------------------------*/
static radio_result_t yawns_set_object(radio_param_t param, const void *src, size_t size)
{
    return RADIO_RESULT_NOT_SUPPORTED;
}
/*---------------------------------------------------------------------------*/
const struct radio_driver yawns_driver = {
    yawns_init,
    NULL, //yawns_prepare,
    NULL, //yawns_transmit,
    yawns_send,
    yawns_radio_read,
    yawns_channel_clear,
    yawns_receiving_packet,
    yawns_pending_packet,
    yawns_on,
    yawns_off,
    yawns_get_value,
    yawns_set_value,
    yawns_get_object,
    yawns_set_object
};
/*---------------------------------------------------------------------------*/

