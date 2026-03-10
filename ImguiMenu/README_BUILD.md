# Jak skompilować mc_external.dll

## Wymagania
- **Visual Studio 2022** (Community lub wyżej)
- **Windows 10/11 SDK**

## Kroki

1. Otwórz plik `ImguiMenu/mc_external.sln` w Visual Studio 2022
2. Upewnij się że konfiguracja to **Release | x64** (górna belka VS)
3. Kliknij **Build → Build Solution** (albo `Ctrl+Shift+B`)
4. Gotowa DLL znajdzie się w folderze `ImguiMenu/bin/mc_external.dll`

## Inject

1. Uruchom **Minecraft Java Edition** (`javaw.exe`)
2. Otwórz **Extreme Injector**
3. Wybierz proces: `javaw.exe`
4. Ustaw metodę: **Manual Map**
5. Dodaj `mc_external.dll` i kliknij **Inject**
6. W grze naciśnij **prawy Shift** żeby otworzyć menu

## Uwagi
- Blur (rozmycie tła) nie działa na OpenGL - jest wyłączony
- Menu działa na Minecraft Java Edition (LWJGL / OpenGL)
- NIE działa na Minecraft Bedrock (to używa DX11)
