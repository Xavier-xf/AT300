```
编译命令
mske
清除缓存命令
make clean
```
```
├── 3rdparty          // 第三方库或自定义模块源码
├── build             // 编译缓存
├── common            // 封装第三方库接口成公共接口
├── documents         // 说明文档
├── driver            // 平台驱动封装接口
├── includes          // 头文件
├── librarys          // 库文件
├── resource          // 资源文件
├── sources           // 功能源码目录
│   ├── layout        // ui布局
│   ├── platform      // 芯片平台功能二次封装接口
│   ├── user          // 根据功能需求二次封装接口
│   ├── main.c        // 主函数入口
├── 修改日志.txt      // 修改日志
├── CMakeLists.txt    // cmake
├── Makefile          // 用于编译工程makefile
└── README.md         // 工程说明
```