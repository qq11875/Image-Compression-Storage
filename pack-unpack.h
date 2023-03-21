
unsigned int getBit(unsigned int pixel, int bit);

void ebcBlock_pack(Data *data);

void ebcBlock_unpack(Data *data);

void packData(Data *data, unsigned int *buffer, int *bit_in_buffer, int row, int column);

int unpackData(Data *data, unsigned int *buffer, int *bit_in_buffer, int row, int column, int *end, int *tmpHeight, int *tmpWidth);
