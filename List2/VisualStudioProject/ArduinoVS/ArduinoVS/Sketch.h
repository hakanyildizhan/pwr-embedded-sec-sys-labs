

bool messageToSend[114];

void setup()
{
    memset(messageToSend, false, sizeof(messageToSend));
}

void loop()
{
    
}

void generateMessage()
{
    for (int i = 0; i < sizeof(messageToSend); i++)
    {
        messageToSend[i] = random(2);
    }
}

