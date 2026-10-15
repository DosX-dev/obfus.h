#ifndef OBFH_TEST_API_DECODE_H
#define OBFH_TEST_API_DECODE_H
#if OBFH_DISABLE_ANY_OBFUSCATION != 1
// Independent cold-resource oracle. Real calls separately exercise the ASM decoder.
static ULONG_PTR test_api_decode(ULONG_PTR value, unsigned int site) {
    ULONG_PTR key = OBFH_GUI_KEY(site), bias = OBFH_GUI_BIAS(site);
    unsigned int rotate = OBFH_GUI_ROTATE(site), form = OBFH_GUI_DRAW(site, 0x47554936u) & 3u;
    if (form == 0 || form == 2) value -= bias;
    if (form == 1 || form == 2) value ^= key;
    value = (value >> rotate) | (value << (sizeof(ULONG_PTR) * 8 - rotate));
    if (form == 1 || form == 3) value -= bias;
    if (form == 0 || form == 3) value ^= key;
    return value;
}
#endif
#endif
