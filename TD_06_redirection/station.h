#define MSG_LEN 128

typedef enum {FREE, BUSY, ACK, STOP} Token;

typedef struct {
	Token m_token; 
	int station_origin;
	int station_dest;
	char msg[MSG_LEN];
} Packet;