
ifeq ($(COMPILEOS),$(LINUX_OS))
ifneq ($(TEL_PATH)/tools/tc32_gcc_v2.0.tar.bz2, $(wildcard $(TEL_PATH)/tools/tc32_gcc_v2.0.tar.bz2))
	@wget http://shyboy.oss-cn-shenzhen.aliyuncs.com/readonly/tc32_gcc_v2.0.tar.bz2
	@tar -xvjf tc32_gcc_v2.0.tar.bz2 -C $(TEL_PATH)/tools/linux/
endif
else
	@wget http://shyboy.oss-cn-shenzhen.aliyuncs.com/readonly/tc32_win.rar
	$tar -xvjf tc32_win.rar -C $(TEL_PATH)/tools/linux/
endif


