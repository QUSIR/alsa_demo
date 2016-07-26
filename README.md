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