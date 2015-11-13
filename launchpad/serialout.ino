void send(short a, short b, short c)
{
    writeShort(0xFEFF);
    writeShort(a);
    writeShort(b);
    writeShort(c);
}

void writeShort(short a)
{
    Serial.write((byte)(a >> 8));
    Serial.write((byte)(a));  
}
