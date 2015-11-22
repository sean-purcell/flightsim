void send(short a, short b, short c, uint32_t d)
{
    writeShort(0xC080);
    writeShort(a);
    writeShort(b);
    writeShort(c);
    writeInt(d);
}

void writeShort(short a)
{
    Serial.write((byte)(a >> 8));
    Serial.write((byte)(a));  
}

void writeInt(uint32_t a) 
{
    Serial.write((byte)(a >> 24));
    Serial.write((byte)(a >> 16));
    Serial.write((byte)(a >> 8));
    Serial.write((byte)(a));
}
