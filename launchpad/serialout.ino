void send(short a, short b, short c)
{
    writeShort(0xC080);
    writeShort(a);
    writeShort(b);
    writeShort(c);
}

void writeShort(short a)
{
    Serial.write((byte)(a >> 8));
    Serial.write((byte)(a & 0xFF));  
}

void sendPot(uint32_t a) {
    writeShort(0xC080);
    writeInt(a);
}

void writeInt(uint32_t a) {
    Serial.write((byte)(a >> 24));
    Serial.write((byte)(a >> 16));
    Serial.write((byte)(a >> 8));
    Serial.write((byte)(a));
}
