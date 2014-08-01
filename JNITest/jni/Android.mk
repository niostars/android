LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := JNITest
LOCAL_SRC_FILES := JNITest.cpp

include $(BUILD_SHARED_LIBRARY)
