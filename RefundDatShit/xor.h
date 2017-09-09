#undef KEY
#undef BUFLEN

template <int XORSTART, int BUFLEN, int XREFKILLER>
class Xor
{
public:
	char s[BUFLEN];
	Xor(const char* xs);
#ifndef DEBUG_OPTION
	~Xor() { for (int i = 0; i<BUFLEN; i++)s[i] = 0; } // clear string from stack
#endif

private:
	Xor();
};

template <int XORSTART, int BUFLEN, int XREFKILLER>
Xor<XORSTART, BUFLEN, XREFKILLER>::Xor(const char* xs)
{
	int xvalue = XORSTART;
	int i = 0;
	for (; i<(BUFLEN - 1); i++) {
		s[i] = xs[i - XREFKILLER] ^ xvalue;
		xvalue += 1;
		xvalue %= 256;
	}
	s[BUFLEN - 1] = 0;
}
