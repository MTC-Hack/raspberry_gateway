#include <fstream>
#include "sx1272.h"
#define BAND868 //900, 433
#define MAX_NB_CHANNEL 9
#define STARTING_CHANNEL 10
#define ENDING_CHANNEL 18
#define LORAMODE  1 //Mode
#define START_ADDRESS 1
#define GATE_ADDRESS 3

#define GET_ADDR              0x00
#define SEND_DATA             0x01
#define SET_PERIOD            0x02
#define PH_CALIBRATION_ACID   0x03
#define PH_CALIBRATION_ALKALI 0x04
#define DOC_CALIBRATION       0x05

//#define DEBUG
void send_data_to_device(char * data, int length, int addr);

void add_data_to_send_device(char * data, int length, int addr);

void send_delayed_data_to_device(int addr);

int callback(void *NotUsed, int argc, char **argv, char **azColName);

void startConfig(int addr);

bool get_data_from_end_device(char * buf, int *addr);

struct commant_str{
	uint8_t data[20];
	uint8_t length;
};

using namespace std;
