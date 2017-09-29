#include "lora.h"

uint8_t loraChannelIndex=0;
uint32_t loraChannelArray[MAX_NB_CHANNEL]={CH_10_868,CH_11_868,CH_12_868,CH_13_868,CH_14_868,CH_15_868,CH_16_868,CH_17_868,CH_18_868};

#define PRINTLN                   printf("\n")
#define PRINT_CSTSTR(fmt,param)   printf(fmt,param)
#define PRINT_STR(fmt,param)      PRINT_CSTSTR(fmt,param)
#define PRINT_VALUE(fmt,param)    PRINT_CSTSTR(fmt,param)
#define FLUSHOUTPUT               fflush(stdout);

#define FILE_NAME_FORMAT		"files/%d.%c"
//Variables
int dest_addr = 6;
char sprintf_buf[100];
bool radioON = false;
uint8_t loraMode = LORAMODE;
uint32_t loraChannel = loraChannelArray[loraChannelIndex];
char loraPower = 'x'; //innitial poser level, M (maximum), H (high), L (low)
uint8_t loraAddr = 255;
unsigned int inter_pkt_time = 10000; //Time between sending
unsigned int random_inter_pkt_time = 0 ;
long last_periodic_sendtime = 0;

void send_data_to_device(char * data, int length, int addr){
	#ifdef DEBUG
	printf("[LORA]: sending data to device \n");
	#endif
	int i=0, e;
	int cmdValue;
	if (radioON) {
		while (!inter_pkt_time){}
		//if (inter_pkt_time) {
			while(!(millis() - last_periodic_sendtime > (random_inter_pkt_time ? random_inter_pkt_time : inter_pkt_time))){};
			//if (millis() - last_periodic_sendtime > (random_inter_pkt_time ? random_inter_pkt_time : inter_pkt_time)) {
				PRINT_CSTSTR("%s", "Start to send data: ");
				PRINT_VALUE("%ld\n", millis());
				printf("%s\n%d\n", data, strlen(data));
				PRINTLN;
				
				sx1272.CarrierSense();
				long startSend = millis();
				e = sx1272.sendPacketTimeout(addr,(uint8_t*)data , length, 2000);
				
				PRINT_CSTSTR("%s", "LoRa Sent in ");
				PRINT_VALUE("%ld", millis() - startSend);
				PRINTLN;
				PRINT_CSTSTR("%s", "Packet sent, state ");
				PRINT_VALUE("%d", e);
				PRINTLN;
				if (random_inter_pkt_time) {
					random_inter_pkt_time = random() % inter_pkt_time + 1000;
					PRINT_CSTSTR("%s", "next in ");
					PRINT_VALUE("%ld", random_inter_pkt_time);
					PRINTLN;
				}
				last_periodic_sendtime = millis();
			//}
		//}

	}
#ifdef DEBUG
	printf("stop send func \n");
#endif
}

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;
    printf("\n\n cal back\n");
    for (int i = 0; i < argc; i++) {
		int ns=-9;
    	printf("argv[%d]: %d\n",i,argv[i]);
    	//send_data("asda"); 
		printf("sending");
    }
    printf("\n");
    return 0;
}

void startConfig(int addr) {
	loraAddr = addr;
	int e;
	e = sx1272.ON();
	PRINT_CSTSTR("[LORA]: %s","^$**********Power ON: state ");
	PRINT_VALUE("%d", e);
	PRINTLN;
	if (!e) {
		PRINT_CSTSTR("[LORA]: %s","^$Default sync word: 0x");
		PRINT_VALUE("%X", sx1272._syncWord);
		PRINTLN;
		radioON = true;
		e = sx1272.getSyncWord();
		// Set transmission mode and print the result
		e = sx1272.setMode(loraMode);
		// Select frequency channel
		if (loraMode==11)
			e = sx1272.setChannel(CH_18_868);
		else 
			e = sx1272.setChannel(loraChannel);
		// Select output power (Max, High or Low)
		e = sx1272.setPower(loraPower);
		// get preamble length
		e = sx1272.getPreambleLength();
		// Set the node address and print the result
		//e = sx1272.setNodeAddress(loraAddr);
		sx1272._nodeAddress=loraAddr;
		e=0;
	}
	FLUSHOUTPUT;
	delay(1000);
	printf("[LORA]: setup finished \n");
}

bool get_data_from_end_device(char * buf, int *addr){
	int i=0, e;
	int cmdValue;
	if (radioON) {
		e=1;
		uint16_t w_timer=1000;
		if (loraMode==1)
			w_timer=5000;
		e = sx1272.receivePacketTimeout(w_timer);
#ifdef DEBUG
		printf("e=%d\n", e);
#endif	
		if (!e) {
			uint8_t tmp_length;
			
			sx1272.getSNR();
			sx1272.getRSSIpacket();
			
			tmp_length=sx1272._payloadlength;
			
			PRINTLN;
			
			sprintf(sprintf_buf,"	[LORA]: RECEIVED dst=%d type=0x%.2X src=%d seq=%d len=%d SNR=%d RSSIpkt=%d BW=%d CR=4/%d SF=%d\n", 
				sx1272.packet_received.dst,
				sx1272.packet_received.type, 
				sx1272.packet_received.src,
				sx1272.packet_received.packnum,
				tmp_length, 
				sx1272._SNR,
				sx1272._RSSIpacket,
				(sx1272._bandwidth==BW_125)?125:((sx1272._bandwidth==BW_250)?250:500),
				sx1272._codingRate+4,
				sx1272._spreadingFactor
			);
			
			PRINT_STR("%s",sprintf_buf);
			
			sprintf(sprintf_buf,"^p%d,%d,%d,%d,%d,%d,%d\n",
				sx1272.packet_received.dst,
				sx1272.packet_received.type,                   
				sx1272.packet_received.src,
				sx1272.packet_received.packnum, 
				tmp_length,
				sx1272._SNR,
				sx1272._RSSIpacket
			);
			
			PRINT_STR("%s",sprintf_buf);          
			sprintf(sprintf_buf,"^r%d,%d,%d\n", 
				(sx1272._bandwidth==BW_125)?125:((sx1272._bandwidth==BW_250)?250:500),
				sx1272._codingRate+4,
				sx1272._spreadingFactor
			);      
			PRINT_STR("%s",sprintf_buf);
			printf("received data=%s\n",sx1272.packet_received.data); 
			memcpy(buf, sx1272.packet_received.data, tmp_length);
			buf[tmp_length] = '\0';
			*addr = sx1272.packet_received.src;
			return true;
		}
		return false;
	}
}

void add_data_to_send_device(char * data, int length, int addr){
	char file_name[50];
	commant_str temp;
	temp.length = length;
	strcpy((char *)temp.data, data);
	sprintf(file_name, FILE_NAME_FORMAT, addr, 'q');
	ofstream f(file_name, ios::binary|ios::app);
	f.write((char*)&temp, sizeof(temp));
	f.close();
}

void send_delayed_data_to_device(int addr){
	char file_name[50];
	char temp_file_name[50];
	bool is_empty = true;
	commant_str temp;
	
	sprintf(file_name, FILE_NAME_FORMAT, addr, 'q');
	ifstream f(file_name, ios::binary|ios::out);
	if (f){		
		if (f.read((char*)&temp, sizeof(temp))){
			is_empty = false;
			printf("[LORA]: file name: %d.q, length: %d, data:", addr, temp.length);
			for ( int i = 0; i < temp.length; i++)
				printf("%x ", temp.data[i]);
			printf("\n");
			send_data_to_device((char*)temp.data, temp.length, addr);
			
			sprintf(temp_file_name, FILE_NAME_FORMAT, addr, 't');
			
			ofstream nf(temp_file_name, ios::binary|ios::out);
			while (f.read((char*)&temp, sizeof(temp))){
				nf.write((char*)&temp, sizeof(temp));
			}
			nf.close();
		}
		else
			puts("[LORA]: queue file is empty");
		
	}
	f.close();
	if (!is_empty){
		if (remove(file_name) != 0){
			perror("[LORA]: file removing error");
		}
		if (rename(temp_file_name, file_name) != 0){
			perror("[LORA]: file renaming error");
		}
	}
}
