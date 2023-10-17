from Crypto.Cipher import AES
from typing import List


def enc_obj(path: str) -> List[int]:
    with open(path, "rb") as f:
        data = f.read()
    data = data[0x104:]
    while len(data)%16 != 0:
        # print(data[-5:])
        data += b'\x00'
    print([hex(b)[2:] for b in data])
    print("PLAINTEXT")
    print(data)
    print("CIPHERTEXT")
    key = b'\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF'
    cipher = AES.new(key, AES.MODE_ECB)
    plaintext = cipher.encrypt(data)
    print(plaintext)

    with open("payload_enc.c", "w") as f:

        f.write("unsigned char payload_enc_bytes[] ="+str([hex(byte) for byte in bytearray(plaintext)]).replace("'", "").replace("[", "{").replace("]", "}")+";")
print(enc_obj("./payload.o"))