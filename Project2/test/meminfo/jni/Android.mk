LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := meminfo.c
LOCAL_MODULE := meminfo
LOCAL_CFLAGS += -pie -fPIE -std=c99
LOCAL_LDFLAGS += -pie -fPIE 
LOCAL_FORCE_STATIC_EXECUTABLE := true

include $(BUILD_EXECUTABLE)
