#define TEXT_SIZE 128

typedef enum{
    INIT,
    LIST,
    TO_ALL,
    TO_ONE,
    STOP,
    PING,
    NAME_TAKEN,
    SERVER_FULL,
    GET
} Type;

typedef struct{
    Type type;
    char text[TEXT_SIZE];
    char sender[TEXT_SIZE];
    char receiver[TEXT_SIZE];
} Msg;