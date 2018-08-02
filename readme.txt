1. 下载三方库

./download_dependencies_v2.sh
该命令会在当前目录生成downloads文件夹。

现在已经downloads目录中已经包含了依赖的三方库，
可以不用下载了。


2. 使用cmake进行交叉编译
  具体toolchain的名字可以在/data/public-space/toolchains/中查询。

编译x86_64版本
TC=linux-gcc-4.9-x86_64
mkdir $TC && cd $TC
cmake ../ -DTOOLCHAIN=$TC
make -j4 install


编译android 32位版本
TC=linux-gcc-4.9-android-softfp
mkdir $TC && cd $TC
cmake ../ -DTOOLCHAIN=$TC
make -j4 install

编译arm 64位版本
TC=linux-gcc-4.9-android-arm64-v8a
mkdir $TC && cd $TC
cmake ../ -DTOOLCHAIN=$TC
make -j4 install

编译arm 32位版本
TC=linux-gcc-4.9-arm-hard
mkdir $TC && cd $TC
cmake ../ -DTOOLCHAIN=$TC
make -j4 install





