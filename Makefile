gate.o:
	g++ -c main.cpp -o main.o

lora.o:
	g++ -c lora.cpp -o lora.o
	
arduPi_pi2.o:
	g++ -c arduPi_pi2.cpp -o arduPi_pi2.o

SX1272_pi2.o:
	g++ -c sx1272.cpp -o SX1272_pi2.o
mqtt.o:
	g++ -c mqtt_funct.cpp -o mqtt.o
	
gate: gate.o arduPi_pi2.o SX1272_pi2.o lora.o mqtt.o
	g++ -lpthread -lmosquitto main.o arduPi_pi2.o SX1272_pi2.o lora.o mqtt.o -o gate

clean:
	rm lora.o
	rm mqtt.o
	rm main.o
	rm gate

