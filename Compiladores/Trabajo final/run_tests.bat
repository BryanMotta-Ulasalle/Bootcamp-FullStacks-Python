@echo off
setlocal ENABLEDELAYEDEXPANSION

set EXE=mini0.exe
if not exist %EXE% (
  echo Ejecutable %EXE% no encontrado. Compila con:
  echo g++ -std=gnu++17 main.cpp lexer.cpp parser.cpp -o %EXE%
  pause
  exit /b 2
)

cd tests
echo ==== Ejecutando tests en %cd% ====

for %%F in (*.m0) do (
  echo.
  echo --- %%F ---
  ..\%EXE% "%%F"
  echo Exit code: !ERRORLEVEL!
)

echo.
echo ==== Fin de tests ====
pause
