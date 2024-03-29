//////////////////////////////////////////////////////////// 
//             Settings Configuration                      //
////////////////////////////////////////////////////////// 

/* special specs of NodeMCU pins */ 
//#define D0 16   //-pin is High on boot
//#define D1 5    //good
//#define D2 4    //good
//#define D3 0    //-boot failure if pulled low !!
//#define D4 2    //-boot failure if pulled low !!
//#define D5 14   //good  
//#define D6 12   //good  
//#define D7 13   //good
//#define D8 15 //-boot failure if pulled high !! must be low as in LDR with 4.7K R to GND
//#define RX 3    //-pin is High on boot , Output reversed

/* I/O pins selection of NodeMCU */ 
#define BedroomAmp           5
#define BedroomSpeakers            4

#define NVRPower            14

