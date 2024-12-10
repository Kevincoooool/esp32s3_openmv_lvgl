# esp32s3_openmv_lvgl
 
# 安装编译指南 #

## 1. 需要在Ubuntu下安装 ESP-IDF 依赖环境 ##
   当前验证过的环境：
   `idf: release v4.4.8`

## 2. 克隆工程 ##

   本工程代码使用submodule来管理openmv和micropython的官方代码，clone 本工程时:

   `git clone https://github.com/Kevincoooool/esp32s3_openmv_lvgl/`
   `cd esp32s3_openmv_lvgl`
   `git submodule update --init --recursive`

## 3. 编译工程 ##

   现在您可以从 `esp32s3_openmv_lvgl/project/ksdiy`编译您的固件
   
   首先进入 `/project/ksdiy` 文件目录

   - 执行 `make` 编译代码(首次编译，需要先执行make)
   
   如果编译没有出错，您在目录 `project/ksdiy/` 中应该可以看到新创建的 `build-GENERIC_S3`文件夹。在这个文件夹中，有编译生成的 `application.bin` 等文件。
   
   其它可能经常会用到指令包括

   - 执行 `make erase` 擦除模块的flash
   - 执行 `make flash` 编译并烧录固件

## 4. 串口工具 ##
   因为本工程固件支持repl模式，所以可以使用 pytty等串口工具来进行repl调试。
   [点击下载putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html)

## 5. 连接你的设备 ##

 将ESP32S3自带的USB端口连接上电脑，使用OpenMV IDE连接设备或者用Thonny IDE连接都可以进行编辑和运行。
 
## 6. 当前固件性能 ##

 RGB QVGA 15FPS
 JPEG VGA 20FPS
 
# TODO #

- 导入esp-sr，实现语音识别
- 导入esp-dl，基于esp-who实现人脸识别，提升openmv的识别速度
- 当前RGB图像 屏幕显示和openmv ide的显示是反色的，如果openmvide显示的话，屏幕就不正常，要屏幕正常的话，ide显示就不正常，需修复
- 提升USB到IDE的图像传输速度，特别是RGB图像传输时速度太慢，可尝试增大缓冲区
- 导入esp-adf的components，实现adf的mpy
