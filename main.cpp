#include "serialib.h"
//#include <crc.hpp>
#include <iostream>
#include <string>
#include <cstdint>

class serialCommunicator{
    private:
        serialib serial;
    public:
        serialCommunicator(std::string port){
            serial.openDevice(port.c_str(), 115200);
        }
        ~serialCommunicator(){
            serial.closeDevice();
        }
        void sendMessgae(uint8_t commandID, uint8_t message[] = {}, unsigned int size = 0){
            unsigned int serialMessSize = size + 3;
            uint8_t serialMessage[serialMessSize];
            serialMessage[0] = 0x32;
            serialMessage[1] = 0xAC;
            serialMessage[2] = commandID;
            for(uint8_t i = 0; i < size; i++){
                serialMessage[i+3] = message[i];
            }
            std::cout << "message:";
            for(uint8_t i=0; i<serialMessSize;i++){
                std::cout << static_cast <int> (serialMessage[i])<<", ";
            }
            serial.writeBytes(serialMessage, serialMessSize);
        }
};

int main()
{
    std::string ports[2] = {"com3", "com4"};
    serialCommunicator snakeSerial(ports[0]);
    uint8_t example[1] = {0xFF};
   // while(true){
        snakeSerial.sendMessgae(0x00, example, 1);
       // snakeSerial.sendMessgae(0x08);
    //}

    return 0 ;
}