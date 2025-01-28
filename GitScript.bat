@echo off
echo 执行 Git

REM 切换路径

REM 切换到分支 main
git-bash checkout main
REM main分支 合并 develop分支
git-bash merge develop
REM 切换到分支 develop
git-bash checkout develop

REM 推送到远程仓库
git-bash push origin develop
git-bash push origin main

echo 操作完成！
pause
