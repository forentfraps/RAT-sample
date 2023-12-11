from Crypto.Cipher import AES



_key = b'\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF'
_cipher = AES.new(_key, AES.MODE_ECB)

def ce(data, cipher = _cipher):
    data = bytes(data, encoding="ascii")
    data += b'\x00'
    while len(data)%16 != 0:
        # print(data[-5:])
        data += b'\x00'
    return list(map(int, bytearray(cipher.encrypt(data))))
def se(data, cipher = _cipher):
    _data = bytes(data, encoding="ascii")
    data = bytes()
    for elem in _data:
        data += bytes([elem])
        data += b'\x00'
    # print(list(map(hex, data)))
    data += b'\x00\x00'
    while len(data)%16 != 0:
        # print(data[-5:])
        data += b'\x00'
    return list(map(int, bytearray(cipher.encrypt(data))))

def be(data, cipher = _cipher):
    while len(data)%16 != 0:
        # print(data[-5:])
        data += b'\x00'
    return list(map(int, bytearray(cipher.encrypt(data))))