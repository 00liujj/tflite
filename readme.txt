1. 下载三方库

./download_dependencies_v2.sh
该命令会在当前目录生成downloads文件夹。



2. 使用cmake进行交叉编译
  具体toolchain的名字可以在/data/public-space/toolchains中查询。

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
TC=linux-gcc-4.9-android-arm64
mkdir $TC && cd $TC
cmake ../ -DTOOLCHAIN=$TC
make -j4 install

编译arm 32位版本
TC=linux-gcc-4.9-arm-hard
mkdir $TC && cd $TC
cmake ../ -DTOOLCHAIN=$TC
make -j4 install





