LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := pra_test.c
LOCAL_MODULE := pra_test
LOCAL_CFLAGS += -pie -fPIE -std=c99
LOCAL_LDFLAGS += -pie -fPIE 
LOCAL_FORCE_STATIC_EXECUTABLE := true

include $(BUILD_EXECUTABLE)
