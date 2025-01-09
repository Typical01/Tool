
@echo off
chcp 65001
if exist "build" rmdir /s /q "build"
mkdir "build"
cd /d "build"
echo 切换到 C:\Users\22793\source\repos\Tool\build 目录

where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo -e "[31m未安装 cmake 或 cmake 不在系统 PATH 中![0m"
    pause
    exit /b 1
)
cmake ..
if %errorlevel% neq 0 (
    echo -e "[31mecho 运行 cmake.. 失败[0m"
    pause
    exit /b %errorlevel%
)
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo.
@echo -e "[32m运行 cmake 成功.[0m"
pause
