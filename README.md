# QNeteaseMusic

网易云音乐第三方 Qt 客户端。

本软件仅供学习交流使用 请勿用于其他用途。

## 编译

测试平台使用 Qt 6.3.x 和 MSVC-2019。

```sh
$ qmake.exe -o Makefile ..\NeteaseMusic\NeteaseMusic.pro -spec win32-msvc "CONFIG+=debug" "CONFIG+=qml_debug"
$ jom.exe -f Makefile.Debug
```

或使用Qt Creator直接编译。

## 下载

[GitHub Release](https://github.com/cheese-233/QNeteaseMusic/releases/latest/)

目前仅编译Windows平台，Linux平台考虑后续增加支持。

## 使用

默认使用作者搭建的Api服务，您也可以自己搭建，参考[NeteaseCloudMusicApi](https://github.com/Binaryify/NeteaseCloudMusicApi)，完成后，请在[mainwindow.cpp](mainwindow.cpp)中修改
```cpp
QString NeteaseServer = "https://neteasecloudmusic.api.cheese233.cn.eu.org/";
```
为自己的服务器地址。

## 感谢

使用HarmonyOS Sans做默认字体，感谢[汉仪字库](https://www.hanyi.com.cn/custom-font)提供字体，字体版权归华为所有，本作品为非商业使用。

感谢[NeteaseCloudMusicApi](https://github.com/Binaryify/NeteaseCloudMusicApi)提供Api服务。

## License

[MIT](https://github.com/cheese-233/QNeteaseMusic/blob/master/LICENCE) © cheese-233
