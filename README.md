# alsa_demo

## 日期 20160705##
## 使用环境 ##
&emsp;使用的是1.0.28或是1.0.27版本

&emsp;在新塘N3292x平台下交叉编译成功
## 说明 ##

# 交叉编译ALSA音频驱动 #

變異成靜態

	./configure --target=arm-linux --enable-shared=no 
	--enable-static=yes

編譯成動態

	./configure --target=arm-linux


更詳細的可以查看源碼目錄下的INSTALL文檔


#使用

alsa的PCM设置有两种方式

一种是以周期的方式设定
例子：

[Linuxvoicenew](/Linuxvoicenew.c)


另一种是以buffer缓冲区大小设定的

例子：

[alsa_dev](/alsa_dev.c)

对应关系如下图

![ALSA缓冲区拆分](/ALSA.jpg)

buffer总的缓冲区的大小，buffer下面分为几个周期，一个周期下面有几帧