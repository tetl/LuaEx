
enum AddressType
{
	ADDR_SERVER,
	ADDR_ENGINE,
};

void *FindAddress(AddressType type, const char *sig, size_t len);