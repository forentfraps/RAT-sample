import winreg

def search_registry_for_string(root_key, sub_key, target_string):
    try:
        with winreg.OpenKey(root_key, sub_key, 0, winreg.KEY_READ) as key:
            for i in range(winreg.QueryInfoKey(key)[1]):
                name, value, _ = winreg.EnumValue(key, i)
                print(name)
                if target_string in str(value).lower():
                    print(f"Found '{target_string}' in '{name}' under '{sub_key}'")

    except FileNotFoundError:
        pass  # Ignore if the key does not exist

def search_registry_recursive(root_key, current_key, target_string):
    try:
        with winreg.OpenKey(root_key, current_key, 0, winreg.KEY_READ) as key:
            for i in range(winreg.QueryInfoKey(key)[0]):
                sub_key_name = winreg.EnumKey(key, i)
                sub_key_path = f"{current_key}\\{sub_key_name}"
                search_registry_recursive(root_key, sub_key_path, target_string)
            search_registry_for_string(root_key, current_key, target_string)

    except FileNotFoundError:
        pass  # Ignore if the key does not exist

if __name__ == "__main__":
    root_key = winreg.HKEY_LOCAL_MACHINE
    aReg = winreg.ConnectRegistry(None, winreg.HKEY_LOCAL_MACHINE)  # You can change this to any other root key you want to search
    target_string = "microsoft"  # Replace with the string you're looking for

    search_registry_recursive(aReg, "", target_string)
